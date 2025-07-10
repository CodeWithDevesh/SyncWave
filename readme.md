# SyncWave

Tired of sharing a single earphone during movie night? Wanna jam out with your friend — on separate headphones?

This command-line tool lets you play audio through multiple (two currently😅) playback devices at the same time — yes, including Bluetooth speakers and headphones.
Perfect for watching movies, listening to music, or just vibing across devices.

# Installation
You can download the latest release from the [releases](https://github.com/CodeWithDevesh/SyncWave/releases).

# Usage
Open the terminal and navigate to the folder where you extracted the files.

Run with -h flag for help

```sh
cd "path to your folder"
./syncwave.exe -h

OPTIONS:

      -h, --help                        Display this help menu
      -l, --list-devices                List available devices
      -d[device], --device=[device]     Output device number
```

You can use the --list-devices command to list the available devices for playback on your system.

```sh
./SyncWave.exe --list-devices

Available Devices
   0: Headphones (Sushi)  -> (default)
   1: TS35505 (HD Audio Driver for Display Audio)
   2: Speakers (Realtek(R) Audio)
```

Then with the -d command you can specify the output device you want to loopback your audio from.

It takes in the device number listed with the --list-devices command

```sh
> ./SyncWave.exe -d 1
```

# Building
The third-party libraries used are-
- [spdlog](https://github.com/KjellKod/g3log) (The Logger).
- [miniaudio](https://miniaud.io/) (The Powerhouse).
- [args](https://github.com/Taywee/args) (The Argument Parser).

All these libraries are header only and are already included in the source so you don't need to setup any of them.

Build with cmake... 

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

For release version use

```sh
cmake --build . --config Release
```

Again... build on windows only...