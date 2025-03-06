# Thumper

Welcome to Thumper. This is an experimental project that is functioning as challenging practice and a fun way to explore a variety of coding tasks.

## Project Goals

The high level functions provided should include:
- High level 2D graphics module focused on flexibility and screen compatibility
- High level 3D graphics module focused on performance and pre-optimized asset compatibility
- Asset loading, modifying, saving, and version control for all modules that utilize assets
- Input handling for a keyboard and mouse
- Procedural generation tools for generating assets and in game generation

The top level architecture decisions include:
- Code is abstracted into functional modules with a hierarchal structure that isolates code by functional group
- Use only D2D for 2D graphics
- Use only D3D11 for 3D graphics
- To use an event driven, multithreaded architecture with looping threads able to be set to a maximum looping frequency

Some technical limitations are accepted up front:
- Compatible with Windows 10 and 11 operating systems only

## Style guide

In the interest of keeping things simple, rules are kept to a minimum.
- Compiler warnings are not allowed and must be addressed
- Do not inherit multiple layers deep, ie no X inherits from Y which inherits from Z, only X inherits from Y
- Do not use templates except as already implemented with C++ standard library functions
- Heap memory allocation must be done using standard library functions. The keywords "new", "malloc", "calloc", and "realloc" are forbidden.

Rules for comments and formatting are specified in STYLE.md.

## Architecture

The project is organized into the following modules:
- G3D, responsible for 3D graphics
- WIN, responsible for handling most windows OS functions including the window itself, mouse, keyboard, and windows headers
- More to be added as development continues

## Building

The project uses Visual Studio and requires DirectX SDK. Build configurations are available for both Debug and Release modes.
