fn main() {
    let api_id_str = std::env::var("DUSK_API_ID").expect("You need to specify the DUSK_API_ID environment variable");
    let api_id = api_id_str.parse::<i64>().expect("DUSK_API_ID has invalid value");

    let api_hash = std::env::var("DUSK_API_HASH").expect("You need to specify the DUSK_API_HASH environment variable");

    #[allow(unused_assignments)]
    let mut dusk_home_dir: String = String::new();
    let mut dusk_config_file: String = String::new();
    
    let dusk_home_dir_r = std::env::var_os("DUSK_HOME_DIR");
    if dusk_home_dir_r.is_none() {
        println!("cargo:warning=The DUSK_HOME_DIR variable is not set; using default path");
        #[allow(unused_assignments)]
        let mut default_home_dir: String = String::new();
        if cfg!(target_os = "windows") {
            default_home_dir = std::env::var_os("USERPROFILE").expect("USERPROFILE is not set").into_string().unwrap();
            default_home_dir += "\\DUSK";
        } else if cfg!(target_os = "linux") || cfg!(target_os = "android") {
            default_home_dir = std::env::var_os("HOME").expect("HOME is not set").into_string().unwrap();
            default_home_dir += "/DUSK";
        } else {
            panic!("This system is not supported now");
        }

        dusk_home_dir = default_home_dir.clone();
        dusk_config_file = default_home_dir + "/config.json"; 
    } else {
        dusk_home_dir = dusk_home_dir_r.unwrap().into_string().unwrap()
    }

    println!("cargo:rustc-env=API_ID={}", api_id);
    println!("cargo:rustc-env=API_HASH=\"{}\"", api_hash);
    println!("cargo:rustc-env=DUSK_HOME=\"{}\"", dusk_home_dir);
    println!("cargo:rustc-env=DUSK_CONFIG=\"{}\"", dusk_config_file);
}