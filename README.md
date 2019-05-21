# HOMR
Hall Of Mirrors Reverberator

Copyright (c) 2019 Dale Johnson, GPL-3 License

This software was written as part of PhD study that took place at the University of Huddersfield

## Prerequisites
HOMR depends on **libsndfile** and **vectorclass**.

libsndfile can be downloaded and installed using brew with the following command:
```
brew install libsndfile
```
vectorclass is a library of SIMD wrapper classes written by Agner Fog, and is available from [here](https://www.agner.org/optimize/#vectorclass).

## Compiling

HOMR can be built using the included Xcode project file. It has been setup to look for libsndfile in your include directory (which should be installed correctly using brew), and vectorclass in the ./HOMR folder.

## Usage

HOMR expects several options including two input files in order to render an impulse response. The two files required are a:

* **Model**, or *.smd* file that describes the dimensions and positions of each surface in the virtual space.
* **Configuration**, or *.cfg* file that informs HOMR of the positions and directions of the sources and receivers, along with information such as their directivity and polar patterns.

HOMR can be executed from a command-line using the following example command:
```
./HOMR -m concertHall.smd -c xy_6m.cfg -i 3 -r 50000 -s 44100 -n concertHallXY -l 0
```
