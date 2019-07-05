# HOMR
**H**all **O**f **M**irrors **R**everberator

This software was written as part of a PhD study that took place at the University of Huddersfield, under the supervision of Hyunkook Lee.

Applied Psychoacoustics Lab, University of Huddersfield, https://www.hud.ac.uk/apl/

Emails: d.s.johsnon2@hud.ac.uk, h.lee@hud.ac.uk


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

Example model and configuration files are provided in *examples* folder.

HOMR can be executed from a command-line using the following example command:
```
./HOMR -m largeroom.smd -c xy_6m.cfg -i 3 -r 50000 -s 44100 -n largeRoomXY -l 0
```
After completion, two files are produced:
* **Raw Impulse Vector (RIV)**: A CSV file that contains all of the sound rays captured by the receiver, along with meta-data about each ray (see the section below).

* **Impulse Response (IR)**: A .WAV file of the model's impulse response synthesised internally from the RIV. This can be used by a convolution reverb plugin in your favourite DAW software.

## Raw Impulse Vector (RIV)

The RIV files produced. HOMR is a CSV file that contains all of the sound rays captured by a particular receiver, along with meta-data for each ray. The number of receivers determines the number of RIV files produced. The ray and its meta-data is stored in a single row, with each column representing each type of data in the following order:

* (1-3) Direction of arrival in vector form.
* (4-11) The octave band energy for all 8 octave bands from 62.5 Hz to 8 kHz (note that when synthesising the WAV file, in order to cover the 31.2 Hz and 16 kHz bands, the energy from the first and last bands are duplicated and filtered by extra band-pass filters).
* (12) Distance travelled in metres.
* (13) Reflection order, where 0 is the direct sound ray.

An example row for a given ray would look like the following.

| x | y | z | 62.5| 125 | 250 | 500  | 1k  | 2k  |  4k | 8k | Dist (m) | Order |
|---|---|---|-----|-----|-----|------|-----|-----|-----|----|----------|-------|
| 0 | 0 | 1 | 0.8 | 0.77| 0.6 | 0.58 | 0.4 | 0.3 | 0.1 | 0.1| 6        | 0     |


## License

This software is licensed using GNU Public License version 3. See LICENSE.md for further details.
