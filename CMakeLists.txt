cmake_minimum_required (VERSION 3.15)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project ("OpenGl-CMake")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
endif()



# 1. TEMEL AYARLAR
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/external")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) 

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

include(FetchContent)

# -------------------------------------------------------------------
# 2. KÜTÜPHANELER (FETCHCONTENT)
# -------------------------------------------------------------------

# --- GLAD (OpenGL Yükleyici) ---
# GLAD'ı CMake ile uyumlu olan bu repodan çekiyoruz.
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG v0.1.36 # En stabil sürüm
)
# GLAD Ayarları: Sadece GL sürümünü belirtiyoruz.
set(GLAD_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glad)

# --- GLFW ---
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# --- GLM ---
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)

# --- ASSIMP ---
FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG v5.3.1
)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(assimp)

# --- STB (Image Loading) ---
FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

# STB için bir "interface" kütüphanesi oluşturalım (çünkü kendisi hazır bir CMake projesi değil)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
# -------------------------------------------------------------------
# 3. SENİN DOSYALARIN
# -------------------------------------------------------------------

# Buraya sadece kendi yazdığın dosyaları ekle.
# glad.c artık otomatik derlendiği için buraya eklemene gerek KALMADI.
add_executable(${PROJECT_NAME} 
    "OpenGl-CMake.cpp" 
    "OpenGl-CMake.h"
    "model_loading_test.cpp"
 "stb_setup.cpp")

# -------------------------------------------------------------------
# 4. BAĞLANTILAR
# -------------------------------------------------------------------

# glad::glad ekleyerek tüm OpenGL fonksiyonlarını projeye dahil ediyoruz.
target_link_libraries(${PROJECT_NAME} PRIVATE 
    glad
    glfw 
    glm::glm 
    assimp::assimp
    stb
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${stb_SOURCE_DIR}
    "Headers"      # headers klasörünün içindeki dosyaları direkt bulmasını sağlar
    .
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")