use krb5_rs::Krb5Error;
use thiserror::Error;

#[derive(Error, Debug)]
pub enum Error {
    #[error("{0}")]
    Library(Krb5Error),
    #[error("Please provide either a password or a keytab")]
    MissingPasswordOrKeytab,
    #[error("{0}")]
    Generic(String),
}

impl From<Krb5Error> for Error {
    fn from(error: Krb5Error) -> Self {
        Error::Library(error)
    }
}

impl From<Error> for napi::Error {
    fn from(error: Error) -> Self {
        napi::Error::from_reason(error.to_string())
    }
}
