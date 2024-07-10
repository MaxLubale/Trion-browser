cmake_minimum_required(VERSION 3.10)
project(SimpleBrowser)

# Set CEF root directory
set(CEF_ROOT "/path/to/cef_binary")

# Add CEF CMake module
list(APPEND CMAKE_MODULE_PATH "${CEF_ROOT}/cmake")

# Include CEF configuration
include(CEF)

# Create a target for the application
add_executable(SimpleBrowser src/simple_browser.cpp)

# Add CEF as a dependency
add_dependencies(SimpleBrowser libcef_dll_wrapper)

# Link against the CEF libraries
target_link_libraries(SimpleBrowser libcef_dll_wrapper ${CEF_STANDARD_LIBS})

# Set C++11 standard
set_target_properties(SimpleBrowser PROPERTIES CXX_STANDARD 11)
