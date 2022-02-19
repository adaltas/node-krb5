use std::{ffi::CString, mem::MaybeUninit};

use krb5_sys::{
    krb5_creds, krb5_free_cred_contents, krb5_get_init_creds_keytab, krb5_get_init_creds_password,
};

use crate::{Context, Keytab, Krb5Error, Principal, Result};

pub struct Credentials<'a> {
    pub(crate) context: &'a Context,
    pub(crate) inner: krb5_creds,
}

impl<'a> Credentials<'a> {
    pub fn get_init_credentials_password(
        context: &'a Context,
        principal: &Principal,
        password: &str,
    ) -> Result<Credentials<'a>> {
        let mut credentials: MaybeUninit<krb5_creds> = MaybeUninit::uninit();
        let password = CString::new(password).map_err(|_err| Krb5Error::StringConversionError)?;
        let error_code = unsafe {
            krb5_get_init_creds_password(
                context.inner,
                credentials.as_mut_ptr(),
                principal.inner,
                password.as_ptr(),
                None,
                std::ptr::null_mut() as *mut _,
                0,
                std::ptr::null() as *const _,
                std::ptr::null_mut() as *mut _,
            )
        };
        Krb5Error::exit_if_library_error(context, error_code)?;

        Ok(Credentials {
            context,
            inner: unsafe { credentials.assume_init() },
        })
    }

    pub fn get_init_credentials_keytab(
        context: &'a Context,
        principal: &Principal,
        keytab: &Keytab,
    ) -> Result<Credentials<'a>> {
        let mut credentials: MaybeUninit<krb5_creds> = MaybeUninit::uninit();
        let error_code = unsafe {
            krb5_get_init_creds_keytab(
                context.inner,
                credentials.as_mut_ptr(),
                principal.inner,
                keytab.inner,
                0,
                std::ptr::null_mut() as *mut _,
                std::ptr::null_mut() as *mut _,
            )
        };
        Krb5Error::exit_if_library_error(context, error_code)?;

        Ok(Credentials {
            context,
            inner: unsafe { credentials.assume_init() },
        })
    }
}

impl<'a> Drop for Credentials<'a> {
    fn drop(&mut self) {
        if !self.context.inner.is_null() {
            unsafe { krb5_free_cred_contents(self.context.inner, &mut self.inner) }
        }
    }
}
