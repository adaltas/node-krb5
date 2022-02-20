use krb5_rs::gssapi;
use napi::{Env, JsString, Task};
use napi_derive::napi;

use lazy_static::lazy_static;
use regex::Regex;

lazy_static! {
    static ref RE: Regex = Regex::new(".*[@]").unwrap();
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

pub fn generate_spnego_token_function(
    options: &mut GenerateSpnegoTokenParameters,
) -> std::result::Result<String, String> {
    let GenerateSpnegoTokenParameters {
        service_principal,
        service_fqdn,
        hostbased_service,
        ccname,
    } = options;

    let (service_principal, input_name_type) = if let Some(service) = service_principal {
        (service.to_owned(), "GSS_C_NT_USER_NAME")
    } else {
        let mut service = match service_fqdn {
            Some(service) => service.to_string(),
            None => hostbased_service
                .as_ref()
                .ok_or("Invalid service principal in input")?
                .to_string(),
        };

        if !RE.is_match(&service) {
            service.insert_str(0, "HTTP@");
        }
        (service.to_owned(), "GSS_C_NT_HOSTBASED_SERVICE")
    };

    let target_name = gssapi::import_name(&service_principal, input_name_type)?;

    if ccname.is_some() {
        gssapi::krb5_ccache_name(ccname.as_ref().unwrap())?;
    }
    let token = gssapi::get_token(target_name)?;
    Ok(base64::encode(token))
}

pub struct Spnego {
    pub input: GenerateSpnegoTokenParameters,
}

#[napi]
impl Task for Spnego {
    type Output = String;
    type JsValue = JsString;

    fn compute(&mut self) -> napi::Result<Self::Output> {
        generate_spnego_token_function(&mut self.input).map_err(napi::Error::from_reason)
    }

    fn resolve(&mut self, env: Env, output: Self::Output) -> napi::Result<Self::JsValue> {
        env.create_string(&output)
    }
}
