cmake_minimum_required(VERSION 3.31)
project(
        DUSK
        VERSION 0.4
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

file(GLOB Server_RPC "Server/RPC/include/*" "Server/RPC/src/*")

file(GLOB Updates "Updates/include/*" "Updates/src/*")

file(GLOB Utils "Utils/*")

add_subdirectory(third_party/CLI11)
add_subdirectory(third_party/glaze)
add_subdirectory(third_party/json)
add_subdirectory(third_party/rpc)
add_subdirectory(third_party/mutex)
add_subdirectory(third_party/spdlog)
set(BUILD_TESTING OFF)
add_subdirectory(third_party/subprocess)

add_executable(DUSK
    ${Args}
    ${Auth}
    ${Events}
    ${Filesystem}
    ${Server_RPC}
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
        "GlazeTypes"
        "Server/RPC/include"
        "Updates/include"
        "Utils"

        "third_party/glaze/include"
        "third_party/mutex/include"
        "third_party/rpc/include"
        "third_party/subprocess/src/cpp"
)
target_link_libraries(
        DUSK PUBLIC
        CLI11::CLI11
        glaze::glaze
        rpc
        Td::TdStatic
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        subprocess
)

if (NOT DEFINED FS_DUSK_HOME OR NOT DEFINED API_ID OR NOT DEFINED API_HASH)
    message(FATAL_ERROR "You need to specify the FS_DUSK_HOME, API_ID and API_HASH variables")
endif ()

target_compile_definitions(
        DUSK PRIVATE
        DUSK_HOME="${FS_DUSK_HOME}"
        DUSK_ACCOUNTS="${FS_DUSK_HOME}/accounts"
        DUSK_CONFIG="${FS_DUSK_HOME}/config.json"
        DUSK_VERSION="${PROJECT_VERSION}"
        DUSK_TDLIB_TIMEOUT=4.0
        API_ID=${API_ID}
        API_HASH="${API_HASH}"
)

if (LINUX)
    target_compile_definitions(
            DUSK PRIVATE
            DUSK_TMP="/tmp/DUSK"
            OS_LINUX=1
    )
elseif (ANDROID)
    target_compile_definitions(
            DUSK PRIVATE
            DUSK_TMP="$ENV{HOME}/dusk_tmp"
            OS_ANDROID=1
    )
else ()
    target_compile_definitions(
            DUSK PRIVATE
            DUSK_TMP="${FS_DUSK_HOME}/tmp"
            OS_WINDOWS=1
    )
endif ()

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif ()