use crate::error::Error;
use krb5_rs::{CCache, Context};

use napi::{Env, JsUndefined, Task};
use napi_derive::napi;

#[napi(object)]
#[derive(Debug)]
pub struct KdestroyParameters {
    pub ccname: Option<String>,
}

pub fn kdestroy_function(options: &mut KdestroyParameters) -> std::result::Result<(), Error> {
    let context = Context::new()?;
    let ccache = match options.ccname {
        Some(ref cc_name) => CCache::resolve(&context, cc_name)?,
        None => CCache::default(&context)?,
    };
    ccache.destroy()?;

    Ok(())
}

pub struct Kdestroy {
    pub input: KdestroyParameters,
}

#[napi]
impl Task for Kdestroy {
    type Output = ();
    type JsValue = JsUndefined;

    fn compute(&mut self) -> napi::Result<Self::Output> {
        kdestroy_function(&mut self.input).map_err(napi::Error::from)
    }
    
    fn resolve(&mut self, env: Env, _output: Self::Output) -> napi::Result<Self::JsValue> {
        env.get_undefined()
    }
}
