pub(crate) mod error;
mod kdestroy;
mod kinit;
mod spnego;

use kdestroy::{Kdestroy, KdestroyParameters};
use kinit::{Kinit, KinitParameters};
use napi::bindgen_prelude::AsyncTask;
use napi_derive::napi;
use spnego::{GenerateSpnegoTokenParameters, Spnego};

#[napi]
pub fn kinit(input: KinitParameters) -> AsyncTask<Kinit> {
    AsyncTask::new(Kinit { input })
}

#[napi(ts_return_type = "Promise<void>")]
pub fn kdestroy(input: KdestroyParameters) -> AsyncTask<Kdestroy> {
    AsyncTask::new(Kdestroy { input })
}

#[napi]
pub fn spnego(input: GenerateSpnegoTokenParameters) -> AsyncTask<Spnego> {
    AsyncTask::new(Spnego { input })
}
