cmake_minimum_required(VERSION 3.10)
project(Executive)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Target x86 architecture
set(CMAKE_GENERATOR_PLATFORM x86)

# Include Directories for External Libraries
include_directories(
    ${CMAKE_SOURCE_DIR}/executive/freeglut/include
    ${CMAKE_SOURCE_DIR}/executive/glew-2.0.0/include
    ${CMAKE_SOURCE_DIR}/executive/glm
    ${CMAKE_SOURCE_DIR}/executive/SDL2/include
    ${CMAKE_SOURCE_DIR}/executive
)

# Link Directories
link_directories(
    ${CMAKE_SOURCE_DIR}/executive/freeglut/lib
    ${CMAKE_SOURCE_DIR}/executive/glew-2.0.0/lib/Release/Win32
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

# Flex/Bison Setup
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(EXECUTIVE_BINARY_DIR ${CMAKE_BINARY_DIR}/executive)
file(MAKE_DIRECTORY ${EXECUTIVE_BINARY_DIR})

add_custom_command(
    OUTPUT ${EXECUTIVE_BINARY_DIR}/exe.tab.cpp
    COMMAND ${BISON_EXECUTABLE} -dy -o ${EXECUTIVE_BINARY_DIR}/exe.tab.cpp ${CMAKE_SOURCE_DIR}/executive/exeparser.y
    DEPENDS ${CMAKE_SOURCE_DIR}/executive/exeparser.y
)
list(APPEND SOURCE_FILES ${EXECUTIVE_BINARY_DIR}/exe.tab.cpp)

add_custom_command(
    OUTPUT ${EXECUTIVE_BINARY_DIR}/lex.exe.cpp
    COMMAND ${FLEX_EXECUTABLE} -o ${EXECUTIVE_BINARY_DIR}/lex.exe.cpp ${CMAKE_SOURCE_DIR}/executive/exeparser.l
    DEPENDS ${CMAKE_SOURCE_DIR}/executive/exeparser.l ${EXECUTIVE_BINARY_DIR}/exe.tab.cpp
)
list(APPEND SOURCE_FILES ${EXECUTIVE_BINARY_DIR}/lex.exe.cpp)

add_custom_command(
    OUTPUT ${EXECUTIVE_BINARY_DIR}/yy.tab.cpp
    COMMAND ${BISON_EXECUTABLE} -dy -o ${EXECUTIVE_BINARY_DIR}/yy.tab.cpp ${CMAKE_SOURCE_DIR}/executive/Interpreter.y
    DEPENDS ${CMAKE_SOURCE_DIR}/executive/Interpreter.y
)
list(APPEND SOURCE_FILES ${EXECUTIVE_BINARY_DIR}/yy.tab.cpp)

add_custom_command(
    OUTPUT ${EXECUTIVE_BINARY_DIR}/lex.yy.cpp
    COMMAND ${FLEX_EXECUTABLE} -o ${EXECUTIVE_BINARY_DIR}/lex.yy.cpp ${CMAKE_SOURCE_DIR}/executive/Interpreter.l
    DEPENDS ${CMAKE_SOURCE_DIR}/executive/Interpreter.l ${EXECUTIVE_BINARY_DIR}/yy.tab.cpp
)
list(APPEND SOURCE_FILES ${EXECUTIVE_BINARY_DIR}/lex.yy.cpp)

# Executable
add_executable(executive ${SOURCE_FILES})

# Link External Libraries
target_link_libraries(executive
    freeglut.lib
    glew32.lib
    SDL2main.lib
    SDL2.lib
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

# Compiler Flags
target_compile_options(executive PRIVATE
    $<$<CONFIG:Release>:/O2 /MT /GS- /W3 /Zc:wchar_t /Gm- /Zc:inline /fp:precise /DWIN32 /DNDEBUG /D_CONSOLE /Zc:forScope /Gd /Oy /FC /EHsc /nologo /diagnostics:column>
    $<$<CONFIG:Debug>:/Od /MDd /GS /ZI /RTC1 /W3 /Zc:wchar_t /Gm- /Zc:inline /fp:precise /DWIN32 /D_DEBUG /D_CONSOLE /Zc:forScope /Gd /Oy- /FC /EHsc /nologo /JMC /diagnostics:column>
)

# Linker Flags
target_link_options(executive PRIVATE
    $<$<CONFIG:Release>:/SUBSYSTEM:CONSOLE /INCREMENTAL:NO /OPT:REF /LTCG /DEBUG>
    $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE /DEBUG /INCREMENTAL>
)
