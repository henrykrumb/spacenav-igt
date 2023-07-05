# Spacenav IGT #

Use your 3DConnexion SpaceMouse with 3D Slicer on Linux.
This project leverages libspacenav (C) and OpenIGTLink (C++) to make 3DConnexion devices
usable with OpenIGTLink on Linux systems.
So far, this is an incomplete proof-of-concept, and it works on my machine (Linux Mint + SpaceMouse wireless).


## Installation and Usage ##

First, you'll need to install OpenIGTLink and libspacenav system-wide.
Once that's done, do:

```bash
make
```

and you'll find a spnavigt binary inside this folder.
Once you run it, you can use SlicerIGT (module SlicerIGTIF) to connect and receive a
transform called "SpaceMouse".


## TODOs ##

* [ ] CMake build with FetchContent (auto-install OpenIGTLink)
* [ ] Rotations (so far, only translations are supported)
* [ ] Flexible configuration via JSON
* [ ] System-wide installation
* [ ] Use button events somehow