# Minecraft clone for 3DS

This is a simple Minecraft clone for the 3DS. It is written in C++ using the libctru and citro3d libraries. It features face culling, physics, and block breaking and placing. It doesn't include some of the more advanced features like lighting, translucency and greedy meshing.

## Usage

A .3dsx build is available in releases. I haven't been able to build for a real 3DS, since that requires [bannertool](https://github.com/Steveice10/bannertool/), but the repository doesn't seem to exist anymore. To build the project yourself, follow the instructions below.

## Building

Invoke `make` to build the project. The following targets are available:

| Targets     | Action                                                                                    |
| ------------| ----------------------------------------------------------------------------------------- |
| 3dsx        | Builds `<project name>.3dsx` and `<project name>.smdh`.
| elf         | Builds `<project name>.elf`.

## Setting up devkitPro
* Follow the steps installing devkitPro at the gbatemp [wiki](https://wiki.gbatemp.net/wiki/3DS_Homebrew_Development#Install_devkitARM)

## Credits
This project is based on the awesome [3DS homebrew template](https://github.com/TricksterGuy/3ds-template).
