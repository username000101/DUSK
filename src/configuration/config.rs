use serde::{Serialize, Deserialize};

use crate::modules;

#[derive(Serialize, Deserialize)]
pub struct UserConfiguration {
    pub dir: String,
    pub id: String,
    pub modules: Vec<String>,
#[serde(skip)]
    pub modules_v: Vec<modules::Module>
}

#[derive(Serialize, Deserialize)]
pub struct Configuration {
    pub version: String,
    pub accounts_dir: String,
    pub users: Vec<String>,
#[serde(skip)]
    pub users_v: Vec<UserConfiguration>
}