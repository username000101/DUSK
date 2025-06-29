use crate::configuration;

fn write_default_user_config(path: std::path::PathBuf) {
    tracing::debug!("Writing default user config {}", path.clone().to_str().unwrap());
    let _ = std::fs::write(path.clone(), serde_json::to_string_pretty(&configuration::UserConfiguration{
        modules: vec![],
        modules_v: vec![],
        dir: path.parent().unwrap().to_str().unwrap().to_string(),
        id: path.parent().unwrap().file_name().unwrap().to_str().unwrap().to_string()
    }).unwrap()).expect("Failed to write to the user config");
}

pub fn add_user(dir: &mut std::path::PathBuf) {

    if !dir.exists() {
        tracing::debug!("Creating user dir {}", dir.clone().to_str().unwrap());
        std::fs::create_dir(dir.clone()).unwrap();
    }

    let mut dir_mods_pb = dir.clone();
    dir_mods_pb.push("mods");
    if !dir_mods_pb.exists() {
        tracing::debug!("Creating user mods dir {}", dir_mods_pb.clone().to_str().unwrap());
        std::fs::create_dir(dir_mods_pb).unwrap()
    }

    let mut dir_files_pb = dir.clone();
    dir_files_pb.push("files");
    if !dir_files_pb.exists() {
        tracing::debug!("Creating user files dir {}", dir_files_pb.clone().to_str().unwrap());
        std::fs::create_dir(dir_files_pb).unwrap();
    }

    let mut dir_db_pb = dir.clone();
    dir_db_pb.push("database");
    if !dir_db_pb.exists() {
        tracing::debug!("Creating user database dir {}", dir_db_pb.clone().to_str().unwrap());
        std::fs::create_dir(dir_db_pb).unwrap();
    }

    let mut dir_user_cfg_pb = dir.clone();
    dir_user_cfg_pb.push("config.json");
    if !dir_user_cfg_pb.exists() {
        tracing::debug!("Creating user config {}", dir_user_cfg_pb.clone().to_str().unwrap());
        write_default_user_config(dir_user_cfg_pb);
    }
}