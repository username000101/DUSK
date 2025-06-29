use crate::globals;
use crate::configuration;
pub fn write_default_config() {
    let cfg_def = configuration::Configuration {
        version: env!("CARGO_PKG_VERSION").to_string(),
        accounts_dir: "".to_string(),
        users: vec![],
        users_v: vec![]
    };
    std::fs::write(globals::CONFIG_FILE.get().unwrap(), serde_json::to_string(&cfg_def).unwrap()).expect("Failed to write default config");
    tracing::info!("Default config will be written to {}", globals::CONFIG_FILE.get().unwrap());
}