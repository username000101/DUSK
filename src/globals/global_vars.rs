use crate::configuration;

pub static CONFIG_FILE: once_cell::sync::OnceCell<String> = once_cell::sync::OnceCell::new();

pub static CURRENT_USER: once_cell::sync::OnceCell<i32> = once_cell::sync::OnceCell::new();

pub static CONFIG: once_cell::sync::OnceCell<std::sync::RwLock<configuration::Configuration>> = once_cell::sync::OnceCell::new();