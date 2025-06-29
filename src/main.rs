pub mod cli;
pub mod configuration;
pub mod filesystem;
pub mod globals;
pub mod json;
pub mod modules;
pub mod logger;
pub mod utils;

fn main() {
    let _guard = logger::init_log();
    unsafe { cli::parse_args(); }

    if cfg!(target_os = "windows") {
        tracing::info!("Setting vars for Windows");
        unsafe {
            std::env::set_var("CARGO_CFG_TARGET_OS", "windows");
            std::env::set_var("CARGO_CFG_TARGET_ARCH", "x86_64")
        }
    } else if cfg!(target_os = "linux") { 
        tracing::info!("Setting vars for Linux");
        unsafe {
            std::env::set_var("CARGO_CFG_TARGET_OS", "linux");
            std::env::set_var("CARGO_CFG_TARGET_ARCH", "x86_64")
        }
    } else if cfg!(target_os = "macos") {
        tracing::info!("Setting vars for MacOS");
        unsafe {
            std::env::set_var("CARGO_CFG_TARGET_OS", "macos");
            std::env::set_var("CARGO_CFG_TARGET_ARCH", "x86_64");
        }
    }

    tracing::debug!("\nAPI_ID: {}\nAPI_HASH: {}\nDUSK_HOME: {}\nCURRENT_USER: {}\nVERSION: {}\nCONFIG: {}", env!("DUSK_API_ID"), env!("DUSK_API_HASH"), env!("DUSK_HOME"), globals::CURRENT_USER.get().unwrap(), globals::CONFIG.get().unwrap().read().unwrap().version, env!("DUSK_CONFIG"));
    
    if cfg!(target_os = "android") {
        let tdlib_path = utils::input(&"Path to tdlib(installed)".to_string());
        tdlib_rs::build::build(Some(tdlib_path));
    } else {
        let cfg = globals::CONFIG.get().unwrap().read().unwrap();
        for module in cfg.users_v.first().unwrap().modules_v.iter().clone() {
            tracing::info!("Module info:\nid: {}\nentry_points: {:?}", module.id, module.entry_points);
        }
        std::thread::sleep(core::time::Duration::from_millis(10));
        unsafe { std::env::set_var("OUT_DIR", utils::input(&"Path to tdlib directory(install)".to_string())); }
        tdlib_rs::build::build(None);
    }
}