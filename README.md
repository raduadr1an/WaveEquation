# 🌊 WaveSim: Interactive C++ Wave Equation Simulator

> An interactive visualization of a Wave Equation in 1D and 2D environments. Built with C++ and SFML.

![C++](https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=c%2B%2B)
![SFML](https://img.shields.io/badge/SFML-3.0-green.svg?style=flat&logo=sfml)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)

---

## 🕹️ Controls

### General
| Key | Action |
| :--- | :--- |
| **Q** | **Quit to Main Menu** |
| **R** | Reset simulation |

### 1D Simulation
> Visualize a wave traveling along a string.
| Input | Action |
| :--- | :--- |
| **Left Click** | Create a disturbance (pluck the string) |

### 2D Simulation
> A ripple tank simulation. Light is positive pressure (Crest), Dark is negative (Trough).
| Input | Action |
| :--- | :--- |
| **Left Click** | Create a wave at mouse position |
| **Right Click** | Create permanent barriers |
| **C** | Calms the water but keeps walls |

---

## 🚀 How to Run

### Method 1: Pre-compiled Binary
1. Download `WaveSim.exe` from the [Releases](../../releases) page.
2. Run the executable.
   > **Note:** If Windows creates a popup saying "Unknown Publisher", click **More Info** -> **Run Anyway**.

### Method 2: Compile it Yourself
If you want to modify the code or build it from scratch, you will need `g++` and the **SFML 3.0** library.

**One-line build command:**
```bash
g++ -O3 WaveEquation.cpp -o WaveSim.exe -DSFML_STATIC \
-I"path/to/SFML/include" -L"path/to/SFML/lib" \
-lsfml-graphics-s -lsfml-window-s -lsfml-system-s \
-lopengl32 -lfreetype -lwinmm -lgdi32 -ldwmapi -lshlwapi -static
