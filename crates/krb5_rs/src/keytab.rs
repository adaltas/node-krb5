use std::ffi::CString;
use std::mem::MaybeUninit;

use crate::{Context, Result};
use krb5_sys::{krb5_keytab, krb5_kt_close, krb5_kt_resolve};

use super::Krb5Error;

pub struct Keytab<'context> {
    pub(crate) context: &'context Context,
    pub(crate) inner: krb5_keytab,
}

impl<'context> Keytab<'context> {
    pub fn resolve(context: &'context Context, name: &str) -> Result<Keytab<'context>> {
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

impl<'context> Drop for Keytab<'context> {
    fn drop(&mut self) {
        if !self.context.inner.is_null() && !self.inner.is_null() {
            unsafe { krb5_kt_close(self.context.inner, self.inner) };
        }
    }
}
