use super::Context;
use krb5_sys::krb5_error_code;

use thiserror::Error;

pub type Result<T> = std::result::Result<T, Krb5Error>;

#[derive(Error, Debug)]
pub enum Krb5Error {
    #[error("{0}")]
    LibraryError(String),
    #[error("Failed to convert string returned from libkr5")]
    StringConversionError,
    #[error("Invalid principal")]
    InvalidPrincipal,
    #[error("Failed to initialize context")]
    ContextInitializationError,
}

impl Krb5Error {
    pub fn from_library_error(context: &Context, error_code: krb5_error_code) -> Krb5Error {
        Krb5Error::LibraryError(context.get_error_message(error_code))
    }

    pub(crate) fn exit_if_library_error(
        context: &Context,
        error_code: krb5_error_code,
    ) -> Result<()> {
        if error_code != 0 {
            Err(Krb5Error::from_library_error(context, error_code))
        } else {
            Ok(())
        }
    }
}
