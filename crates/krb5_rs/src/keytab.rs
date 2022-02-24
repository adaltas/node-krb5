use std::ffi::CString;
use std::mem::MaybeUninit;

use crate::{Context, Result};
use krb5_sys::{krb5_keytab, krb5_kt_close, krb5_kt_resolve};

use super::Krb5Error;

pub struct Keytab<'a> {
    pub(crate) context: &'a Context,
    pub(crate) inner: krb5_keytab,
}

impl<'a> Keytab<'a> {
    pub fn resolve(context: &'a Context, name: &str) -> Result<Keytab<'a>> {
        let mut keytab: MaybeUninit<krb5_keytab> = MaybeUninit::uninit();
        let name = CString::new(name).map_err(|_| Krb5Error::StringConversionError)?;
        let error_code =
            unsafe { krb5_kt_resolve(context.inner, name.as_ptr(), keytab.as_mut_ptr()) };
        Krb5Error::exit_if_library_error(context, error_code)?;
        Ok(Keytab {
            context,
            inner: unsafe { keytab.assume_init() },
        })
    }
}

impl<'a> Drop for Keytab<'a> {
    fn drop(&mut self) {
        if !self.context.inner.is_null() && !self.inner.is_null() {
            unsafe { krb5_kt_close(self.context.inner, self.inner) };
        }
    }
}
