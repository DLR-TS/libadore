# libadore external libraries
This project contains all external libraries consumed by libadore not available
in central repositories

This project contains submodules and a Dockerfile necessary to build and package
all external libadore libraries.

### External Libraries
All external libraries are located in `libadore/external`. There is a provided
make file to build and publish all external libraries. By default all external
libraries are disabled in the `.gitmodules` file. They have been previously 
published to docker.io. In order to build them you must first enable the one 
you would like to build in the `.gitmodules` file. 

> **ℹ️ INFO:**
> External library submodues are disabled and will not be pulled. Enable them
> by modifying the `.gitmodules` and invoking 'git submodue update --init --recursive'.
> `make build_<some lib>` will fail without cloning submodules.

> **ℹ️ INFO:**
> By default external libraries are not built. They are sourced as pre-compiled
> docker images from docker.io. 

## Getting Started
Docker and Make are required

### build
to build all external libraries run make:
```bash
make
```
you can build specific libraries with the provided make targets for example:
```bash
make build_xodr
```

### Build Artifacts
Once a particular library is built artifacts can be found at:
- <library>/build
- <library>/build/<library name>.deb
- <library>/build/install - cmake install context


## Tags
Catch2:v2.10.2
dlib:v19.24
csaps-cpp:master
osqp:v0.6.2
qpOASES:268b2f2
xodr:a751ae6
osqp:latest 
