# motion-search

Perform motion search and compute motion vectors and residual information in order to extract features for predicting video compressibility.

Contents:

- [Requirements](#requirements)
- [Building](#building)
- [Running](#running)
- [License](#license)

## Requirements

- [CMake](https://cmake.org/) (>= 3.1)
- `clang-format`

Under Linux, to install CMake, install the `cmake` and `clang-format` packages from your distribution's package manager.

Under macOS, you can install CMake using [Homebrew](https://brew.sh/):

```bash
brew install cmake clang-format
```

## Building

Create a build folder and run CMake, then `make`:

``` shell
mkdir build
cd build
cmake ..
make -j $(nproc)
```

The `motion-search` executable will be created in the `build/bin` folder.

## Running

The basic usage is:

``` shell
motion-search <input> -W=<width> -H=<height> <output>
```

For example:

``` shell
motion-search input.yuv -W=1920 -H=1080 stats.txt
```

The input file can be a `.yuv` or `.y4m` file.

You can use `-g` to control GOP size (default 150), `-n` to control
number of frames to read (default is to read all) and `-b` to control
number of consecutive B-frames in subgop (default 0).

See `motion-search -h` for more information.

Note that currently when using B-frames:
* code may read more than requested number of frames to complete last subgop
* if there aren't enough frames to complete last subgop then trailing
  frames after the previous complete subgop will be ignored

## License

This source code is licensed under the BSD3 license found in the
LICENSE file in the root directory of this source tree.
