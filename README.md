<!--
********************************************************************************
* Copyright (C) 2017-2020 German Aerospace Center (DLR). 
* Eclipse ADORe, Automated Driving Open Research https://eclipse.org/adore
*
* This program and the accompanying materials are made available under the 
* terms of the Eclipse Public License 2.0 which is available at
* http://www.eclipse.org/legal/epl-2.0.
*
* SPDX-License-Identifier: EPL-2.0 
*
* Contributors: 
********************************************************************************
-->
# libadore
The core library of adore, project folder. Content can be found in subfolder [libadore](libadore).

## Build Status
[![CI](https://github.com/DLR-TS/libadore/actions/workflows/ci.yaml/badge.svg)](https://github.com/DLR-TS/libadore/actions/workflows/ci.yaml)

## Getting Started
This module requires **make** and **docker** installed and configured for your user.

### Building
To build libadore run the following:
```bash
make build
```

### Testing
To run unit tests run the following target:
```bash 
make test
```

### Static checking
There are several included static analysis tools. To run them use the following:
```bash
make lint
```

```bash
make cppcheck
```

### External Libraries
All external libraries are located in `libadore/external`. There is a provided
make file to build and publish all external libraries. By default all external
libraries are disabled in the `.gitmodules` file. They have been previously 
published to docker.io. In order to build them you must first enable the one 
you would like to build in the `.gitmodules` file. 

> **ℹ️ INFO:**
> External library submodues are disabled and will not be pulled. Enable them
> by modifying the `.gitmodules` and invoking 'git submodue update --init'.

> **ℹ️ INFO:**
> By default external libraries are not built. They are sourced as pre-compiled
> docker images from docker.io. 
