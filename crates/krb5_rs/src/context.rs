use std::{ffi::CStr, mem::MaybeUninit, os::raw::c_char, sync::Mutex};

use crate::Result;
use krb5_sys::{
    krb5_context, krb5_error_code, krb5_free_context, krb5_free_default_realm,
    krb5_free_error_message, krb5_get_default_realm, krb5_get_error_message, krb5_init_context,
};

use super::Krb5Error;

static CONTEXT_LOCK: Mutex<()> = Mutex::new(());

pub struct Context {
    pub(crate) inner: krb5_context,
}

impl Context {
    pub fn get_default_realm(&self) -> Result<String> {
        let mut default_realm: MaybeUninit<*mut c_char> = MaybeUninit::zeroed();
        let error_code = unsafe { krb5_get_default_realm(self.inner, default_realm.as_mut_ptr()) };
        let default_realm = unsafe { default_realm.assume_init() };
        if error_code != 0 {
            unsafe { krb5_free_default_realm(self.inner, default_realm) }
            return Err(Krb5Error::from_library_error(self, error_code));
        }

        let result = unsafe {
            match CStr::from_ptr(default_realm).to_owned().into_string() {
                Ok(string) => Ok(string),
                Err(_) => Err(Krb5Error::StringConversionError),
            }
        };
        unsafe { krb5_free_default_realm(self.inner, default_realm) }
        result
    }

    pub(crate) fn get_error_message(&self, error_code: krb5_error_code) -> String {
        let c_str = unsafe { krb5_get_error_message(self.inner, error_code) };

        if c_str.is_null() {
            unsafe {
                krb5_free_error_message(self.inner, c_str);
            }
            return String::from("krb5 error message is null");
        }
        let result = unsafe {
            match CStr::from_ptr(c_str).to_owned().into_string() {
                Ok(string) => string,
                Err(_) => String::from("Failed to convert CString to String"),
            }
        };
        unsafe { krb5_free_error_message(self.inner, c_str) };
        result
    }

    pub fn new() -> Result<Context> {
        let _guard = &CONTEXT_LOCK.lock().expect("Failed to lock context");
        let mut krb5_context: MaybeUninit<krb5_context> = MaybeUninit::uninit();
        let error_code = unsafe { krb5_init_context(krb5_context.as_mut_ptr()) };
        if error_code != 0 {
            return Err(Krb5Error::ContextInitializationError);
        }

        let krb5_context = Context {
            inner: unsafe { krb5_context.assume_init() },
        };
        Ok(krb5_context)
    }
}

impl Drop for Context {
    fn drop(&mut self) {
        let _guard = &CONTEXT_LOCK.lock().expect("Failed to lock context");
        if !self.inner.is_null() {
            unsafe { krb5_free_context(self.inner) }
        }
    }
}
