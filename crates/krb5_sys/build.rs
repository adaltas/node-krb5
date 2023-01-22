use std::{env, path::PathBuf};

#[cfg(target_os = "windows")]
use winreg::enums::*;
#[cfg(target_os = "windows")]
use winreg::RegKey;

#[cfg(target_os = "windows")]
fn link_library() -> Vec<PathBuf> {
    let krb5_home = env::var("KRB5_HOME");
    let path = match krb5_home {
        Ok(krb5_home) => krb5_home,
        Err(_) => {
            let hklm = RegKey::predef(HKEY_LOCAL_MACHINE);
            let mit_kerberos = hklm
                .open_subkey(r"SOFTWARE\MIT\Kerberos\SDK\4.1.0")
                .expect("Failed to find MIT Kerberos in WinRegistry");
            mit_kerberos
                .get_value("PathName")
                .expect("Failed to find MIT Kerberos path in WinRegistry")
        }
    };
    println!(r"cargo:rustc-link-search=native={}\lib\amd64\", path);
    println!("cargo:rustc-link-lib=dylib=krb5_64");
    println!("cargo:rustc-link-lib=dylib=gssapi64");
    vec![
        [&path, r"include"].iter().collect::<PathBuf>(),
        [&path, r"include\gssapi"].iter().collect::<PathBuf>(),
    ]
}

#[cfg(any(target_os = "linux", target_os = "macos"))]
fn link_library() -> Vec<PathBuf> {
    let krb5_home = env::var("KRB5_HOME");
    if let Ok(krb5_home) = krb5_home {
        let path: PathBuf = [&krb5_home, "lib", "pkgconfig"].iter().collect();
        if path.exists() {
            env::set_var("PKG_CONFIG_PATH", path);
        }
    }
    let krb5 = pkg_config::Config::new()
        .atleast_version("1.15")
        .probe("mit-krb5")
        .expect("Failed to bind libkrb5");

    let gssapi = pkg_config::Config::new()
        .atleast_version("1.15")
        .probe("mit-krb5-gssapi")
        .expect("Failed to bind libgssapi_krb5");

    let mut include_dirs = Vec::new();
    include_dirs.extend(krb5.include_paths);
    include_dirs.extend(gssapi.include_paths);
    include_dirs
}

fn main() {
    println!("cargo:rerun-if-env-changed=KRB5_HOME");
    let include_dirs = link_library();

    println!("cargo:rerun-if-changed=src/wrapper.h");

    let bindings = bindgen::Builder::default()
        // The input header we would like to generate
        // bindings for.
        .header("src/wrapper.h")
        .allowlist_var("krb5_.*")
        .allowlist_function("krb5_init_context")
        .allowlist_function("krb5_free_context")
        .allowlist_function("krb5_get_error_message")
        .allowlist_function("krb5_free_error_message")
        .allowlist_function("krb5_get_default_realm")
        .allowlist_function("krb5_free_default_realm")
        .allowlist_function("krb5_build_principal_ext")
        .allowlist_function("krb5_free_principal")
        .allowlist_function("krb5_cc_default")
        .allowlist_function("krb5_cc_initialize")
        .allowlist_function("krb5_cc_resolve")
        .allowlist_function("krb5_cc_close")
        .allowlist_function("krb5_cc_destroy")
        .allowlist_function("krb5_cc_store_cred")
        .allowlist_function("krb5_cc_get_name")
        .allowlist_function("krb5_get_init_creds_password")
        .allowlist_function("krb5_free_cred_contents")
        .allowlist_function("krb5_kt_resolve")
        .allowlist_function("krb5_get_init_creds_keytab")
        .allowlist_function("krb5_kt_close")
        .allowlist_function("gss_import_name")
        .allowlist_function("gss_release_name")
        .allowlist_function("gss_krb5_ccache_name")
        .allowlist_function("gss_init_sec_context")
        .allowlist_function("gss_delete_sec_context")
        .allowlist_function("gss_display_status")
        .allowlist_function("gss_release_buffer")
        .allowlist_function("gss_error")
        .allowlist_function("gss_c_nt_user_name")
        .allowlist_function("gss_c_nt_hostbased_service")
        .allowlist_type("gss_int32")
        .allowlist_var("GSS_C_NO_CREDENTIAL")
        .allowlist_var("GSS_C_NO_CHANNEL_BINDINGS")
        .allowlist_var("GSS_C_NO_CONTEXT")
        .clang_args(
            include_dirs
                .iter()
                .map(|include_dir| String::from("-I") + include_dir.to_str().unwrap())
                .collect::<Vec<String>>(),
        )
        // Tell cargo to invalidate the built crate whenever any of the
        // included header files changed.
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        // Finish the builder and generate the bindings.
        .generate()
        // Unwrap the Result and panic on failure.
        .expect("Unable to generate bindings");

    // Write the bindings to the $OUT_DIR/bindings.rs file.
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");

    println!("cargo:rerun-if-changed=src/wrapper.c");
    cc::Build::new()
        .includes(include_dirs)
        .include("src")
        .file("src/wrapper.c")
        .compile("gssapi_wrapper");
}
