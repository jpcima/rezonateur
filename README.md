# Rézonateur
Virtual-analog 3-band resonator

This resonator consists of 3 multi-mode resonant filters which cover independent bands of the audio spectrum.
- Low: 100 Hz - 300 Hz
- Mid: 300 Hz - 1.5 kHz
- High: 1.5 kHz - 7.5 kHz

## Build instructions

1. Obtain prerequisites

Install needed packages:

- `git`
- `build-essential`
- `pkg-config`
- `libx11-dev`
- `libcairo2-dev`
- `libjack-jackd2-dev` or `libjack-dev`

2. Check out the repository and submodules

```
git clone https://github.com/jpcima/rezonateur.git
cd rezonateur
git submodule update --init
```

3. Patch DPF

This recommended patch works around a current bug in LV2 graphical interfaces.

```
patch -d dpf -p 1 -i "`pwd`/resources/patch/DPF-bypass.patch"
```

4. Compile

```
make
```

5. Install

The VST, LV2, and JACK are available in the `bin` folder.
Copy these plugins to their appropriate system locations.
