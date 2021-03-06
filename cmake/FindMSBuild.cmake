# searches for Visual Studio MSBuild on Windows, and Mono MSBuild on *nix systems.

if(WIN32)
    set(VISUAL_STUDIO_TYPES Community Professional Enterprise)
    set(VISUAL_STUDIO_VERSIONS 2022 2019)
    set(PROGRAM_FILES_DIRECTORIES "Program Files" "Program Files (x86)")

    foreach(VS_TYPE ${VISUAL_STUDIO_TYPES})
        foreach(VS_VERSION ${VISUAL_STUDIO_VERSIONS})
            foreach(PF_DIR ${PROGRAM_FILES_DIRECTORIES})
                list(APPEND VISUAL_STUDIO_PATHS "C:\\${PF_DIR}\\Microsoft Visual Studio\\${VS_VERSION}\\${VS_TYPE}")
            endforeach()
        endforeach()
    endforeach()

    find_program(MSBUILD_EXE "MSBuild"
        PATHS ${VISUAL_STUDIO_PATHS}
        PATH_SUFFIXES "\\MSBuild\\Current\\Bin\\")

    set(MSBUILD_ARGS ${MSBUILD_EXE})
    set(MSBUILD_ERR_MESSAGE "Visual Studio 2022 not found! Please install Visual Studio at https://visualstudio.microsoft.com/downloads/.")
elseif(UNIX)
    set(MONO_PATHS
        "/Library/Frameworks/Mono.framework/Versions/Current/"
        "/usr/"
        "/usr/local/"
    )

    find_program(MSBUILD_EXE "mono"
        PATHS ${MONO_PATHS}
        PATH_SUFFIXES "bin/")

    find_file(MSBUILD_DLL "MSBuild.dll"
        PATHS ${MONO_PATHS}
        PATH_SUFFIXES "lib/mono/msbuild/Current/bin/")

    if(NOT "${MSBUILD_DLL}" STREQUAL "MSBUILD_DLL-NOTFOUND")
        set(MSBUILD_ARGS "${MSBUILD_EXE}" "${MSBUILD_DLL}")
    endif()

    set(MSBUILD_ERR_MESSAGE "Mono not found! Please install Mono at https://www.mono-project.com/download/stable/.")
endif()

set(MSBUILD_REQUIRED_VARS MSBUILD_ARGS MSBUILD_EXE)
mark_as_advanced(${MSBUILD_REQUIRED_VARS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MSBuild ${MSBUILD_ERR_MESSAGE} ${MSBUILD_REQUIRED_VARS})