use std::{mem::MaybeUninit, os::raw::c_uint};

use crate::{Context, Krb5Error, Result};
use krb5_sys::{krb5_build_principal_ext, krb5_free_principal, krb5_principal};

pub struct Principal<'context> {
    pub(crate) context: &'context Context,
    pub(crate) inner: krb5_principal,
}

impl<'context> Principal<'context> {
    pub fn build_principal(
        context: &'context Context,
        realm: &str,
        user: &str,
    ) -> Result<Principal<'context>> {
        let mut krb5_principal: MaybeUninit<krb5_principal> = MaybeUninit::uninit();
        let sp: Vec<&str> = user.split('/').collect();
        let error_code = match sp.len() {
            1 => unsafe {
                let user = sp[0];

                krb5_build_principal_ext(
                    context.inner,
                    krb5_principal.as_mut_ptr(),
                    realm.len() as c_uint,
                    realm.as_ptr() as *const i8,
                    user.len() as c_uint,
                    user.as_ptr(),
                    0,
                )
            },
            2 => unsafe {
                let user = sp[0];
                let host = sp[1];
                krb5_build_principal_ext(
                    context.inner,
                    krb5_principal.as_mut_ptr(),
                    realm.len() as c_uint,
                    realm.as_ptr() as *const i8,
                    user.len() as c_uint,
                    user.as_ptr(),
                    host.len() as c_uint,
                    host.as_ptr(),
                    0,
                )
            },
            _ => return Err(Krb5Error::InvalidPrincipal),
        };
        Krb5Error::exit_if_library_error(context, error_code)?;

        Ok(Principal {
            context,
            inner: unsafe { krb5_principal.assume_init() },
        })
    }
}

impl<'context> Drop for Principal<'context> {
    fn drop(&mut self) {
        if !self.context.inner.is_null() && !self.inner.is_null() {
            unsafe { krb5_free_principal(self.context.inner, self.inner) }
        }
    }
}
