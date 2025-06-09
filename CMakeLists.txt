cmake_minimum_required(VERSION 3.31)
project(
        DUSK
        VERSION 0.1
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(FTXUI_ENABLE_INSTALL OFF)

find_package(Td REQUIRED)

function(add_definitions_to_targets)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs TARGETS DEFINITIONS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    foreach(target IN LISTS ARG_TARGETS)
        target_compile_definitions(${target} PUBLIC ${ARG_DEFINITIONS})
    endforeach()
endfunction()

add_subdirectory(third_party/FTXUI)
add_subdirectory(third_party/json)
add_subdirectory(third_party/spdlog)

file(GLOB Args_FILES "Args/include/*" "Args/src/*")
add_library(args SHARED ${Args_FILES})
target_include_directories(args PUBLIC "Args/include" "Utils")
add_subdirectory(Args/third_party/CLI11)
target_link_libraries(args PUBLIC CLI11::CLI11 ftxui::component ftxui::dom ftxui::screen nlohmann_json::nlohmann_json)

file(GLOB Auth_FILES "Auth/include/*" "Auth/src/*")
add_library(tdauth SHARED ${Auth_FILES})
target_include_directories(tdauth PUBLIC "Auth/include")
target_link_libraries(tdauth PUBLIC Td::TdStatic spdlog::spdlog)

file(GLOB Events_FILES "Events/include/*" "Events/src/*")
add_library(events SHARED ${Events_FILES})
target_include_directories(events PUBLIC "Events/include")

file(GLOB RuntimeContext_FILES "RuntimeContext/include/*" "RuntimeContext/src/*")
add_library(runtime_context SHARED ${RuntimeContext_FILES})
target_include_directories(runtime_context PUBLIC "RuntimeContext/include")

file(GLOB Filesystem_FILES "Filesystem/include/*" "Filesystem/src/*")
add_library(filesystem SHARED ${Filesystem_FILES})
if (NOT FS_DUSK_HOME)
    message(FATAL_ERROR "You need to set the FS_DUSK_HOME variable")
endif ()
target_include_directories(filesystem PUBLIC "Filesystem/include" "Utils")
target_link_libraries(filesystem PUBLIC spdlog::spdlog)

file(GLOB Modpack_FILES "Modpack/include/*" "Modpack/src/*")
add_library(modpack SHARED ${Modpack_FILES})
target_include_directories(modpack PUBLIC "Modpack/include")
add_subdirectory(Modpack/third_party/libarchive-cpp)
target_link_libraries(modpack PUBLIC archive-cpp spdlog::spdlog)

file(GLOB Utils_FILES "Utils/include/*")

add_executable(DUSK
        main.cpp
        ${Args_FILES}
        ${Auth_FILES}
        ${Events_FILES}
        ${RuntimeContext_FILES}
        ${Filesystem_FILES}
        ${Modpack_FILES}
        ${Utils_FILES}
)
target_include_directories(DUSK PUBLIC "Utils")
target_compile_definitions(DUSK PUBLIC DUSK_VERSION="${PROJECT_VERSION}")
target_link_libraries(DUSK PRIVATE args tdauth events filesystem runtime_context spdlog::spdlog modpack)

add_definitions_to_targets(
        TARGETS args filesystem
        DEFINITIONS DUSK_HOME="${FS_DUSK_HOME}" DUSK_CONFIG="${FS_DUSK_HOME}/config.json" DUSK_ACCOUNTS="${FS_DUSK_HOME}/accounts" DUSK_VERSION="${PROJECT_VERSION}"
)