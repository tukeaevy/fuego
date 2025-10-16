# Fuego TUI (Terminal User Interface) Module
# Provides ncurses-based TUI for wallet, daemon, and mining management

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(NCURSES REQUIRED ncurses)

# Include directories
include_directories(${NCURSES_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Wallet)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Daemon)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Mining)

# Common TUI sources
set(TUI_COMMON_SOURCES
    Common/TUICommon.cpp
)

# Wallet TUI sources
set(TUI_WALLET_SOURCES
    Wallet/WalletTUI.cpp
)

# Daemon TUI sources
set(TUI_DAEMON_SOURCES
    Daemon/DaemonTUI.cpp
)

# Mining TUI sources
set(TUI_MINING_SOURCES
    Mining/MiningTUI.cpp
)

# Main TUI sources
set(TUI_MAIN_SOURCES
    TUIMain.cpp
)

# All TUI sources
set(TUI_SOURCES
    ${TUI_COMMON_SOURCES}
    ${TUI_WALLET_SOURCES}
    ${TUI_DAEMON_SOURCES}
    ${TUI_MINING_SOURCES}
    ${TUI_MAIN_SOURCES}
)

# Create TUI library
add_library(fuego_tui STATIC ${TUI_SOURCES})

# Link libraries
target_link_libraries(fuego_tui
    ${NCURSES_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

# Include directories for the library
target_include_directories(fuego_tui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Common
    ${CMAKE_CURRENT_SOURCE_DIR}/Wallet
    ${CMAKE_CURRENT_SOURCE_DIR}/Daemon
    ${CMAKE_CURRENT_SOURCE_DIR}/Mining
)

# Compiler definitions
target_compile_definitions(fuego_tui PRIVATE
    ${NCURSES_CFLAGS_OTHER}
)

# Compiler flags
target_compile_options(fuego_tui PRIVATE
    ${NCURSES_CFLAGS}
)

# Create TUI executable
add_executable(fuego-tui
    TUIMain.cpp
)

# Link TUI executable
target_link_libraries(fuego-tui
    fuego_tui
    ${NCURSES_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

# Include directories for executable
target_include_directories(fuego-tui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Common
    ${CMAKE_CURRENT_SOURCE_DIR}/Wallet
    ${CMAKE_CURRENT_SOURCE_DIR}/Daemon
    ${CMAKE_CURRENT_SOURCE_DIR}/Mining
)

# Compiler definitions for executable
target_compile_definitions(fuego-tui PRIVATE
    ${NCURSES_CFLAGS_OTHER}
)

# Compiler flags for executable
target_compile_options(fuego-tui PRIVATE
    ${NCURSES_CFLAGS}
)

# Install TUI executable
install(TARGETS fuego-tui
    RUNTIME DESTINATION bin
)

# Install TUI library
install(TARGETS fuego_tui
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install TUI headers
install(DIRECTORY Common/
    DESTINATION include/fuego/tui/Common
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY Wallet/
    DESTINATION include/fuego/tui/Wallet
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY Daemon/
    DESTINATION include/fuego/tui/Daemon
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY Mining/
    DESTINATION include/fuego/tui/Mining
    FILES_MATCHING PATTERN "*.h"
)

install(FILES
    TUIMain.h
    DESTINATION include/fuego/tui
)

# Optional: Create a simple test executable
option(BUILD_TUI_TESTS "Build TUI tests" OFF)

if(BUILD_TUI_TESTS)
    add_executable(tui_test
        test/TUITest.cpp
    )
    
    target_link_libraries(tui_test
        fuego_tui
        ${NCURSES_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
    )
    
    target_include_directories(tui_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

# Print configuration summary
message(STATUS "Fuego TUI Configuration:")
message(STATUS "  NCURSES_INCLUDE_DIRS: ${NCURSES_INCLUDE_DIRS}")
message(STATUS "  NCURSES_LIBRARIES: ${NCURSES_LIBRARIES}")
message(STATUS "  NCURSES_CFLAGS: ${NCURSES_CFLAGS}")
message(STATUS "  BUILD_TUI_TESTS: ${BUILD_TUI_TESTS}")