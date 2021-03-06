cmake_minimum_required(VERSION 3.10)

# set up base variables
set(VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor")
set(ASSET_DIR "${CMAKE_SOURCE_DIR}/assets")
set(SGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# find mono libs
if(SGE_PLATFORM_WINDOWS)
    set(MONO_PLATFORM windows)
elseif(SGE_PLATFORM_MACOSX)
    set(MONO_PLATFORM osx)
elseif(SGE_PLATFORM_LINUX)
    set(MONO_PLATFORM linux)
endif()
file(GLOB MONO_LIBS CONFIGURE_DEPENDS "${VENDOR_DIR}/mono/lib/${MONO_PLATFORM}/${SGE_TARGET_ARCHITECTURE}/*")

# msbuild shenanigans
if(SGE_BUILD_SCRIPTCORE)
    set(SGE_DOTNET_EXE ${DOTNET_EXE})
endif()

if(SGE_BUILD_DEBUGGER)
    set(SGE_MSBUILD_ARGS "")
    foreach(ARGUMENT ${MSBUILD_ARGS})
        set(SGE_MSBUILD_ARGS "${SGE_MSBUILD_ARGS}\"${ARGUMENT}\", ")
    endforeach()
    
    set(SGE_MSBUILD_EXE ${MSBUILD_EXE})
    set(SGE_MSBUILD_ARGS "{ ${SGE_MSBUILD_ARGS} }")
endif()


# configuration header
set(SGE_CONFIG "${CMAKE_CURRENT_BINARY_DIR}/sge_config.h")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/sge_config.h.in" ${SGE_CONFIG} @ONLY)

# base lists
set(SGE_INCLUDES ${SGE_DIR} "${VENDOR_DIR}/mono/include" "${VENDOR_DIR}/glm" ${CMAKE_CURRENT_BINARY_DIR})
set(SGE_LIBS spdlog shaderc spirv-cross-glsl stb EnTT box2d sge-imgui nlohmann_json ${MONO_LIBS})

# nvidia nsight aftermath
if(SGE_USE_AFTERMATH)
    list(APPEND SGE_LIBS sge-aftermath-helpers)
    list(APPEND SGE_DEFINES SGE_AFTERMATH_LINKED)
endif()

# platform detection
if(SGE_PLATFORM_DESKTOP)
    list(APPEND SGE_DEFINES SGE_PLATFORM_DESKTOP)
    list(APPEND SGE_LIBS glfw)
    list(APPEND ENABLED_PLATFORMS desktop)

    if(SGE_PLATFORM_WINDOWS)
        list(APPEND SGE_DEFINES SGE_PLATFORM_WINDOWS)
        list(APPEND ENABLED_PLATFORMS windows)
    elseif(SGE_PLATFORM_MACOSX)
        list(APPEND SGE_DEFINES SGE_PLATFORM_MACOSX)
        list(APPEND ENABLED_PLATFORMS macosx)
    elseif(SGE_PLATFORM_LINUX)
        list(APPEND SGE_DEFINES SGE_PLATFORM_LINUX)
        list(APPEND SGE_LIBS pthread stdc++fs)
        list(APPEND ENABLED_PLATFORMS linux)
    endif()
else()
    message(FATAL_ERROR "Other systems are not currently supported!")
endif()

# graphics api
if(SGE_USE_VULKAN)
    list(APPEND SGE_LIBS volk vma)
    list(APPEND SGE_DEFINES SGE_USE_VULKAN)
    list(APPEND ENABLED_PLATFORMS vulkan)
endif()

# glob sources
file(GLOB_RECURSE SGE_SOURCE CONFIGURE_DEPENDS "${SGE_DIR}/*")
file(GLOB_RECURSE PLATFORM_CODE CONFIGURE_DEPENDS "${SGE_DIR}/sge/platform/*")
list(REMOVE_ITEM SGE_SOURCE ${PLATFORM_CODE})
foreach(PLATFORM_NAME ${ENABLED_PLATFORMS})
    file(GLOB_RECURSE GLOBBED_PLATFORM_CODE CONFIGURE_DEPENDS "${SGE_DIR}/sge/platform/${PLATFORM_NAME}/*")
    list(APPEND SGE_SOURCE ${GLOBBED_PLATFORM_CODE})
endforeach()

# glob shaders
file(GLOB_RECURSE SGE_SHADERS CONFIGURE_DEPENDS "${ASSET_DIR}/shaders/*")
if(MSVC)
    source_group("shaders" FILES ${SGE_SHADERS})
    set_source_files_properties(${SGE_SHADERS} PROPERTIES VS_TOOL_OVERRIDE "None")
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL Windows)
    set(LICENSE_DESC_SEPARATOR ";")
else()
    set(LICENSE_DESC_SEPARATOR ":")
endif()

# type face directory generation
set(TYPE_FACE_DIRECTORY_FILE "${CMAKE_CURRENT_BINARY_DIR}/type_face_directory.cpp")
add_custom_command(OUTPUT ${TYPE_FACE_DIRECTORY_FILE}
    COMMAND embed_type_faces
    "${CMAKE_SOURCE_DIR}/assets/fonts"
    ${TYPE_FACE_DIRECTORY_FILE}
    "${CMAKE_SOURCE_DIR}/LICENSE${LICENSE_DESC_SEPARATOR}190"
    COMMENT "Generating embedded SGE type face directory"
    DEPENDS "${CMAKE_SOURCE_DIR}/assets/fonts/" embed_type_faces)

# add target
set(SGE_MANIFEST ${SGE_SOURCE} ${SGE_SHADERS} ${TYPE_FACE_DIRECTORY_FILE} ${SGE_CONFIG})
add_library(sge STATIC ${SGE_MANIFEST})
target_link_libraries(sge PUBLIC ${SGE_LIBS})
target_include_directories(sge PUBLIC ${SGE_INCLUDES})
target_compile_definitions(sge PUBLIC ${SGE_DEFINES} $<$<CONFIG:Debug>:SGE_DEBUG>
                               PRIVATE SGE_INTERNAL)
set_target_properties(sge PROPERTIES CXX_STANDARD 17 FOLDER "core")

# source groups
foreach(SGE_SOURCE_FILE ${SGE_MANIFEST})
    cmake_path(GET SGE_SOURCE_FILE PARENT_PATH DIRECTORY)
    cmake_path(RELATIVE_PATH DIRECTORY
        BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src"
        OUTPUT_VARIABLE SOURCE_GROUP_NAME)

    string(FIND ${SOURCE_GROUP_NAME} "." PERIOD_INDEX)
    if(${PERIOD_INDEX} LESS 0)
        source_group(${SOURCE_GROUP_NAME} FILES ${SGE_SOURCE_FILE})
    endif()
endforeach()

# setup pch
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.16)
    set(SGE_PCH "${SGE_DIR}/sgepch.h")
    message(STATUS "SGE: Using precompiled header: ${SGE_PCH}")
    target_precompile_headers(sge PRIVATE ${SGE_PCH})
endif()

message(STATUS "SGE: Linking against libraries: ${SGE_LIBS}")
message(STATUS "SGE: Including directories: ${SGE_INCLUDES}")
message(STATUS "SGE: Defining preprocessor macros: ${SGE_DEFINES}")
message(STATUS "SGE: Enabled platform code: ${ENABLED_PLATFORMS}")
message(STATUS "Building SGE version ${SGE_VERSION}")