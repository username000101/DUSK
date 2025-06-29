use std::str::FromStr;

use crate::configuration;
use crate::globals;

fn write_default_config() {
    tracing::debug!("Writing default config");
    let config_file = globals::CONFIG_FILE.get().expect("Config file is empty");
    let accounts_dir = std::path::Path::new(config_file).to_path_buf();
    let _ = std::fs::write(globals::CONFIG_FILE.get().unwrap(), serde_json::to_string_pretty(&configuration::Configuration{
        accounts_dir: accounts_dir.parent().unwrap().join("accounts").to_str().unwrap().to_string(),
        version: env!("CARGO_PKG_VERSION").to_string(),
        users: vec![],
        users_v: vec![]
    }).unwrap());
}

pub fn check_fs() {
    let home_dir_pb = std::path::PathBuf::from_str(env!("DUSK_HOME").trim_matches('"')).unwrap();
    if !home_dir_pb.exists() {
        tracing::debug!("Creating DUSK home directory {}", home_dir_pb.clone().to_str().unwrap());
        
        std::fs::create_dir(home_dir_pb.clone()).expect("Failed to create directory");
    }
    
    let mut config_pb = home_dir_pb.clone();
    config_pb.push("config.json");
    if !config_pb.exists() {
        write_default_config();
    }

    let accounts_dir_pb = home_dir_pb.join("accounts");
    if !accounts_dir_pb.exists() {
        tracing::debug!("Creating DUSK accounts directory {}", accounts_dir_pb.clone().to_str().unwrap());
        std::fs::create_dir(accounts_dir_pb).unwrap();
    }
}