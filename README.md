# JMPXRDS [![Travis (.org)](https://img.shields.io/travis/uoc-radio/jmpxrds)](https://travis-ci.org/github/UoC-Radio/JMPXRDS/) [![Sonar Quality Gate](https://img.shields.io/sonar/quality_gate/UoC-Radio_JMPXRDS?server=https%3A%2F%2Fsonarcloud.io)](https://sonarcloud.io/dashboard?id=UoC-Radio_JMPXRDS) [![Sonar Coverage](https://img.shields.io/sonar/coverage/UoC-Radio_JMPXRDS?server=https%3A%2F%2Fsonarcloud.io)](https://sonarcloud.io/component_measures?id=UoC-Radio_JMPXRDS&metric=coverage)
An FM MPX signal generator on top of Jack Audio Connection Kit with support for:
* Typical DSB modulator
* Multiple SSB modulators for the stereo subchannel (L-R), plus a mono mode
* FM pre-emphasis (50us / 75us) and an audio low-pass filter to protect the pilot
* RDS Encoder with support for all basic fields (PI, PS, PTY, PTYN, RT, TP, TA, MS, DI, ECC, LIC, CT) and dynamic PS / RT from a file
* Built-in RTP server for sending the generated signal on a remote site

The generator output has a fixed sampling rate of 192000, it outputs data to a local named pipe (`/run/user/<uid>/jmpxrds.sock`, raw float32 mono, for the GUI to do FFT analysis and/or other uses e.g. netcat/sox/GNU Radio), and through a built-in RTP server. Note that RTP server sends the signal FLAC-encoded to reduce bandwidth. A client for the RTP server is also available as well as a GNU Radio - based receiver for debugging. The signal can be used to drive an FM exciter via a normal sound card. We 've tried this with RPi + HiFi Berry and Odroid + HiFi Shield. JMPXRDS produces the baseband MPX signal only; to transmit it with an SDR such as a HackRF it has to be FM-modulated first (e.g. with GNU Radio and gr-osmosdr, see the documentation).

The optional GUI is based on GTK3+ and can be used to fully control the generator, alternatively a set of command line tools are also available for headless systems. The generator itself is a standalone app and uses shared memory to communicate with the GUI/tools.

It's currently used in production on 3 radio stations in Crete (UoC Radio 96.7, MatzoRe 89.1, Best 94.7). In order to operate this properly you should also use some audio processing, we recommend [Calf Studio Gear](https://calf-studio-gear.org/). The default settings together with a properly configured compressor/limiter will get you within the allowed deviation constraints (75KHz). You should also make sure your exciter and amplifier are properly set up.


An overview of JMPXRDS is included in a presentation at [FOSDEM 2018](https://youtu.be/H6Ki-RbeSHI?t=603).

## Building

Dependencies: JACK, libsoxr, FFTW3 (float), and optionally GStreamer 1.0 (RTP server), GTK+ >= 3.16 with libepoxy (GUI) and libsystemd (service notification). On Debian/Ubuntu:

```sh
sudo apt install build-essential autoconf automake pkg-config \
    libjack-jackd2-dev jackd2 libsoxr-dev libfftw3-dev \
    libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev gstreamer1.0-plugins-good \
    libgtk-3-dev libepoxy-dev libsystemd-dev
```

```sh
autoreconf -ivf
./configure        # --without-gtk, --without-rtp-server, --enable-debug
make
sudo make install
```

This installs `jmpxrds`, `fmmod_tool`, `rds_tool` and, depending on the configuration, `rtp_tool` and `jmpxrds_gui`. The RTP client in `rtp_client/` is built separately with its own Makefile.

## Quick start

```sh
jackd -R -d alsa -r 48000 -p 1024 &      # any sample rate works
jmpxrds &
jack_connect <source>:out_L FMmod:AudioL
jack_connect <source>:out_R FMmod:AudioR
rds_tool -e -pi 0x1234 -ps "MYRADIO" -rt "Hello world"   # RDS is disabled by default
fmmod_tool -g                                             # show levels and settings
jmpxrds_gui                                               # optional
```

## Documentation

Detailed documentation (in German) covering the architecture, all command line options, the RTP setup, the RDS encoder, HackRF/osmo-sdr output and known issues is available in [docs/DOKUMENTATION.md](docs/DOKUMENTATION.md).
