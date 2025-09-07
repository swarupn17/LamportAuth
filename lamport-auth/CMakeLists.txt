cmake_minimum_required(VERSION 3.16)
project(lamport_auth_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automatic moc/uic handling
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt (Widgets + Core + Network)
find_package(Qt6 COMPONENTS Widgets Core Network REQUIRED)

# Find Crypto++ (we'll try Homebrew prefix as hint; user can override CRYPTOPP_ROOT)
if(NOT DEFINED CRYPTOPP_ROOT)
  execute_process(COMMAND brew --prefix OUTPUT_VARIABLE BREW_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(CRYPTOPP_ROOT ${BREW_PREFIX} CACHE PATH "Crypto++ root prefix")
endif()
find_path(CRYPTOPP_INCLUDE_DIRS cryptopp/sha.h HINTS ${CRYPTOPP_ROOT}/include /usr/local/include /opt/homebrew/include)
find_library(CRYPTOPP_LIBRARIES NAMES cryptopp libcryptopp HINTS ${CRYPTOPP_ROOT}/lib /usr/local/lib /opt/homebrew/lib)

if(NOT CRYPTOPP_INCLUDE_DIRS OR NOT CRYPTOPP_LIBRARIES)
  message(WARNING "Crypto++ not found by CMake. You can still build if you link manually or provide CRYPTOPP_ROOT.")
endif()

# include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
if(CRYPTOPP_INCLUDE_DIRS)
  include_directories(${CRYPTOPP_INCLUDE_DIRS})
endif()

set(SOURCES
    src/main.cpp
    src/gui/MainWindow.cpp
    src/network/NetworkManager.cpp
    src/app/Controller.cpp
    src/crypto/LamportChain.cpp
    src/util/ConfigManager.cpp
)

# UI file (ensure this matches actual file in src/gui/)
set(UI_FILES
    src/gui/mainwindow.ui
)

# Add executable
add_executable(lamport_app
    ${SOURCES}
    ${UI_FILES}
)

# Link Qt libraries
target_link_libraries(lamport_app PRIVATE Qt6::Widgets Qt6::Core Qt6::Network)

# If Crypto++ was found, link it
if(CRYPTOPP_LIBRARIES)
  target_link_libraries(lamport_app PRIVATE ${CRYPTOPP_LIBRARIES})
else()
  # you can link a static lib manually, for example:
  # target_link_libraries(lamport_app PRIVATE /opt/homebrew/lib/libcryptopp.a)
endif()

# Install rule (optional)
install(TARGETS lamport_app DESTINATION bin)

# ---- config_test target ----
add_executable(config_test
  tests/config_test.cpp
)
target_include_directories(config_test PRIVATE ${SRC_DIR})
target_link_libraries(config_test PRIVATE Qt6::Core)
set_target_properties(config_test PROPERTIES
  FOLDER "tests"
)
