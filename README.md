# MicroRenderer

MicroRenderer is a C++ header-only library for quick simple 3d rendering on resource-constrained embedded systems
without a dedicated graphics accelerator, specifically 32-bit microcontrollers. It is defined to be both fast and 
amenable to modification by the user. Shading computations can be programmed into a set of Shader classes, analogous 
to rendering on GPUs using shader scripts, which are inlined into code without overhead by providing them as template 
parameters to the main renderer class.

The library was developed as part of my bachelor's thesis at the Technical 
University of Vienna, details about the rendering pipeline implemented in MicroRenderer and reasoning behind it can 
be found in my thesis at: TODO.

For usage guidelines refer to the thesis, especially Section 3.7 **Library Usage** and Appendix A.
The 3d_freefly_viewer.cpp file in the demo directory may also be helpful, which can be run using SDL2 on PC.

MicroRenderer has been tested using the MSVC toolchain on PC, and on STM32 and RP2040 microcontrollers using their 
recommended toolchains.

This project is licensed under the terms of the MIT license.