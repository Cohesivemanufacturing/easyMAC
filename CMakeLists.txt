cmake_minimum_required(VERSION 3.10)
project(Executive)

# Set the C and C++ compiler paths to target x86 architecture
set(CMAKE_C_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.29.30133/bin/Hostx86/x86/cl.exe")
set(CMAKE_CXX_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.29.30133/bin/Hostx86/x86/cl.exe")

# Set the linker to target x86 architecture
set(CMAKE_LINKER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.29.30133/bin/Hostx86/x86/link.exe")

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Ensure CMake targets x86 architecture
set(CMAKE_GENERATOR_PLATFORM x86)

# Compiler and linker flags for x86 architecture
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:x86")

# Include Directories for External Libraries
include_directories(
    ${CMAKE_SOURCE_DIR}/executive/freeglut/include
    ${CMAKE_SOURCE_DIR}/executive/glew-2.0.0/include
    ${CMAKE_SOURCE_DIR}/executive/glm
    ${CMAKE_SOURCE_DIR}/executive/SDL2/include
    ${CMAKE_SOURCE_DIR}/executive  # Ensure the 'executive' directory is also included
)

# Link Directories for External Libraries (x86)
link_directories(
    ${CMAKE_SOURCE_DIR}/executive/freeglut/lib/x86
    ${CMAKE_SOURCE_DIR}/executive/glew-2.0.0/lib/x86
    ${CMAKE_SOURCE_DIR}/executive/SDL2/lib/x86
)

# Source Files
set(SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/executive/atomic_actions.cpp
    ${CMAKE_SOURCE_DIR}/executive/buffer.cpp
    ${CMAKE_SOURCE_DIR}/executive/ConsoleLogger.cpp
    ${CMAKE_SOURCE_DIR}/executive/display.cpp
    ${CMAKE_SOURCE_DIR}/executive/executive.cpp
    ${CMAKE_SOURCE_DIR}/executive/exeparser.cpp
    ${CMAKE_SOURCE_DIR}/executive/h_variables.cpp
    ${CMAKE_SOURCE_DIR}/executive/interpolator.cpp
    ${CMAKE_SOURCE_DIR}/executive/interpreter.cpp
    ${CMAKE_SOURCE_DIR}/executive/main.cpp
    ${CMAKE_SOURCE_DIR}/executive/mesh.cpp
    ${CMAKE_SOURCE_DIR}/executive/modal_vector.cpp
    ${CMAKE_SOURCE_DIR}/executive/obj_loader.cpp
    ${CMAKE_SOURCE_DIR}/executive/parson.cpp
    ${CMAKE_SOURCE_DIR}/executive/queue.cpp
    ${CMAKE_SOURCE_DIR}/executive/serial.cpp
    ${CMAKE_SOURCE_DIR}/executive/shader.cpp
    ${CMAKE_SOURCE_DIR}/executive/stb_image.cpp
    ${CMAKE_SOURCE_DIR}/executive/texture.cpp
    ${CMAKE_SOURCE_DIR}/executive/virtualMachine.cpp
    ${CMAKE_SOURCE_DIR}/executive/Winsocket.cpp
)

# Bison and Flex Integration for Executive Parser
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Set up build directories for generated files
set(EXECUTIVE_BINARY_DIR ${CMAKE_BINARY_DIR}/executive)

# Ensure build directories exist
file(MAKE_DIRECTORY ${EXECUTIVE_BINARY_DIR})

# Executive Parser - Yacc
add_custom_command(
    OUTPUT ${EXECUTIVE_BINARY_DIR}/exe.tab.cpp
    COMMAND ${BISON_EXECUTABLE} -dy -o ${EXECUTIVE_BINARY_DIR}/exe.tab.cpp ${CMAKE_SOURCE_DIR}/executive/exeparser.y
    DEPENDS ${CMAKE_SOURCE_DIR}/executive/exeparser.y
    COMMENT "Generating exe.tab.cpp from exeparser.y"
    VERBATIM
)
list(APPEND SOURCE_FILES ${EXECUTIVE_BINARY_DIR}/exe.tab.cpp)

# Executive Parser - Lex
add_custom_command(
    OUTPUT ${EXECUTIVE_BINARY_DIR}/lex.exe.cpp
    COMMAND ${FLEX_EXECUTABLE} -o ${EXECUTIVE_BINARY_DIR}/lex.exe.cpp ${CMAKE_SOURCE_DIR}/executive/exeparser.l
    DEPENDS ${CMAKE_SOURCE_DIR}/executive/exeparser.l ${EXECUTIVE_BINARY_DIR}/exe.tab.cpp
    COMMENT "Generating lex.exe.cpp from exeparser.l"
    VERBATIM
)
list(APPEND SOURCE_FILES ${EXECUTIVE_BINARY_DIR}/lex.exe.cpp)

# NC-Interpreter parser - Yacc
add_custom_command(
    OUTPUT ${EXECUTIVE_BINARY_DIR}/yy.tab.cpp
    COMMAND ${BISON_EXECUTABLE} -dy -o ${EXECUTIVE_BINARY_DIR}/yy.tab.cpp ${CMAKE_SOURCE_DIR}/executive/Interpreter.y
    DEPENDS ${CMAKE_SOURCE_DIR}/executive/Interpreter.y
    COMMENT "Generating yy.tab.cpp from Interpreter.y"
    VERBATIM
)
list(APPEND SOURCE_FILES ${EXECUTIVE_BINARY_DIR}/yy.tab.cpp)

# NC-Interpreter parser - Lex
add_custom_command(
    OUTPUT ${EXECUTIVE_BINARY_DIR}/lex.yy.cpp
    COMMAND ${FLEX_EXECUTABLE} -o ${EXECUTIVE_BINARY_DIR}/lex.yy.cpp ${CMAKE_SOURCE_DIR}/executive/Interpreter.l
    DEPENDS ${CMAKE_SOURCE_DIR}/executive/Interpreter.l ${EXECUTIVE_BINARY_DIR}/yy.tab.cpp
    COMMENT "Generating lex.yy.cpp from Interpreter.l"
    VERBATIM
)
list(APPEND SOURCE_FILES ${EXECUTIVE_BINARY_DIR}/lex.yy.cpp)

# Executable Target
add_executable(executive ${SOURCE_FILES})

# Link External Libraries (x86)
target_link_libraries(executive
    ${CMAKE_SOURCE_DIR}/executive/freeglut/lib/freeglut.lib
    ${CMAKE_SOURCE_DIR}/executive/glew-2.0.0/lib/Release/Win32/glew32.lib
    ${CMAKE_SOURCE_DIR}/executive/SDL2/lib/x86/SDL2main.lib
    ${CMAKE_SOURCE_DIR}/executive/SDL2/lib/x86/SDL2.lib
    OpenGL32.lib
    kernel32.lib
    user32.lib
    gdi32.lib
    winspool.lib
    comdlg32.lib
    advapi32.lib
    shell32.lib
    ole32.lib
    oleaut32.lib
    uuid.lib
    odbc32.lib
    odbccp32.lib
)

# Compiler Options for Release Build
target_compile_options(executive PRIVATE
    $<$<CONFIG:Release>:/O2>
    $<$<CONFIG:Release>:/MT>
)

# Linker Options for Release Build
target_link_options(executive PRIVATE
    $<$<CONFIG:Release>:/LTCG:incremental>
    $<$<CONFIG:Release>:/OPT:REF>
)
