# JobSystem

A naive job system written in C++11, built step by step to learn multithreading:
fixed size jobs (function pointer + `void*`), a ring buffer queue, worker threads,
atomic counters and `ParallelFor`. No virtual methods, no exceptions and no heap
allocations inside the job system.

## Requirements

| Platform | Compiler | Tools |
|---|---|---|
| macOS | Apple clang (`xcode-select --install`) | CMake 3.16+ (`brew install cmake`), optional Ninja (`brew install ninja`) |
| Windows | Visual Studio 2022 (MSVC) | CMake 3.16+ (included with Visual Studio) |
| Linux | clang or gcc | CMake 3.16+, optional Ninja |

## Project layout

```
JobSystem/
├── CMakeLists.txt     # the only source of truth for the build
├── JobSystem.hpp      # job system interface
├── JobSystem.cpp      # job system implementation  -> JobSystemLib (static library)
├── main.cpp           # demo                       -> JobSystemDemo (executable)
└── README.md
```

All commands below are run from the folder that contains `CMakeLists.txt`.
Every build goes into its own `build*/` folder (ignored by git), so the sources stay clean.

## Build (command line)

```sh
# Debug
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/JobSystemDemo

# Release (optimized, in a separate folder)
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
./build-release/JobSystemDemo
```

- The first command (*configure*) is only needed once per folder. After that, `cmake --build`
  detects changes in `CMakeLists.txt` and reconfigures by itself.
- Add `-G Ninja` to the configure command to use Ninja instead of Make (faster incremental builds).

## ThreadSanitizer / data race detection (clang or gcc)

```sh
cmake -S . -B build-tsan -DCMAKE_BUILD_TYPE=Debug -DJOBSYSTEM_ENABLE_TSAN=ON
cmake --build build-tsan
./build-tsan/JobSystemDemo
```

Look for `WARNING: ThreadSanitizer: data race` in the output. Not available on MSVC.

## Xcode (macOS)

```sh
# generate the project
cmake -S . -B build-xcode -G Xcode
open build-xcode/JobSystem.xcodeproj

# or build it from the terminal
cmake --build build-xcode --config Debug
./build-xcode/Debug/JobSystemDemo
```

- In Xcode, select the `JobSystemDemo` scheme to run it.
- `ALL_BUILD` builds everything and `ZERO_CHECK` regenerates the project when `CMakeLists.txt` changes.
- Add new files in `CMakeLists.txt`, not from Xcode: changes made in Xcode are lost on the next regeneration.

## Visual Studio (Windows)

```sh
# generate the solution
cmake -S . -B build-vs -G "Visual Studio 17 2022" -A x64
start build-vs/JobSystem.sln

# or build it from the terminal
cmake --build build-vs --config Debug
build-vs\Debug\JobSystemDemo.exe
```

- In Visual Studio, right click `JobSystemDemo` → *Set as Startup Project* before running.
- Visual Studio can also open the folder directly (*File → Open → Folder*) and use `CMakeLists.txt` without generating a solution.

## Single-config vs multi-config

| Generators | How to choose Debug/Release | Executable path |
|---|---|---|
| Make, Ninja (single-config) | `-DCMAKE_BUILD_TYPE=...` when configuring, one folder per configuration | `build/JobSystemDemo` |
| Xcode, Visual Studio (multi-config) | `--config ...` when building; `CMAKE_BUILD_TYPE` is ignored | `build-xcode/Debug/JobSystemDemo` |

## Other useful commands

```sh
# build using all CPU cores
cmake --build build --parallel

# build only one target
cmake --build build --target JobSystemLib

# show the exact compiler commands (flags, includes, -std=c++11...)
cmake --build build --verbose

# clean the compiled files but keep the configuration
cmake --build build --target clean

# start from scratch: delete the build folder
rm -rf build

# reconfigure from scratch without deleting the folder (CMake 3.24+)
cmake --fresh -S . -B build -DCMAKE_BUILD_TYPE=Debug

# list the available generators on this machine
cmake --help
```

`compile_commands.json` is generated inside `build/` (Make and Ninja only). Tools like clangd
and VS Code use it for autocompletion; point them to that folder or create a symlink:

```sh
ln -s build/compile_commands.json compile_commands.json
```

## Build without CMake (quick test)

```sh
clang++ -std=c++11 -Wall -Wextra -o app *.cpp && ./app
clang++ -std=c++11 -g -fsanitize=thread -o app_tsan *.cpp && ./app_tsan
```

## Debugging

```sh
lldb ./build/JobSystemDemo
(lldb) run
(lldb) thread list      # all threads and where they are
(lldb) bt all           # call stack of every thread (useful when something hangs)
```
