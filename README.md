# FrancisFrancis X1

## Building the binary

Create a build folder, run cmake with your wifi info and build the project
```
$ mkdir firmware/build && cd firmware/build
$ cmake .. -DWIFI_SSID="network_name" -DWIFI_PASSWORD="password"
$ make -j4
```

## Running

Run openocd in the firmware folder
```
$ (sudo) openocd
```

Then use gdb to load the code
```
$ arm-none-eabi-gdb -q -x openocd.gdb build/francisfrancisx1.elf 
```


## Livebook

To run the notebook without installing Livebook you can use docker
```
$ docker run -p 8080:8080 -p 8081:8081 --pull always -u $(id -u):$(id -g) -v $(pwd):/data ghcr.io/livebook-dev/livebook
```
