cmake_minimum_required(VERSION 3.31)
project(
        DUSK
        VERSION 0.1
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(FTXUI_ENABLE_INSTALL OFF)
set(DUSK_TDLIB_USE_TEST_DC OFF)

find_package(Td REQUIRED)

file(GLOB Args "Args/include/*" "Args/src/*")

file(GLOB Auth "Auth/include/*" "Auth/src/*")

file(GLOB Events "Events/include/*" "Events/src/*")

file(GLOB Filesystem "Filesystem/include/*" "Filesystem/src/*")

file(GLOB Modpack "Modpack/include/*" "Modpack/src/*")
add_subdirectory(Modpack/third_party/libarchive-cpp)

file(GLOB Updates "Updates/include/*" "Updates/src/*")

file(GLOB Utils "Utils/*")

add_subdirectory(third_party/FTXUI)
add_subdirectory(third_party/json)
add_subdirectory(third_party/mutex)
add_subdirectory(third_party/spdlog)

add_executable(DUSK
    ${Args}
    ${Auth}
    ${Events}
    ${Filesystem}
    ${Modpack}
    ${Updates}
    ${Utils}
    main.cpp
)

target_include_directories(
        DUSK PRIVATE
        "Args/include"
        "Auth/include"
        "Events/include"
        "Filesystem/include"
        "Modpack/include"
        "Updates/include"
        "Utils"

        "third_party/mutex/include"
)
target_link_libraries(
        DUSK PUBLIC
        archive-cpp
        CLI11::CLI11
        ftxui::component
        ftxui::dom
        ftxui::screen
        Td::TdStatic
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)
if (NOT MSVC)
    target_compile_options(
            DUSK PRIVATE
            -flto
    )
    target_link_options(
            DUSK PRIVATE
            -Wl,--export-dynamic
    )
endif ()

if (NOT DEFINED FS_DUSK_HOME OR NOT DEFINED API_ID OR NOT DEFINED API_HASH)
    message(FATAL_ERROR "You need to specify the FS_DUSK_HOME, API_ID and API_HASH variables")
else ()
    target_compile_definitions(
            DUSK PRIVATE
            DUSK_HOME="${FS_DUSK_HOME}"
            DUSK_ACCOUNTS="${FS_DUSK_HOME}/accounts"
            DUSK_CONFIG="${FS_DUSK_HOME}/config.json"
            DUSK_VERSION="${PROJECT_VERSION}"
            DUSK_TMP="/tmp/DUSK"
            DUSK_TDLIB_TIMEOUT=5.0
            API_ID=${API_ID}
            API_HASH="${API_HASH}"
    )
endif ()
