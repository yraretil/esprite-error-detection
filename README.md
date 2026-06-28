# error correction using spectral bins (bch), implemented on esp32

audio samples from an inmp441 mic are encoded into a 6-point codeword using a systematic generator matrix. a single-error-correcting bch decoder locates and erases impulse noise by computing the dft syndrome, extracting the error phase, and resolving the half-period alias against the corrupted array. the corrected message samples sit in the first four indices of the codeword, so decoding is a free read.

the esp32 streams raw and corrected samples over serial. a python websocket bridge forwards them to a browser-based plotter that draws mic-raw, errored, and output waveforms in real time and reports whether bch correction is active.

## structure

    src/bch_sim.cpp   - standalone c++ simulation of the encode → corrupt → detect → correct pipeline
    src/bch.py        - computes the pseudoinverse decode matrix from the generator matrix
    src/ard.py        - records i2s audio from the esp32 over serial and saves a wav file
    src/project.py    - websocket bridge (serial → ws://localhost:8765)
    src/plotter.html  - real-time waveform viewer (connects to the websocket bridge)

## setup

    pip install pyserial websockets numpy

## commands

record audio to wav:

    python src/ard.py /dev/ttyUSB0

run the live plotter:

    python src/project.py
    # then open src/plotter.html in a browser

run the bch simulation:

    g++ -o bch_sim src/bch_sim.cpp -lm && ./bch_sim

generate the decode matrix:

    python src/bch.py
