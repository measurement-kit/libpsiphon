cmake_minimum_required(VERSION 3.1.0)
project(psiphon LANGUAGES CXX)

# Download header only dependencies

message(STATUS "Downloading nlohmann/json")
file(
  DOWNLOAD https://raw.githubusercontent.com/nlohmann/json/v3.3.0/single_include/nlohmann/json.hpp
  "${CMAKE_CURRENT_BINARY_DIR}/json.hpp" EXPECTED_HASH
  SHA256=f1327bb60c58757a3dd2b0c9c45d49503d571337681d950ec621f8374bcc14d4
  TLS_VERIFY ON SHOW_PROGRESS)

if (${MSVC})
  if("${CMAKE_SIZEOF_VOID_P}" EQUAL 8)
    set(LIBNDT_CURL_ARCH "amd64")
    set(LIBNDT_CURL_BASEURL "https://github.com/measurement-kit/prebuilt/releases/download")
    set(LIBNDT_CURL_CHANNEL "testing")
    set(LIBNDT_CURL_HASH "fd22453ab3ded80cbc41a8f909ceaef163b2658a07b1527f6a7ca16dc8a0912d")
    set(LIBNDT_CURL_OS "windows")
    set(LIBNDT_CURL_VERSION "7.59.0")
    message(STATUS "Downloading precompiled cURL")
    file(
      DOWNLOAD "${LIBNDT_CURL_BASEURL}/${LIBNDT_CURL_CHANNEL}/curl-${LIBNDT_CURL_VERSION}-${LIBNDT_CURL_OS}-${LIBNDT_CURL_ARCH}.tar.gz"
      "${CMAKE_CURRENT_BINARY_DIR}/curl-dep.tar.gz"
      EXPECTED_HASH SHA256=${LIBNDT_CURL_HASH}
      TLS_VERIFY ON SHOW_PROGRESS
    )
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E tar xf "curl-dep.tar.gz"
      WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
      RESULT_VARIABLE TAR_FAILURE
    )
    if("${TAR_FAILURE}")
      message(FATAL_ERROR "Cannot unpack cURL: ${TAR_FAILURE}")
    endif()
    list(APPEND CMAKE_INCLUDE_PATH "${CMAKE_BINARY_DIR}/MK_DIST/windows/amd64/include")
    list(APPEND CMAKE_LIBRARY_PATH "${CMAKE_BINARY_DIR}/MK_DIST/windows/amd64/lib")
    add_definitions(-DCURL_STATICLIB)
    list(APPEND _EXTRA_LIBS Crypt32)
  else()
    message(FATAL_ERROR "No precompiled cURL for this system")
  endif()
endif()

# Checks

find_package(CURL)

# Compiler flags

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if("${UNIX}" OR "${MINGW}")
  set(LIBNDT_FLAGS "-Wall -Wextra -Werror -fsanitize=address")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LIBNDT_FLAGS} -Wmissing-prototypes")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBNDT_FLAGS}")
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # for GCC, -Wmissing-prototypes only works for C/ObjC.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-prototypes")
  endif()
endif()

if(${WIN32})
  add_definitions(-D_WIN32_WINNT=0x0600) # for NI_NUMERICSERV
endif()

# Library

add_library(
  mkpsiphon
  STATIC
  mkpsiphon.cpp
  mkpsiphon.h
)
target_include_directories(
  mkpsiphon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/vendor
                        ${CURL_INCLUDE_DIRS}
)
if("${WIN32}" OR "${MINGW}")
  target_link_libraries(mkpsiphon ws2_32 "${_EXTRA_LIBS}")
  if ("${MINGW}")
    target_link_libraries(mkpsiphon -static-libgcc -static-libstdc++)
  endif()
endif()

add_executable(mkpsiphon-client mkpsiphon-client.cpp)
target_link_libraries(mkpsiphon-client mkpsiphon ${CURL_LIBRARIES})

# Testing

set(BUILD_TESTING ON CACHE BOOL "Whether to build tests")
enable_testing()

if(${BUILD_TESTING}) # is a standard name in CMake-land
  if(${CURL_FOUND})
    add_test(
      NAME bootstrap_test
      COMMAND mkpsiphon-client
    )
  else()
    message(WARNING "cURL not found: cannot build example clients")
  endif()
endif()
