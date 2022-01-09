cmake_minimum_required(VERSION 3.10)
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
file(GLOB IMGUI_CORE CONFIGURE_DEPENDS "${IMGUI_DIR}/*.cpp" "${IMGUI_DIR}/*.h" "${IMGUI_DIR}/misc/cpp/imgui_stdlib.*")
set(IMGUI_INCLUDES ${IMGUI_DIR})

if(SGE_DESKTOP)
    list(APPEND ENABLED_IMGUI_BACKENDS glfw)
    list(APPEND IMGUI_LINKS glfw)
endif()

if(Vulkan_FOUND)
    list(APPEND ENABLED_IMGUI_BACKENDS vulkan)
    list(APPEND IMGUI_LINKS ${Vulkan_LIBRARY})
    list(APPEND IMGUI_INCLUDES ${Vulkan_INCLUDE_DIR})
endif()

message(STATUS "Enabled Dear ImGui backends: ${ENABLED_IMGUI_BACKENDS}")
foreach(BACKEND_NAME ${ENABLED_IMGUI_BACKENDS})
    file(GLOB BACKEND_SOURCE CONFIGURE_DEPENDS "${IMGUI_DIR}/backends/imgui_impl_${BACKEND_NAME}.*")
    list(APPEND IMGUI_BACKEND_SOURCE ${BACKEND_SOURCE})
endforeach()

set(IMGUI_MANIFEST ${IMGUI_CORE} ${IMGUI_BACKEND_SOURCE})
add_library(sge-imgui STATIC ${IMGUI_MANIFEST})
set_target_properties(sge-imgui PROPERTIES CXX_STANDARD 17)
target_include_directories(sge-imgui PUBLIC ${IMGUI_INCLUDES})
if(DEFINED IMGUI_LINKS)
    target_link_libraries(sge-imgui PUBLIC ${IMGUI_LINKS})
endif()