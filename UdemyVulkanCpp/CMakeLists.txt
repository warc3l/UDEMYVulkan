cmake_minimum_required(VERSION 3.26)
project(UdemyVulkanCpp)

set(CMAKE_CXX_STANDARD 23)

project(UdemyVulkanCpp
        LANGUAGES CXX
)

set(CMAKE_OSX_ARCHITECTURES "arm64")

include_directories("externals/GLFW/include")
include_directories("externals/glm-0.9.9.8")

link_directories("externals/GLFW/lib-universal")

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)


add_executable(UdemyVulkanCpp main.cpp)

target_sources(UdemyVulkanCpp PRIVATE main.cpp)
target_link_libraries(UdemyVulkanCpp PRIVATE Vulkan::Vulkan)
target_link_libraries(UdemyVulkanCpp PRIVATE GLFW)

