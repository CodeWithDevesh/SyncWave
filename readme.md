# SyncWave
This is a simple "windows only" command line tool written in c++ for using multiple playback devices at the save time.

# Installation
You can download the latest release from the [releases](https://github.com/CodeWithDevesh/SyncWave/releases).

# Usage
Open the terminal and navigate to the folder where you extracted the files.

Run with -h flag for help

```sh
> cd "path to your folder"
> ./syncwave.exe -h

OPTIONS:

      -h, --help                        Display this help menu
      -l, --list-devices                List available devices
      -d[device], --device=[device]     Output device number
```

You can use the --list-devices command to list the available devices for playback on your system.

```sh
> ./SyncWave.exe --list-devices
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
The project is built using cmake and MSVC only. 

The third-party libraries used are-
- [g3log](https://github.com/KjellKod/g3log)
- [miniaudio](https://miniaud.io/)
- [args](https://github.com/Taywee/args)

All these libraries are already included in the source so you don't need to setup any of them. I have modified the g3log a little bit so other versions may not work.

Build with cmake... 

```sh
> mkdir build
> cd build
> cmake ..
> cmake --build .
```

For release version use

```sh
> cmake --build . --config Release
```

I would prefer you to use only visual studio generator with it as I have only tested it with that and it is also windows only