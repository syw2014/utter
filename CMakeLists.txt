#
# Main CMakelists for the boilerplate project.
#
cmake_minimum_required(VERSION 3.14)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Do not build in-source. Please remove CMakeCache.txt and the CMakeFiles/ directory. Then build out-of-source.")
endif()

# Put the project early since modules might need to detect the compiler.
# More information https://cmake.org/cmake/help/latest/command/project.html
project(
    "UTTER" # This will exposed as the variable PROJECT_NAME.
    VERSION 0.1.0 # Used for installation and defines variables PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH, and PROJECT_VERSION_TWEAK.
    LANGUAGES C CXX # Used to determine the languages to use based on file extensions
)

############################
##  Modules and scripts   ##
############################

# Standard CMake modules

include(CTest)                          # Must be called before adding tests but after calling project(). This automatically calls enable_testing() and configures ctest targets when using Make/Ninja
include(CMakeDependentOption)           # This is a really useful scripts that creates options that depends on other options. It can even be used with generator expressions !
include(GNUInstallDirs)                 # This will define the default values for installation directories (all platforms even if named GNU)
include(InstallRequiredSystemLibraries) # Tell CMake that the `install` target needs to install required system libraries (eg: Windows SDK)
include(CMakePackageConfigHelpers)      # Helper to create relocatable packages

# Custom modules and scripts

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake") # Make our cmake scripts available


###############
##  OPTIONS  ##
###############

# You should try to give as much control over the project setup to the user.
# When modifying compile flags for example, if they are not mandatory, provide an option.

option(${PROJECT_NAME}_USE_DOXYGEN "Add a doxygen target to generate the documentation" ON)
option(${PROJECT_NAME}_USE_ADDITIONAL_SOURCEFILE "Use the additional source file" ON)
option(${PROJECT_NAME}_INSTALL "Should ${PROJECT_NAME} be added to the install list? Useful if included using add_subdirectory." ON)

# Use your own option for tests, in case people use your library through add_subdirectory
cmake_dependent_option(${PROJECT_NAME}_BUILD_TESTS
    "Enable ${PROJECT_NAME} project tests targets" ON # By default we want tests if CTest is enabled
    "BUILD_TESTING" OFF # Stay coherent with CTest variables
)


# It is always easier to navigate in an IDE when projects are organized in folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Whe building a shared library, you do not want to export all symbols by default
# gcc (and hence clang) are wrong about this.
#
# For more information, see https://gcc.gnu.org/wiki/Visibility and https://www.youtube.com/embed/m0DwB4OvDXk
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)


# 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -fpermissive")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall")


###############
##Set Directory##
###############
set(UTTER_SOURCE_DIR "./src")
set(UTTER_EXAMPLE_DIR "./example")
set(THIRD_PARTY_DIR "./third-party")


###############
##  Project  ##
###############

# add include directory
include_directories("${UTTER_SOURCE_DIR}/")
include_directories("${UTTER_SOURCE_DIR}/common/")
include_directories("${UTTER_SOURCE_DIR}/StringMatching/")



# 
set(demo_cc "${UTTER_EXAMPLE_DIR}/unit_test.cpp"
    "${UTTER_SOURCE_DIR}/common/common.cc"
    "${UTTER_SOURCE_DIR}/StringMatching/ahocorasick.cc"
)


add_executable(Demo ${demo_cc})


