use std::{
    ffi::{c_void, CString},
    mem::MaybeUninit,
    slice,
};

use krb5_sys::{
    gss_OID, gss_OID_desc, gss_buffer_desc, gss_buffer_desc_struct, gss_c_nt_hostbased_service,
    gss_c_nt_user_name, gss_delete_sec_context, gss_display_status, gss_error, gss_import_name,
    gss_init_sec_context, gss_int32, gss_krb5_ccache_name, gss_name_t, gss_release_buffer,
    gss_release_name, OM_uint32, GSS_C_NO_CHANNEL_BINDINGS, GSS_C_NO_CONTEXT, GSS_C_NO_CREDENTIAL,
};

const MAX_AD_TOKEN_SIZE_BEFORE_B64: usize = 48000;

const GSS_C_REPLAY_FLAG: OM_uint32 = 4;
const GSS_C_SEQUENCE_FLAG: OM_uint32 = 8;
const GSS_C_INDEFINITE: OM_uint32 = 0xffffffff;
const GSS_C_GSS_CODE: gss_int32 = 1;

pub struct Name(gss_name_t);

impl Drop for Name {
    fn drop(&mut self) {
        if !self.0.is_null() {
            unsafe {
                gss_release_name(std::ptr::null_mut(), &mut self.0);
            }
        }
    }
}

pub fn import_name(principal: &str, input_name_type: &str) -> Result<Name, String> {
    let mut minor = 0;
    let mut service = gss_buffer_desc {
        length: principal.len(),
        value: principal.as_ptr() as *mut _,
    };
    let gss_oid = unsafe {
        if input_name_type == "GSS_C_NT_USER_NAME" {
            gss_c_nt_user_name()
        } else {
            gss_c_nt_hostbased_service()
        }
    };
    let mut desired_name: MaybeUninit<gss_name_t> = MaybeUninit::uninit();
    let error_code =
        unsafe { gss_import_name(&mut minor, &mut service, gss_oid, desired_name.as_mut_ptr()) };
    if unsafe { gss_error(error_code) } != 0 {
        return Err(convert_gss_error(error_code, minor));
    }

    let desired_name = unsafe { desired_name.assume_init() };

    Ok(Name(desired_name))
}

pub fn krb5_ccache_name(ccache_name: &str) -> Result<(), String> {
    let mut minor = 0;
    let ccache_name =
        CString::new(ccache_name).map_err(|_| "Failed to convert ccache name to a c string")?;

    let error_code =
        unsafe { gss_krb5_ccache_name(&mut minor, ccache_name.as_ptr(), std::ptr::null_mut()) };
    if unsafe { gss_error(error_code) } != 0 {
        return Err(convert_gss_error(error_code, minor));
    }
    Ok(())
}

pub fn get_token(target_name: Name) -> Result<Vec<u8>, String> {
    let mut minor: OM_uint32 = 0;
    let mut gss_context = unsafe { GSS_C_NO_CONTEXT };
    let mut gss_mech_spnego = gss_OID_desc {
        length: 6,
        elements: (*b"\x2b\x06\x01\x05\x05\x02").as_ptr() as *mut c_void,
    };
    let mut input_token: MaybeUninit<gss_buffer_desc_struct> = MaybeUninit::uninit();
    let mut output_token: MaybeUninit<gss_buffer_desc_struct> = MaybeUninit::uninit();
    let error_code = unsafe {
        gss_init_sec_context(
            &mut minor,
            GSS_C_NO_CREDENTIAL, // uses ccache specified with gss_krb5_ccache_name or default
            &mut gss_context,
            target_name.0,
            &mut gss_mech_spnego,
            GSS_C_REPLAY_FLAG | GSS_C_SEQUENCE_FLAG,
            GSS_C_INDEFINITE,
            GSS_C_NO_CHANNEL_BINDINGS,
            input_token.as_mut_ptr(), // make null
            std::ptr::null_mut(),
            output_token.as_mut_ptr(),
            std::ptr::null_mut(),
            std::ptr::null_mut(),
        )
    };
    let res = if unsafe { gss_error(error_code) } != 0 {
        Err(convert_gss_error(error_code, minor))
    } else {
        let output_token = unsafe { output_token.assume_init() };
        if output_token.length > MAX_AD_TOKEN_SIZE_BEFORE_B64 {
            Err(String::from(
                "The token returned by GSS is greater than the size allowed by Windows AD",
            ))
        } else {
            let size = output_token.length;
            let mut vec: Vec<u8> = Vec::with_capacity(size);
            unsafe {
                std::ptr::copy(output_token.value, vec.as_mut_ptr() as *mut _, size);
                vec.set_len(size);
            };

            Ok(vec)
        }
    };
    let error_code =
        unsafe { gss_delete_sec_context(&mut minor, &mut gss_context, output_token.as_mut_ptr()) };
    if unsafe { gss_error(error_code) } != 0 {
        let error_message = convert_gss_error(error_code, minor);
        if let Err(message) = res {
            Err(message + &error_message)
        } else {
            Err(error_message)
        }
    } else {
        res
    }
}

fn convert_gss_error(error_code: OM_uint32, minor: OM_uint32) -> String {
    let mut message_context = 0;
    let mut min_status = 0;
    let gss_c_no_oid: gss_OID = unsafe { MaybeUninit::zeroed().assume_init() };
    let mut status_string = gss_buffer_desc {
        length: 0,
        value: std::ptr::null_mut(),
    };
    let mut error_msg = Vec::new();
    loop {
        unsafe {
            gss_display_status(
                &mut min_status,
                error_code,
                GSS_C_GSS_CODE,
                gss_c_no_oid,
                &mut message_context,
                &mut status_string,
            )
        };
        let slice_from_data: &[u8] =
            unsafe { slice::from_raw_parts(status_string.value as *mut _, status_string.length) };
        error_msg.extend_from_slice(slice_from_data);
        unsafe { gss_release_buffer(&mut min_status, &mut status_string) };

        if message_context == 0 {
            break;
        }
    }
    error_msg.extend_from_slice(b" (minor ");
    error_msg.extend_from_slice(minor.to_string().as_bytes());
    error_msg.extend_from_slice(b")");

    unsafe { String::from_utf8_unchecked(error_msg) }
}
