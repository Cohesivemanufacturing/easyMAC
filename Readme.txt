# Installing easyMAC  
*Easy and Modular Architecture Controller – University of Illinois at Urbana-Champaign*

The source code for **easyMAC** has been developed on Windows for a 32-bit architecture (x86) and can be set up for development using either a Visual Studio solution file or a CMake file. Both methods require the installation of Visual Studio 2019 along with the C++ development tools.

## Required Software

1. Visual Studio 2019 (newer versions require migration)
2. C++ Development Tools for Visual Studio
3. WinFlex and WinBison (Windows versions of Flex and Bison)
4. CMake Tools

---

## Building the Source with the Visual Studio Solution File

easyMAC uses Flex and Bison (`.l`, `.y`) to define interpreters for both the console commands and G-code. The solution file includes custom build rules for these interpreters.

To view or modify these rules:

- Open the solution file
- Navigate to **Solution Explorer** → `exeparser` → expand **lex** and **yacc**
- Right-click on `exeparser.y` or `exeparser.l`

The custom build settings can be found under:  
**Properties → Custom Build Tool**

> **Important**:  
> To compile these files correctly, ensure the system environment variables for `win_flex` and `win_bison` are properly configured.  
> Also, make sure the target architecture is set to **Win32 (x86)**.  
> Then, build the project via **Build → Build Solution** in the top menu.

---

## Building the Source with CMake and NMake

Alternatively, you can build easyMAC using CMake and NMake:

1. Inside the root directory of the project (where the `.sln` file is located), create a folder named `out`.

2. Open a **Visual Studio Developer Command Prompt for VS2019** in the `/out` folder.  
   > ⚠️ Make sure it's the **Developer Command Prompt for VS2019** and not the **x64 Cross Tools Command Prompt**, as that targets the x64 architecture.

3. Run the following commands:

```bash
cmake -G "NMake Makefiles" ..
nmake

4. Once the executable has been generated in the out folder, copy the following files and folders into the same directory as the executable:

glew32.dll
SDL2.dll
The res folder inside the source folder excecutable