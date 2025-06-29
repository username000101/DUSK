use crate::configuration;
use crate::globals::CONFIG;
use crate::modules::load_modules;

pub unsafe fn load_users() {
    let users = &mut CONFIG.get().unwrap().write().unwrap();
    for user in users.users.clone() {

        let mut path_user = std::path::Path::new(&user).to_path_buf();
        if !path_user.exists() {
            tracing::warn!("Not found files for user {}", &user);
            configuration::add_user(&mut path_user);
            continue;
        }

        path_user.push("config.json");
        let user_cfg_str = std::fs::read_to_string(path_user).unwrap();
        let mut user_ = serde_json::from_str::<configuration::UserConfiguration>(user_cfg_str.as_str()).unwrap();
        user_.modules_v = load_modules(&mut user_.modules);
        users.users_v.push(user_);
    }

    if users.users_v.is_empty() {
        panic!("Loaded users: 0(maybe user can't be added in {}?)", env!("DUSK_CONFIG"));
    }
}