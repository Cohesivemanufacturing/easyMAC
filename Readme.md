# **Installing easyMAC**  
*Easy and Modular Architecture Controller – University of Illinois at Urbana-Champaign*

The source code for **easyMAC** has been developed on Windows for a 32-bit architecture (x86) and can be set up for development using either a Visual Studio solution file or a CMake file. Both methods require the installation of Visual Studio 2019 along with the C++ development tools.

## **Required Software**

1. Visual Studio 2019 (newer versions may require migration)
2. C++ Development Tools for Visual Studio
3. WinFlex and WinBison (Windows versions of Flex and Bison)
4. CMake Tools
5. Git (for version control)

---

## **Building the Source with the Visual Studio Solution File**

easyMAC uses Flex and Bison (`.l`, `.y`) to define interpreters for both the console commands and G-code. The solution file includes custom build rules for these interpreters.

To view or modify these rules:

1. Open the solution file in Visual Studio.
2. Navigate to **Solution Explorer** → `exeparser` → expand **lex** and **yacc**.
3. Right-click on `exeparser.y` or `exeparser.l`.

The custom build settings can be found under:  
**Properties → Custom Build Tool**

> **Important**:  
> To compile these files correctly, ensure the system environment variables for `win_flex` and `win_bison` are properly configured.  
> Also, make sure the target architecture is set to **Win32 (x86)**.  
> Then, build the project via **Build → Build Solution** in the top menu.

---

## **Building the Source with CMake and NMake**

Alternatively, you can build easyMAC using CMake and NMake. Follow the steps below to set up the project using VS Code:

### **Step 1: Prepare the Project Directory**

1. Inside the root directory of the project (where the `.sln` file is located), create a folder named `out` to hold the build output files.

### **Step 2: Set Up the Developer Command Prompt**

1. Open a **Visual Studio Developer Command Prompt for VS2019** in the `/out` folder.  
   ⚠️ **Important**: Make sure to use the **Developer Command Prompt for VS2019** (not the **x64 Cross Tools Command Prompt**), as that targets the x64 architecture.

### **Step 3: Configure the Build Using CMake**

In the **Developer Command Prompt for VS2019**, run the following commands to configure the build:

```bash
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
nmake
```
Alternatively, for a release build:
```bash
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release ..
nmake
```

### **Step 4: Copy Required DLLs**
Once the executable has been generated, copy the following files and folders into the same directory as the executable:

>glew32.dll
>SDL2.dll
>The res folder from the source directory

This ensures that all necessary dependencies and resources are available when running the program.

## **VS Code Configuration for Debugging and Building**

### **1. Install VS Code Extensions**
Install the following extensions in VS Code to enhance the C++ development experience:

- **C/C++** (by Microsoft)
- **CMake Tools**
- **CMake** (optional)

---

### **2. Configure `tasks.json` for Build Tasks**
Create or update the `.vscode/tasks.json` file in your project directory with the following content:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build with NMake (Debug)",
            "type": "shell",
            "command": "cmd.exe",
            "args": [
                "/c",
                "\"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\Common7\\Tools\\VsDevCmd.bat\" && cmake -G \"NMake Makefiles\" -DCMAKE_BUILD_TYPE=Debug ../.. && nmake"
            ],
            "options": {
                "cwd": "${workspaceFolder}/build/Debug",
                "shell": {
                    "executable": "cmd.exe",
                    "args": ["/d", "/c"]
                }
            },
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": []
        },
        {
            "label": "Build with NMake (Release)",
            "type": "shell",
            "command": "cmd.exe",
            "args": [
                "/c",
                "\"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\Common7\\Tools\\VsDevCmd.bat\" && cmake -G \"NMake Makefiles\" -DCMAKE_BUILD_TYPE=Release ../.. && nmake"
            ],
            "options": {
                "cwd": "${workspaceFolder}/build/Release",
                "shell": {
                    "executable": "cmd.exe",
                    "args": ["/d", "/c"]
                }
            },
            "group": {
                "kind": "build",
                "isDefault": false
            },
            "problemMatcher": []
        }
    ]
}
```
### **3. Configure `launch.json` for Debugging**

Create or update the `.vscode/launch.json` file with the following content to enable debugging:

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug (NMake) - Debug",
            "type": "cppvsdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/Debug/your_executable.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}/build/Debug",
            "environment": [],
            "externalConsole": false,
            "preLaunchTask": "Build with NMake (Debug)"
        },
        {
            "name": "Release (NMake) - Release",
            "type": "cppvsdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/Release/your_executable.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}/build/Release",
            "environment": [],
            "externalConsole": false,
            "preLaunchTask": "Build with NMake (Release)"
        }
    ]
}
```

## **Running and Debugging in VS Code**

1. **Build the Project**  
   To build your project, press `Ctrl + Shift + B` and select either "Build with NMake (Debug)" or "Build with NMake (Release)".

2. **Debug the Project**  
   To start debugging, press `F5` or go to `Run > Start Debugging` and choose the corresponding build configuration (Debug or Release).

3. **Monitor Output and Errors**  
   Monitor the output in the Terminal panel to see build and runtime messages. Any build or runtime errors will appear here.

---

### **Troubleshooting**

- **File Not Found Errors**: Ensure that the working directory (`cwd`) in the `launch.json` and `tasks.json` files is correctly set to the folder containing the executable.

- **Missing DLLs**: Make sure the `glew32.dll`, `SDL2.dll`, and `res` folder are in the same directory as the executable.

- **Debugging Assertion Errors**: Check if any files are missing or not being properly opened (e.g., configuration files) and ensure your file paths are correct.

---

### **Additional Resources**

- [CMake Documentation](https://cmake.org/documentation/): Official CMake Documentation

- [Visual Studio Documentation](https://docs.microsoft.com/en-us/visualstudio/): Official Visual Studio Documentation

- [VS Code Debugging Docs](https://code.visualstudio.com/docs/editor/debugging): Official VS Code Debugging Documentation