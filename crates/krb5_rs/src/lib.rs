mod error;
pub use error::*;

mod context;
pub use context::*;

mod credentials;
pub use credentials::*;

mod keytab;
pub use keytab::*;

mod principal;
pub use principal::*;

mod ccache;
pub use ccache::*;

pub mod gssapi;

#[cfg(test)]
mod test {
    use super::CCache;
    use super::Context;
    use super::Credentials;
    use super::Krb5Error;
    use super::Principal;
    use super::Result;

    #[test]
    fn test_init() -> Result<()> {
        let _krb5_context = Context::new()?;
        Ok(())
    }

    #[test]
    fn test_default_realm() -> Result<()> {
        let krb5_context = Context::new()?;
        let realm = krb5_context.get_default_realm()?;
        println!("Default realm: {}", realm);
        Ok(())
    }

    #[test]
    fn test_principal_data() -> Result<()> {
        let krb5_context = Context::new()?;
        let realm = krb5_context.get_default_realm()?;
        let _princ = Principal::build_principal(&krb5_context, &realm, "patrick")?;
        let _princ = Principal::build_principal(&krb5_context, &realm, "patrick/host")?;
        Ok(())
    }

    #[test]
    fn test_cc_default() -> Result<()> {
        let context = Context::new()?;
        let _ccache = CCache::default(&context)?;
        Ok(())
    }

    #[test]
    fn test_cc_resolve() -> Result<()> {
        let context = Context::new()?;
        let _ccache = CCache::resolve(&context, "pipo")?;
        Ok(())
    }

    #[test]
    fn test_cc_destroy_memory_ccache() -> Result<()> {
        let context = Context::new()?;
        let ccache = CCache::resolve(&context, "MEMORY:pipo")?;
        ccache.destroy()?;
        Ok(())
    }

    #[test]
    fn test_cc_destroy_unexisting_ccache() {
        let context = Context::new().unwrap();
        let ccache = CCache::resolve(&context, "/tmp/pipo").unwrap();
        match ccache.destroy() {
            Err(Krb5Error::LibraryError(_)) => assert!(true),
            Ok(_) => assert!(false, "Shouldn't be able to destroy a non-existing ccache"),
            _ => assert!(
                false,
                "Destroy a non-existing ccache should trigger a `Krb5Error::LibraryError`"
            ),
        }
    }

    #[test]
    fn test_store_creds_then_destroy() -> Result<()> {
        let context = Context::new()?;
        let principal =
            Principal::build_principal(&context, &context.get_default_realm()?, "admin")?;
        let ccache = CCache::default(&context)?;
        ccache.initialize(&principal)?;
        let credentials =
            Credentials::get_init_credentials_password(&context, &principal, "adm1n_p4ssw0rd")?;
        ccache.store_creds(credentials)?;
        ccache.destroy()?;
        Ok(())
    }
}
