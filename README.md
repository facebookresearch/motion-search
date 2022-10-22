# motion-search

Perform motion search and compute motion vectors and residual information in order to extract features for predicting video compressibility.

## Building

This project uses CMake to build and depends on `clang-format` for
auto formatting. On OS X you can install `clang-format` using
`brew install clang-format`.


``` shell
mkdir build
cd build
cmake ..
make
```

## Running

``` shell
bin/motion-search input.yuv -W=<width> -H=<height> stats.txt
```

You can use `-g` to control GOP size (default 150), `-n` to control
number of frames to read (default is to read all) and `-b` to control
number of consecutive B-frames in subgop (default 0)

Note that currently when using B-frames:
* code may read more than requested number of frames to complete last subgop
* if there aren't enough frames to complete last subgop then trailing
  frames after the previous complete subgop will be ignored

## License

This source code is licensed under the BSD3 license found in the
LICENSE file in the root directory of this source tree.
