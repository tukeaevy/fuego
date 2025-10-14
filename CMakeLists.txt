cmake_minimum_required(VERSION 3.10)

# Set CMake policies to handle newer Boost versions
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

include(CheckCXXCompilerFlag)
set(VERSION "0.2")
# Packaged from main commits
set(COMMIT 72946d9)
set(REFS " (HEAD -> master)")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CONFIGURATION_TYPES Debug RelWithDebInfo Release CACHE STRING INTERNAL)
set(CMAKE_SKIP_INSTALL_RULES ON)
set(CMAKE_SKIP_PACKAGE_ALL_DEPENDENCY ON)
set(CMAKE_SUPPRESS_REGENERATION ON)

project(fuegoX)

include_directories(include src external "${CMAKE_BINARY_DIR}/version")

if(APPLE OR FREEBSD)
  include_directories(SYSTEM /usr/include/malloc)
  if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
  endif()
  enable_language(ASM)
endif()

if(MSVC)
  include_directories(src/Platform/Windows)
elseif(APPLE)
  include_directories(src/Platform/OSX)
else()
	include_directories(src/Platform/Linux)
	include(TestCXXAcceptsFlag)
	include(CheckCCompilerFlag)
  if (NOT ARCH)
    set(ARCH native CACHE STRING "CPU to build for: -march value or 'default' to not pass -march at all")
  endif()
  message(STATUS "Building on ${CMAKE_SYSTEM_PROCESSOR}")
  if(ARCH STREQUAL "default")
    set(ARCH_FLAG "")
  else()
    set(ARCH_FLAG "-march=${ARCH}")
    if(ARCH STREQUAL "native")
      check_c_compiler_flag(-march=native CC_SUPPORTS_MARCH_NATIVE)
      if (NOT CC_SUPPORTS_MARCH_NATIVE)
        check_c_compiler_flag(-mtune=native CC_SUPPORTS_MTUNE_NATIVE)
        if (CC_SUPPORTS_MTUNE_NATIVE)
          set(ARCH_FLAG "-mtune=${ARCH}")
        else()
          set(ARCH_FLAG "")
        endif()
      endif()
      if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        set(ARCH_FLAG "-march=x86-64")
      endif()
    endif()
	endif()
endif()

include (arm.cmake)

# Check support for AES
option(NO_AES "Explicitly disable AES support" ${NO_AES})

if(NO_AES) # No AES
	message(STATUS "AES support explicitly disabled")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNO_AES")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNO_AES")

elseif(NOT ARM) # x86/64
	message(STATUS "AES support enabled")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -maes")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -maes")

endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARCH_FLAG}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARCH_FLAG}")

set(STATIC ${MSVC} CACHE BOOL "Link libraries statically")

if(MSVC)
  add_definitions("/bigobj /MP /W3 /GS- /D_CRT_SECURE_NO_WARNINGS /wd4996 /wd4345 /D_WIN32_WINNT=0x0600 /DWIN32_LEAN_AND_MEAN /DGTEST_HAS_TR1_TUPLE=0 /D_VARIADIC_MAX=8 /D__SSE4_1__")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:10485760")
  if(STATIC)
    foreach(VAR CMAKE_C_FLAGS_DEBUG CMAKE_CXX_FLAGS_DEBUG CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELEASE)
      string(REPLACE "/MD" "/MT" ${VAR} "${${VAR}}")
    endforeach()
  endif()
  include_directories(SYSTEM src/platform/msc)
  add_definitions(/DHAVE_ROTR)
else()
  if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    # This option has no effect in glibc version less than 2.20.
    # Since glibc 2.20 _BSD_SOURCE is deprecated, this macro is recomended instead
    add_definitions("-D_DEFAULT_SOURCE -D_GNU_SOURCE")
	endif()

  set(WARNINGS "-Wall -Wextra -Wpointer-arith -Wundef -Wvla -Wwrite-strings -Wno-error=extra -Wno-error=unused-function -Wno-error=deprecated-declarations -Wno-error=sign-compare -Wno-error=strict-aliasing -Wno-error=type-limits -Wno-unused-parameter -Wno-error=unused-variable -Wno-error=undef -Wno-error=uninitialized -Wno-error=unused-result")
  if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(WARNINGS "${WARNINGS} -Wno-error=mismatched-tags -Wno-error=null-conversion -Wno-overloaded-shift-op-parentheses -Wno-error=shift-count-overflow -Wno-error=tautological-constant-out-of-range-compare -Wno-error=unused-private-field -Wno-error=unneeded-internal-declaration -Wno-error=unused-function")
    add_definitions(/DHAVE_ROTR)
  else()
    set(WARNINGS "${WARNINGS} -Wlogical-op -Wno-error=maybe-uninitialized -Wno-error=clobbered -Wno-error=unused-but-set-variable")
  endif()
  if(MINGW)
    set(WARNINGS "${WARNINGS} -Wno-error=unused-value")
    set(MINGW_FLAG "-DWIN32_LEAN_AND_MEAN")
    include_directories(SYSTEM src/platform/mingw)
  else()
    set(MINGW_FLAG "")
  endif()
  if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND NOT (CMAKE_C_COMPILER_VERSION VERSION_LESS 5.1))
    set(WARNINGS "${WARNINGS} -Wno-error=odr")
    add_definitions(/DHAVE_ROTR)
  endif()
  set(C_WARNINGS "-Waggregate-return -Wnested-externs -Wold-style-definition -Wstrict-prototypes")
  set(CXX_WARNINGS "-Wno-reorder -Wno-missing-field-initializers")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 ${MINGW_FLAG} ${WARNINGS} ${C_WARNINGS}")
  if(NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 ${MINGW_FLAG} ${WARNINGS} ${CXX_WARNINGS}")
  if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_HAS_TR1_TUPLE=0")
  endif()
  if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND NOT (CMAKE_C_COMPILER_VERSION VERSION_LESS 4.8))
    set(DEBUG_FLAGS "-g3 -Og")
  else()
    set(DEBUG_FLAGS "-g3 -O0")
  endif()
  set(RELEASE_FLAGS "-g3 -O0")

endif()

if(APPLE)
	add_definitions(/DHAVE_ROTR)
endif()

if(STATIC)
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_USE_STATIC_RUNTIME ON)
endif()

# Set policy to use legacy FindBoost module for Boost 1.70+ compatibility
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 OLD)
endif()

#set(Boost_DEBUG on)
# Force use of FindBoost module instead of BoostConfig.cmake for compatibility
set(Boost_USE_STATIC_LIBS ON)
set(Boost_NO_BOOST_CMAKE ON)

# Debug output for platform detection
message(STATUS "APPLE: ${APPLE}")
message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")

# For macOS with Homebrew, manually set Boost paths to avoid BoostConfig.cmake
if(APPLE AND CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  # Try multiple possible Homebrew paths
  if(EXISTS "/opt/homebrew")
    set(BOOST_ROOT "/opt/homebrew")
    set(Boost_INCLUDE_DIR "/opt/homebrew/include")
    set(Boost_LIBRARY_DIR "/opt/homebrew/lib")
    # Manually set library paths to avoid BoostConfig.cmake
    # Look for various library file patterns and extensions
    find_library(Boost_system_LIBRARY 
      NAMES boost_system libboost_system boost_system-mt libboost_system-mt
      PATHS "/opt/homebrew/lib" 
      NO_DEFAULT_PATH)
    find_library(Boost_filesystem_LIBRARY 
      NAMES boost_filesystem libboost_filesystem boost_filesystem-mt libboost_filesystem-mt
      PATHS "/opt/homebrew/lib" 
      NO_DEFAULT_PATH)
    find_library(Boost_thread_LIBRARY 
      NAMES boost_thread libboost_thread boost_thread-mt libboost_thread-mt
      PATHS "/opt/homebrew/lib" 
      NO_DEFAULT_PATH)
    find_library(Boost_date_time_LIBRARY 
      NAMES boost_date_time libboost_date_time boost_date_time-mt libboost_date_time-mt
      PATHS "/opt/homebrew/lib" 
      NO_DEFAULT_PATH)
    find_library(Boost_chrono_LIBRARY 
      NAMES boost_chrono libboost_chrono boost_chrono-mt libboost_chrono-mt
      PATHS "/opt/homebrew/lib" 
      NO_DEFAULT_PATH)
    find_library(Boost_regex_LIBRARY 
      NAMES boost_regex libboost_regex boost_regex-mt libboost_regex-mt
      PATHS "/opt/homebrew/lib" 
      NO_DEFAULT_PATH)
    find_library(Boost_serialization_LIBRARY 
      NAMES boost_serialization libboost_serialization boost_serialization-mt libboost_serialization-mt
      PATHS "/opt/homebrew/lib" 
      NO_DEFAULT_PATH)
    find_library(Boost_program_options_LIBRARY 
      NAMES boost_program_options libboost_program_options boost_program_options-mt libboost_program_options-mt
      PATHS "/opt/homebrew/lib" 
      NO_DEFAULT_PATH)
    
    # Debug output for library detection
    message(STATUS "Boost_system_LIBRARY: ${Boost_system_LIBRARY}")
    message(STATUS "Boost_filesystem_LIBRARY: ${Boost_filesystem_LIBRARY}")
    message(STATUS "Boost_thread_LIBRARY: ${Boost_thread_LIBRARY}")
    
    # Check if critical libraries were found
    if(NOT Boost_system_LIBRARY)
      message(FATAL_ERROR "Boost system library not found at /opt/homebrew/lib. Try 'brew install boost'.")
    endif()
  elseif(EXISTS "/usr/local")
    set(BOOST_ROOT "/usr/local")
    set(Boost_INCLUDE_DIR "/usr/local/include")
    set(Boost_LIBRARY_DIR "/usr/local/lib")
    # Manually set library paths to avoid BoostConfig.cmake
    # Look for various library file patterns and extensions
    find_library(Boost_system_LIBRARY 
      NAMES boost_system libboost_system boost_system-mt libboost_system-mt
      PATHS "/usr/local/lib" 
      NO_DEFAULT_PATH)
    find_library(Boost_filesystem_LIBRARY 
      NAMES boost_filesystem libboost_filesystem boost_filesystem-mt libboost_filesystem-mt
      PATHS "/usr/local/lib" 
      NO_DEFAULT_PATH)
    find_library(Boost_thread_LIBRARY 
      NAMES boost_thread libboost_thread boost_thread-mt libboost_thread-mt
      PATHS "/usr/local/lib" 
      NO_DEFAULT_PATH)
    find_library(Boost_date_time_LIBRARY 
      NAMES boost_date_time libboost_date_time boost_date_time-mt libboost_date_time-mt
      PATHS "/usr/local/lib" 
      NO_DEFAULT_PATH)
    find_library(Boost_chrono_LIBRARY 
      NAMES boost_chrono libboost_chrono boost_chrono-mt libboost_chrono-mt
      PATHS "/usr/local/lib" 
      NO_DEFAULT_PATH)
    find_library(Boost_regex_LIBRARY 
      NAMES boost_regex libboost_regex boost_regex-mt libboost_regex-mt
      PATHS "/usr/local/lib" 
      NO_DEFAULT_PATH)
    find_library(Boost_serialization_LIBRARY 
      NAMES boost_serialization libboost_serialization boost_serialization-mt libboost_serialization-mt
      PATHS "/usr/local/lib" 
      NO_DEFAULT_PATH)
    find_library(Boost_program_options_LIBRARY 
      NAMES boost_program_options libboost_program_options boost_program_options-mt libboost_program_options-mt
      PATHS "/usr/local/lib" 
      NO_DEFAULT_PATH)
    
    # Check if critical libraries were found
    if(NOT Boost_system_LIBRARY)
      message(FATAL_ERROR "Boost system library not found at /usr/local/lib. Try 'brew install boost'.")
    endif()
  endif()
  
  # Debug output for library detection
  message(STATUS "Boost_system_LIBRARY: ${Boost_system_LIBRARY}")
  message(STATUS "Boost_filesystem_LIBRARY: ${Boost_filesystem_LIBRARY}")
  message(STATUS "Boost_thread_LIBRARY: ${Boost_thread_LIBRARY}")
  message(STATUS "Boost_date_time_LIBRARY: ${Boost_date_time_LIBRARY}")
  message(STATUS "Boost_chrono_LIBRARY: ${Boost_chrono_LIBRARY}")
  message(STATUS "Boost_regex_LIBRARY: ${Boost_regex_LIBRARY}")
  message(STATUS "Boost_serialization_LIBRARY: ${Boost_serialization_LIBRARY}")
  message(STATUS "Boost_program_options_LIBRARY: ${Boost_program_options_LIBRARY}")
  
  # Check if all libraries were found
  if(Boost_system_LIBRARY AND Boost_filesystem_LIBRARY AND Boost_thread_LIBRARY AND 
     Boost_date_time_LIBRARY AND Boost_chrono_LIBRARY AND Boost_regex_LIBRARY AND 
     Boost_serialization_LIBRARY AND Boost_program_options_LIBRARY)
    # Set Boost variables manually
    set(Boost_FOUND TRUE)
    set(Boost_LIBRARIES 
      ${Boost_system_LIBRARY}
      ${Boost_filesystem_LIBRARY}
      ${Boost_thread_LIBRARY}
      ${Boost_date_time_LIBRARY}
      ${Boost_chrono_LIBRARY}
      ${Boost_regex_LIBRARY}
      ${Boost_serialization_LIBRARY}
      ${Boost_program_options_LIBRARY}
    )
    set(Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIR})
    message(STATUS "Boost libraries found manually for macOS")
  else()
    # Fallback to standard FindBoost with explicit module selection
    message(STATUS "Manual Boost detection failed, falling back to FindBoost")
    # Clear any previous Boost variables to ensure clean detection
    unset(Boost_FOUND)
    unset(Boost_LIBRARIES)
    unset(Boost_INCLUDE_DIRS)
    # Force use of FindBoost module
    set(Boost_NO_BOOST_CMAKE ON)
    set(Boost_NO_CMAKE_FIND_ROOT_PATH ON)
    find_package(Boost 1.55 REQUIRED COMPONENTS system filesystem thread date_time chrono regex serialization program_options)
  endif()
else()
  # Use standard FindBoost for non-macOS systems
  find_package(Boost 1.55 REQUIRED COMPONENTS system filesystem thread date_time chrono regex serialization program_options)
endif()
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

if(MINGW)
  set(Boost_LIBRARIES "${Boost_LIBRARIES};ws2_32;mswsock")
elseif(APPLE)
  set(Boost_LIBRARIES "${Boost_LIBRARIES}")
elseif(NOT MSVC)
  set(Boost_LIBRARIES "${Boost_LIBRARIES};rt")
endif()

# option(BUILD_TESTS "Build tests." ON)

if(BUILD_TESTS)
  add_subdirectory(tests)
  enable_testing()
endif()

set(COMMIT_ID_IN_VERSION ON CACHE BOOL "Include commit ID in version")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/version")
if (NOT COMMIT_ID_IN_VERSION)
  set(VERSION "${VERSION}-unknown")
  configure_file("src/version.h.in" "version/version.h")
  add_custom_target(version ALL)
elseif(DEFINED COMMIT)
  string(REPLACE "." "\\." VERSION_RE "${VERSION}")
  if(NOT REFS MATCHES "(\\(|, )tag: v${VERSION_RE}(\\)|, )")
    set(VERSION "${VERSION}-g${COMMIT}")
  endif()
  configure_file("src/version.h.in" "version/version.h")
  add_custom_target(version ALL)
else()
  find_package(Git QUIET)
  if(Git_FOUND OR GIT_FOUND)
    message(STATUS "Found Git: ${GIT_EXECUTABLE}")
    add_custom_target(version ALL "${CMAKE_COMMAND}" "-D" "VERSION=${VERSION}" "-D" "GIT=${GIT_EXECUTABLE}" "-D" "TO=${CMAKE_BINARY_DIR}/version/version.h" "-P" "src/version.cmake" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
  else()
    message(STATUS "WARNING: Git was not found!")
    set(VERSION "${VERSION}-unknown")
    configure_file("src/version.h.in" "version/version.h")
    add_custom_target(version ALL)
  endif()
endif()

add_subdirectory(external)
add_subdirectory(src)
