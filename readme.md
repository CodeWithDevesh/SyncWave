# SyncWave
This is a simple "windows only" command line tool written in c++ for using multiple playback devices at the save time.

# Installation
You can download zip files from the release section. Unzip the contents at a suitable location.

You can also add the folder to your path for global access.

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
A CMakeLists.txt file is included. You will need gflags and g3log libraries to be already built and installed on your system.
Then build with cmake...

```sh
> cmake -S . -B ./out
> cd ./out
> cmake --build
```