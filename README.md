# Spacenav IGT #

Use your 3DConnexion SpaceMouse with 3D Slicer on Linux.
This project leverages libspacenav (C) and OpenIGTLink (C++) to make 3DConnexion devices
usable with OpenIGTLink on Linux systems.

Tested with:
* [x] SpaceMouse Wireless
* [ ] SpaceMouse Compact
* [ ] SpaceMouse Pro
* [ ] SpaceMouse Pro Wireless
* [ ] SpaceMouse Enterprise

![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/henrykrumb/spacenav-igt/cmake.yml)

## Installation and Usage ##

Run

```bash
mkdir build
cd build
cmake ..
make
```

and you'll find a spnavigt binary inside this folder.
The CMakeLists.txt is written as such that dependencies (libspnav and OpenIGTLink) are installed automatically.
Once you run the binary, you can use SlicerIGT (module SlicerIGTIF) to connect and receive a transform called "SpaceMouse".

To install system-wide, run

```bash
sudo make install
```

## Buttons ##

The two buttons of the SpaceMouse are currently mapped as follows:
* Button 0 (left button): Reset translation
* Button 1 (right button): Reset rotation


## Usage in 3D Slicer ##

3D Slicer needs the OpenIGTLink extension to make this work.

1. Open 3D Slicer
2. Start spnav-igt
3. In Slicer, go to "OpenIGTLinkIF" extension and create a new connection.
4. Set the port to the port of spnav-igt (default: 18944).
5. Hit the "Active" checkbox.
6. Go to "Data" extension, "Transform hierarchy" tab.
7. Drag "Camera" (or the camera you want to manipulate) under "SpaceMouse".

## TODOs ##

* [x] CMake build with FetchContent (auto-install OpenIGTLink)
* [x] Rotations (so far, only translations are supported)
* [ ] Flexible configuration via JSON
* [x] System-wide installation
* [x] Use button events
* [x] Command line arguments