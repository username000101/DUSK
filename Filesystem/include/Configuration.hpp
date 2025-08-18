#pragma once

#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <utility>
#include <vector>

#include "Macros.hpp"

namespace config {
    struct BaseModuleInfo {
        std::filesystem::path file;
        std::uint16_t rpc_port;
        std::string get_config_rpc_function;
        std::string prefix;
    };

    struct Module {
        std::string name;
        std::string id;
        std::string author;
        std::string version;
        std::string description;
        std::filesystem::path file;
        std::filesystem::path directory;

        Module() = default;
        Module(const std::string& name_, const std::string& id_, const std::string& author_, const std::string& version_,
               const std::string& description_,
               const std::filesystem::path& file_, const std::filesystem::path& directory_) {
            if (!std::filesystem::exists(file_))
                throw std::runtime_error("Failed to create struct Module instance: file_(" + file_.string() + ") does not exist");
            if (!std::filesystem::exists(directory_) || !std::filesystem::is_directory(directory_))
                throw std::runtime_error("Failed to create struct Module instance: directory_(" + directory_.string() + " does not exist or it's not a directory");

            this->name = std::move(name_);
            this->id = std::move(id_);
            this->author = std::move(author_);
            this->version = std::move(version_);
            this->description = std::move(description_);
            this->file = file_;
            this->directory = directory_;
        }
    };

    class UserConfiguration {
    public:
        void load_modules();

        const std::vector<Module>& get_modules() const { return this->modules_; }
        const std::vector<std::int64_t>& get_blocked_requests() const { return this->blocked_requests_; }

        std::filesystem::path account_directory() { return this->user_account_directory_; }
        std::filesystem::path tdlib_files_directory() { return this->tdlib_files_directory_; }
        std::filesystem::path tdlib_database_directory() { return this->tdlib_database_directory_; }
        std::filesystem::path modules_directory() { return this->user_modules_directory_; }
        std::filesystem::path config() { return this->configuration_file_; }

        UserConfiguration() = default;
        UserConfiguration(const std::filesystem::path& account_dir, const std::filesystem::path& tdlib_files_dir,
                          const std::filesystem::path& tdlib_database_dir, const std::filesystem::path& user_modules_dir) {
            if (!std::filesystem::exists(account_dir) || !std::filesystem::is_directory(account_dir))
                throw std::runtime_error("Failed to create class UserConfiguration instance: accounts_dir(" + account_dir.string() + ") does not exist or it's not a directory");
            if (!std::filesystem::exists(tdlib_files_dir) || !std::filesystem::is_directory(tdlib_files_dir))
                throw std::runtime_error("Failed to create class UserConfiguration instance: tdlib_files_dir(" + tdlib_files_dir.string() + ") does not exist or it's not a directory");
            if (!std::filesystem::exists(tdlib_database_dir) || !std::filesystem::is_directory(tdlib_database_dir))
                throw std::runtime_error("Failed to create class UserConfiguration instance: tdlib_database_dir(" + tdlib_database_dir.string() + ") does not exist or it's not a directory");
            if (!std::filesystem::exists(user_modules_dir) || !std::filesystem::is_directory(user_modules_dir))
                throw std::runtime_error("Failed to create class UserConfiguration instance: user_modules_dir(" + user_modules_dir.string() + ") does not exist or it's not a directory");

            this->user_account_directory_ = account_dir;
            this->tdlib_files_directory_ = tdlib_files_dir;
            this->tdlib_database_directory_ = tdlib_database_dir;
            this->user_modules_directory_ = user_modules_dir;
            this->configuration_file_ = account_dir / "config.json";

            this->inl_parse_config();
        }

        explicit operator std::int64_t() const {
            return std::stoll(this->configuration_file_.parent_path().filename().string());
        }

        auto operator()() const { return static_cast<std::int64_t>(*this); }
    private:
        std::filesystem::path user_account_directory_;
        std::filesystem::path tdlib_files_directory_;
        std::filesystem::path tdlib_database_directory_;
        std::filesystem::path user_modules_directory_;
        std::filesystem::path configuration_file_;

        std::int64_t id_ = 0;
        std::string prefix_;
        std::vector<Module> modules_;
        std::vector<BaseModuleInfo> modules_base_;
        std::vector<std::int64_t> blocked_requests_;

        void inl_parse_config();
    };

    class Configuration {
    public:
        Configuration() = default;
        Configuration(const std::string& version_,
            const std::vector<UserConfiguration>& users_) {
            this->version = version_;
            this->users = users_;
        }

        static Configuration parse_file(const std::filesystem::path& file);

        std::string version;
        std::vector<Module> modules; /* DUSK-level modules*/
        std::vector<UserConfiguration> users;
        UserConfiguration current_user;
    };
}
