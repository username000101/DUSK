#include "ModpackRead.hpp"

#include <libarchive-cpp.hpp>

std::filesystem::path modpack::ModpackReader::extract_modpack(std::filesystem::path into) {
    libarchive::Archive archive(this->file_);
    this->extracted_modpack_path_ = archive.extract(into);
    return this->extracted_modpack_path_;
}
