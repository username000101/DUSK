#include "ModpackRead.hpp"

#include <libarchive-cpp.hpp>

std::expected<std::filesystem::path, std::string> modpack::ModpackReader::extract_modpack(std::filesystem::path into) {
    libarchive::Archive archive(this->file_);
    auto extract_result = archive.extract(into);
    if (extract_result.has_value()) {
        this->extracted_modpack_path_ = extract_result.value() / this->file_.stem();
        return this->extracted_modpack_path_;
    }
    else
        return extract_result;
}
