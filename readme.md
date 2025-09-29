# CleanSrc for Visual Studio 2022

This project is a modernized version of **CleanSrc**, originally written by **Tomas Jakobsson**, which itself is a cleaned-up fork of the **Quake source release by id Software**. This version has been adapted to build and run using **Visual Studio 2022**, replacing the original dependency on **Visual C++ 6.0**.

## 📜 Background

- **id Software** released the Quake engine source code under the GPL.
- **CleanSrc** was developed by Tomas Jakobsson to clean and restructure the codebase for easier readability and modification.
- This project builds upon that by:
  - Generating a **Visual Studio 2022 solution and project files**
  - Maintaining functional parity with the original CleanSrc version

## ✅ Features

- Builds with **Visual Studio 2022**
- Updated solution/project files (`.sln`, `.vcxproj`)
- No gameplay changes — adheres to CleanSrc's intent of being a clean base

## 🚀 Getting Started

### Prerequisites

- [Visual Studio 2022](https://visualstudio.microsoft.com/)
- Desktop development with C++ workload installed

### Building

1. Clone the repository:
````
   git clone https://github.com/elaverick/cleansrc.git
   cd Cleansrc
````

2. Open the solution:

   * `GLQuake.sln` in Visual Studio 2022

3. Build the solution:

   * Select your desired configuration (`Debug` / `Release`)
   * Build via **Build > Build Solution** or press `Ctrl+Shift+B`

## 🗂️ Project Structure

```
/CleanSrc
├── gas2masm/ # Tools or scripts for assembly conversion
├── glquake/ # OpenGL-based Quake renderer
│ └── opengl/ # OpenGL-specific source files
├── scitech/ # Headers
├── shared/ # Shared code used across builds
│ ├── asm/ # Assembly routines
│ ├── client/ # Client-side logic
│ ├── console/ # Console and command system
│ ├── host/ # Main host system (game loop, timing, etc.)
│ ├── misc/ # Miscellaneous utilities
│ ├── network/ # Networking code
│ ├── progs/ # Scripting and entity program handling
│ ├── render/ # Shared rendering code
│ ├── server/ # Server-side logic
│ └── sound/ # Sound system
├── winquake/ # Software renderer
│ ├── asm/ # Assembly routines for software rendering
│ ├── draw/ # Software rasterizer code
│ ├── misc/ # Miscellaneous platform-specific code
│ └── render/ # Rendering logic for software path
```

## 🧾 License

This project is licensed under the terms of the **GNU General Public License (GPL)**, consistent with the original Quake source release by id Software.

Please refer to the `LICENSE.txt` file for full license details.

## 🙏 Credits

* **id Software** for the original Quake engine
* **Tomas Jakobsson** for creating CleanSrc
* Contributors from the Quake and open-source community
