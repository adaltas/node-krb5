use std::path::Path;

use napi::{Env, JsString, Task};
use napi_derive::napi;

use krb5_rs::{CCache, Context, Credentials, Keytab, Principal};

use crate::error::Error;

#[napi(object)]
#[derive(Debug)]
pub struct KinitParameters {
    pub principal: String,
    pub password: Option<String>,
    pub keytab: Option<String>,
    pub realm: Option<String>,
    pub ccname: Option<String>,
}

pub fn kinit_function(
    KinitParameters {
        principal,
        password,
        keytab,
        realm,
        ccname,
    }: &mut KinitParameters,
) -> Result<String, Error> {
    if password.is_none() && keytab.is_none() {
        return Err(Error::MissingPasswordOrKeytab);
    }

    let (principal, realm_from_split) = if principal.contains('@') {
        let splits: Vec<&str> = principal.split('@').collect();
        if splits.len() > 2 {
            return Err(Error::Generic(String::from(
                "Principal can contain at most one `@`",
            )));
        }
        (splits[0], Some(splits[1]))
    } else {
        (principal.as_str(), None)
    };
    let context = Context::new()?;
    let realm = match realm_from_split.or(realm.as_deref()) {
        Some(realm) => realm.to_owned(),
        None => context.get_default_realm()?,
    };
    let principal = Principal::build_principal(&context, realm.as_str(), principal)?;
    let ccache = match ccname {
        Some(ref cc_name) => CCache::resolve(&context, cc_name)?,
        None => CCache::default(&context)?,
    };
    let credentials = match keytab {
        Some(ref keytab) => {
            let keytab = Keytab::resolve(&context, keytab)?;
            Credentials::get_init_credentials_keytab(&context, &principal, &keytab)?
        }
        None => Credentials::get_init_credentials_password(
            &context,
            &principal,
            password.as_ref().unwrap(),
        )?,
    };
    let ccache_path = ccache.name()?;
    if !Path::new(&ccache_path).exists() {
        ccache.initialize(&principal)?;
    }
    ccache.store_creds(credentials)?;

    Ok(ccache_path)
}

pub struct Kinit {
    pub input: KinitParameters,
}

#[napi]
impl Task for Kinit {
    type Output = String;
    type JsValue = JsString;

    fn compute(&mut self) -> napi::Result<Self::Output> {
        kinit_function(&mut self.input).map_err(napi::Error::from)
    }

    fn resolve(&mut self, env: Env, output: Self::Output) -> napi::Result<Self::JsValue> {
        env.create_string(&output)
    }
}
