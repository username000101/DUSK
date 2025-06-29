use std::str::FromStr;

use super::module;

pub fn load_module(mod_dir: &mut std::path::PathBuf) -> module::Module {
    tracing::debug!("Loading module {}", mod_dir.to_str().unwrap().to_string());

    let mut mod_cfg_pb = mod_dir.clone();
    mod_cfg_pb.push("config.json");
    if !mod_cfg_pb.exists() {
        tracing::debug!("Not found mod config file; trying to get configuration from the mod");
        let mod_pb = mod_dir.clone();
        let _ = mod_pb.join("module.mod");
        unsafe {
            let lib = libloading::Library::new(mod_pb).unwrap();
            let config_fn: libloading::Symbol<unsafe extern "C" fn() -> *mut std::ffi::c_char> = lib.get(b"config\0").unwrap();
            serde_json::from_str::<module::Module>(std::ffi::CStr::from_ptr(config_fn()).to_str().unwrap()).unwrap()
        }
    } else {
        let content = std::fs::read_to_string(mod_cfg_pb).expect("Failed to read mod config file");
        serde_json::from_str::<module::Module>(content.as_str()).unwrap()
    }
}

pub fn load_modules(mods: &mut Vec<String> ) -> Vec<module::Module> {
    let mut result: Vec<module::Module> = Vec::new();

    for r#mod in mods {
        let mut mod_pb = std::path::PathBuf::from_str(r#mod).unwrap();

        if !mod_pb.exists() {
            tracing::warn!("Not found module directory {}", mod_pb.to_str().unwrap().to_string());
            continue;
        }

        let mut mod_file_pb = mod_pb.clone();
        mod_file_pb.push("module.mod");
        if !mod_file_pb.exists() {
            tracing::warn!("Not found module file {}", mod_file_pb.to_str().unwrap().to_string());
            continue;
        }

        result.push(load_module(&mut mod_pb));
    }
    
    tracing::info!("Loaded modules {}", result.len());
    result
}