cmake_minimum_required( VERSION 3.1 )
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project( GLFW-CMake-starter )

# Specify the path to the Vulkan SDK installation
set(VULKAN_SDK_PATH "C:/VulkanSDK/1.3.261.0")



# Include Vulkan headers
include_directories(${VULKAN_SDK_PATH}/Include)
# Dear ImGui
set(IMGUI_DIR ThirdParty/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ..)

set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )

add_subdirectory( ThirdParty/glfw )
add_subdirectory(ThirdParty/imgui)

if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()

set( GLFW-CMAKE-STARTER-SRC
     main.cpp
        rh_pipeline.h
        rh_pipeline.cpp
        Dat_Engine_Device.cpp
        Dat_Engine_Device.hpp
        Source/Engine/Application/AppWindow.h
        Source/Engine/Application/AppWindow.cpp
        Source/Engine/Application/SwapChain.hpp
        Source/Engine/Application/SwapChain.cpp
        Source/Engine/Application/VulkanRender.cpp
        Source/Engine/Application/VulkanRender.h

)
     
add_executable( GLFW-CMake-starter WIN32 ${GLFW-CMAKE-STARTER-SRC} )

find_package(Vulkan REQUIRED)


# Link directory for vulkan-1
#link_directories(${VULKAN_PATH}/Bin;${VULKAN_PATH}/Lib)
target_link_libraries(GLFW-CMake-starter PRIVATE ${Vulkan_LIBRARIES})
target_link_libraries( GLFW-CMake-starter PRIVATE ${OPENGL_LIBRARIES} glfw )

set(IMGUI_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${IMGUI_DIR}/Bin/Debug/IMGUI.lib)
if(NOT EXISTS ${IMGUI_PATH})
    message(FATAL_ERROR "IMGUI_LIB: " ${IMGUI_PATH})
endif ()
message(STATUS "IMGUI_LIB: " ${IMGUI_PATH})

target_link_libraries(GLFW-CMake-starter PRIVATE ${IMGUI_PATH})

if( MSVC )
    if(${CMAKE_VERSION} VERSION_LESS "3.6.0") 
        message( "\n\t[ WARNING ]\n\n\tCMake version lower than 3.6.\n\n\t - Please update CMake and rerun; OR\n\t - Manually set 'GLFW-CMake-starter' as StartUp Project in Visual Studio.\n" )
    else()
        set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GLFW-CMake-starter )
    endif()
endif()
