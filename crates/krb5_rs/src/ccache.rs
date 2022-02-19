use std::{
    ffi::{CStr, CString},
    mem::MaybeUninit,
};

use krb5_sys::{
    krb5_cc_close, krb5_cc_default, krb5_cc_destroy, krb5_cc_get_name, krb5_cc_initialize,
    krb5_cc_resolve, krb5_cc_store_cred, krb5_ccache,
};

use super::{Context, Credentials, Krb5Error, Principal, Result};

pub struct CCache<'a> {
    pub(crate) context: &'a Context,
    pub(crate) inner: krb5_ccache,
}

impl<'a> CCache<'a> {
    pub fn default(context: &'a Context) -> Result<CCache> {
        let mut ccache: MaybeUninit<krb5_ccache> = MaybeUninit::uninit();
        let error_code = unsafe { krb5_cc_default(context.inner, ccache.as_mut_ptr()) };
        Krb5Error::exit_if_library_error(context, error_code)?;

        Ok(CCache {
            context,
            inner: unsafe { ccache.assume_init() },
        })
    }

    pub fn resolve(context: &'a Context, cc_name: &str) -> Result<CCache<'a>> {
        let mut ccache: MaybeUninit<krb5_ccache> = MaybeUninit::uninit();

        let cc_name = CString::new(cc_name).unwrap();

        let error_code =
            unsafe { krb5_cc_resolve(context.inner, cc_name.as_ptr(), ccache.as_mut_ptr()) };
        Krb5Error::exit_if_library_error(context, error_code)?;
        Ok(CCache {
            context,
            inner: unsafe { ccache.assume_init() },
        })
    }

    pub fn destroy(mut self) -> Result<()> {
        if !self.context.inner.is_null() && !self.inner.is_null() {
            let error_code = unsafe { krb5_cc_destroy(self.context.inner, self.inner) };
            self.inner = std::ptr::null_mut() as *mut _;
            Krb5Error::exit_if_library_error(self.context, error_code)?;
        }
        Ok(())
    }

    pub fn initialize(&self, principal: &Principal) -> Result<()> {
        let error_code =
            unsafe { krb5_cc_initialize(self.context.inner, self.inner, principal.inner) };
        Krb5Error::exit_if_library_error(self.context, error_code)?;

        Ok(())
    }

    pub fn store_creds(&self, mut credentials: Credentials) -> Result<()> {
        let error_code =
            unsafe { krb5_cc_store_cred(self.context.inner, self.inner, &mut credentials.inner) };
        Krb5Error::exit_if_library_error(self.context, error_code)?;
        Ok(())
    }

    pub fn name(&self) -> Result<String> {
        let ccache_name = unsafe { krb5_cc_get_name(self.context.inner, self.inner) };
        unsafe {
            let cstr = CStr::from_ptr(ccache_name);
            match cstr.to_owned().into_string() {
                Ok(ccache_name) => Ok(ccache_name),
                Err(_) => Err(Krb5Error::StringConversionError),
            }
        }
    }
}

impl<'a> Drop for CCache<'a> {
    fn drop(&mut self) {
        if !self.context.inner.is_null() && !self.inner.is_null() {
            unsafe { krb5_cc_close(self.context.inner, self.inner) };
        }
    }
}
