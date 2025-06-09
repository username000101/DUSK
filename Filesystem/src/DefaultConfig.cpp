#include "FS.hpp"

nlohmann::json filesystem::default_config() {
    nlohmann::json config;
    config["accounts_dir"] = DUSK_ACCOUNTS;
    config["version"] = DUSK_VERSION;
    return config;
}
