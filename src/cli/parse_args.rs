use crate::configuration;
use crate::filesystem;
use crate::globals;

pub unsafe fn parse_args() {
    let result = clap::Command::new("DUSK")
        .version(env!("CARGO_PKG_VERSION"))
        .author("username000101")
        .about("The Telegram userbot")
        .arg(clap::Arg::new("reinit")
                 .short('w')
                 .long("reinit")
                 .help("Rewrite configuration file"))
        .arg(clap::Arg::new("user")
                 .short('u')
                 .long("user")
                 .help("Specify the user"))
        .arg_required_else_help(true)
        .get_matches();
    
    if let Some(user) = result.get_one::<String>("user") {
        globals::CONFIG_FILE.set(env!("DUSK_CONFIG").trim_matches('"').to_string()).unwrap();
        filesystem::check_fs();
        let config_str = std::fs::read_to_string(env!("DUSK_CONFIG").trim_matches('"')).expect("Failed to read from config");
        let _ = globals::CONFIG.set(std::sync::RwLock::new(serde_json::from_str::<configuration::Configuration>(&config_str).unwrap()));
        unsafe { configuration::load_users(); }
        globals::CURRENT_USER.set(user.parse::<i32>().unwrap()).unwrap();
        let user_pb = std::path::Path::new(env!("DUSK_HOME").trim_matches('"')).to_path_buf();
        configuration::add_user(&mut user_pb.join("accounts").join(user));
    }
}