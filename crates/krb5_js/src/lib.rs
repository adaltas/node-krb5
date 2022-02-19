use std::path::Path;

use krb5_rs::{gssapi, CCache, Context, Credentials, Keytab, Principal};
use lazy_static::lazy_static;
use napi::tokio;
use napi_derive::napi;

mod error;
use error::Error;
use regex::Regex;

lazy_static! {
    static ref RE: Regex = Regex::new(".*[@]").unwrap();
}

#[napi(object)]
#[derive(Debug)]
pub struct KinitParameters {
    pub principal: String,
    pub password: Option<String>,
    pub keytab: Option<String>,
    pub realm: Option<String>,
    pub ccname: Option<String>,
}

fn kinit_function(parameters: KinitParameters) -> Result<String, Error> {
    let KinitParameters {
        principal,
        password,
        keytab,
        realm,
        ccname,
    } = parameters;
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
    let realm = match realm_from_split.or_else(|| realm.as_deref()) {
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
        None => {
            Credentials::get_init_credentials_password(&context, &principal, &password.unwrap())?
        }
    };
    let ccache_path = ccache.name()?;
    if !Path::new(&ccache_path).exists() {
        ccache.initialize(&principal)?;
    }
    ccache.store_creds(credentials)?;

    Ok(ccache_path)
}

#[napi(object)]
#[derive(Debug)]
pub struct KdestroyParameters {
    pub ccname: Option<String>,
}

fn kdestroy_function(options: KdestroyParameters) -> std::result::Result<(), Error> {
    let context = Context::new()?;
    let ccache = match options.ccname {
        Some(ref cc_name) => CCache::resolve(&context, cc_name)?,
        None => CCache::default(&context)?,
    };
    ccache.destroy()?;

    Ok(())
}

#[napi(object)]
#[derive(Debug)]
pub struct GenerateSpnegoTokenParameters {
    #[napi(js_name = "service_principal")]
    pub service_principal: Option<String>,
    #[napi(js_name = "service_fqdn")]
    pub service_fqdn: Option<String>,
    #[napi(js_name = "hostbased_service")]
    pub hostbased_service: Option<String>,
    pub ccname: Option<String>,
}

fn generate_spnego_token_function(
    options: GenerateSpnegoTokenParameters,
) -> std::result::Result<String, String> {
    let GenerateSpnegoTokenParameters {
        service_principal,
        service_fqdn,
        hostbased_service,
        ccname,
    } = options;

    let (service_principal, input_name_type) = if let Some(service) = service_principal {
        (service, String::from("GSS_C_NT_USER_NAME"))
    } else {
        let mut service = service_fqdn
            .or(hostbased_service)
            .ok_or_else(|| String::from("Invalid service principal in input"))?;
        if !RE.is_match(&service) {
            service.insert_str(0, "HTTP@");
        }
        (service, String::from("GSS_C_NT_HOSTBASED_SERVICE"))
    };

    let target_name = gssapi::import_name(&service_principal, &input_name_type)?;

    if ccname.is_some() {
        gssapi::krb5_ccache_name(&ccname.unwrap())?;
    }
    let token = gssapi::get_token(target_name)?;
    Ok(base64::encode(token))
}

#[napi]
pub async fn kinit(options: KinitParameters) -> napi::Result<String> {
    tokio::task::spawn(async move { kinit_function(options).map_err(napi::Error::from) })
        .await
        .unwrap()
}

#[napi]
pub async fn kdestroy(options: KdestroyParameters) -> napi::Result<()> {
    tokio::task::spawn(async move { kdestroy_function(options).map_err(napi::Error::from) })
        .await
        .unwrap()
}

#[napi]
pub async fn spnego(options: GenerateSpnegoTokenParameters) -> napi::Result<String> {
    tokio::task::spawn(async move {
        generate_spnego_token_function(options).map_err(napi::Error::from_reason)
    })
    .await
    .unwrap()
}
