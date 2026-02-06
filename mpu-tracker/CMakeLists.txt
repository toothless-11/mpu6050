cmake_minimum_required(VERSION 3.10)
project(MPU6050_3D_Native)

set(CMAKE_CXX_STANDARD 17)

# 寻找 OpenGL
find_package(OpenGL REQUIRED)

add_executable(3d_display main.cpp)

# 如果是 MinGW 环境，我们需要显式链接这些系统库
if(MINGW)
    target_link_libraries(3d_display PRIVATE 
        OpenGL::GL 
        OpenGL::GLU 
        -lgdi32 
        -luser32 
        -lkernel32
    )
else()
    target_link_libraries(3d_display PRIVATE OpenGL::GL OpenGL::GLU gdi32 user32)
endif()