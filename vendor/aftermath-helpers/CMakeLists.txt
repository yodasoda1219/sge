file(GLOB AFTERMATH_HELPERS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_library(sge-aftermath-helpers STATIC ${AFTERMATH_HELPERS})
set_target_properties(sge-aftermath-helpers PROPERTIES CXX_STANDARD 17)
target_link_libraries(sge-aftermath-helpers PUBLIC ${Aftermath_LIBRARY} Vulkan-Headers)
target_include_directories(sge-aftermath-helpers PUBLIC ${Aftermath_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})