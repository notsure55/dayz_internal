cmake_minimum_required(VERSION 3.31)
project(dayz_internal)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
if(MSVC)
    foreach(flag_var
            CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_DEBUG)
        string(REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    endforeach()
endif()

include(FetchContent)

FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.0
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(minhook
        GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
        GIT_TAG        v1.3.4
)
FetchContent_MakeAvailable(minhook)

set(BUILD_SHARED_LIBS ON)

file(GLOB_RECURSE SOURCES src/*.*)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
  PREFIX ""
  OUTPUT_NAME "dayz_internal"
)

target_include_directories(${PROJECT_NAME} PRIVATE	
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  glm::glm
  minhook
)
