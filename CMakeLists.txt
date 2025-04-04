cmake_minimum_required(VERSION 3.10)
project(Executive)

# Set the C and C++ compiler paths
set(CMAKE_C_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x86/cl.exe")
set(CMAKE_CXX_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x86/cl.exe")

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Include Directories for External Libraries
include_directories(
    executive/freeglut/include
    executive/glew-2.0.0/include
    executive/glm
    executive/SDL2/include
)

# Source Files
set(SOURCE_FILES
    executive/atomic_actions.cpp
    executive/buffer.cpp
    executive/ConsoleLogger.cpp
    executive/display.cpp
    executive/executive.cpp
    executive/h_variables.cpp
    executive/interpolator.cpp
    executive/main.cpp
    executive/mesh.cpp
    executive/modal_vector.cpp
    executive/obj_loader.cpp
    executive/parson.cpp
    executive/queue.cpp
    executive/serial.cpp
    executive/shader.cpp
    executive/stb_image.cpp
    executive/texture.cpp
    executive/virtualMachine.cpp
    executive/Winsocket.cpp
)

# Bison and Flex Integration for Executive Parser
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Executive Parser - Yacc
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/executive/exe.tab.cpp
    COMMAND ${BISON_EXECUTABLE} -dy -o ${CMAKE_CURRENT_BINARY_DIR}/executive/exe.tab.cpp executive/exeparser.y
    DEPENDS executive/exeparser.y
    COMMENT "Generating exe.tab.cpp from exeparser.y"
    VERBATIM
)
list(APPEND SOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/executive/exe.tab.cpp)

# Executive Parser - Lex
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/executive/lex.exe.cpp
    COMMAND ${FLEX_EXECUTABLE} -o ${CMAKE_CURRENT_BINARY_DIR}/executive/lex.exe.cpp executive/exeparser.l
    DEPENDS executive/exeparser.l ${CMAKE_CURRENT_BINARY_DIR}/executive/exe.tab.cpp
    COMMENT "Generating lex.exe.cpp from exeparser.l"
    VERBATIM
)
list(APPEND SOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/executive/lex.exe.cpp)

# Bison and Flex Integration for NC-Interpreter parser
# NC-Interpreter parser - Yacc
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/executive/yy.tab.cpp
    COMMAND ${BISON_EXECUTABLE} -dy -o ${CMAKE_CURRENT_BINARY_DIR}/executive/yy.tab.cpp executive/Interpreter.y
    DEPENDS executive/Interpreter.y
    COMMENT "Generating yy.tab.cpp from Interpreter.y"
    VERBATIM
)
list(APPEND SOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/executive/yy.tab.cpp)

# NC-Interpreter parser - Lex
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/executive/lex.yy.cpp
    COMMAND ${FLEX_EXECUTABLE} -o ${CMAKE_CURRENT_BINARY_DIR}/executive/lex.yy.cpp executive/Interpreter.l
    DEPENDS executive/Interpreter.l ${CMAKE_CURRENT_BINARY_DIR}/executive/exe.tab.cpp
    COMMENT "Generating lex.yy.cpp from Interpreter.l"
    VERBATIM
)
list(APPEND SOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/executive/lex.yy.cpp)

# Executable Target
add_executable(executive ${SOURCE_FILES})

# Link External Libraries
target_link_libraries(executive
    freeglut
    glew32
    SDL2main
    SDL2
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