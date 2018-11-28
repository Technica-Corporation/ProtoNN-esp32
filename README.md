# ProtoNN-esp32
esp-idf project to run EdgeML ProtoNNPredict on the ESP32  


### Dependencies
Building requires the following to be installed:

* ESP-IDF <https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html>
* OpenBLAS compiled for ESP32
 - This repo contains a static library for OpenBLAS but you can build your own using <https://github.com/Technica-Corporation/OpenBLAS>.


### Configure
- Comment out COMMON_WARNING_FLAGS in esp-idf/make/project.mk
 * EdgeML code base fails many of the warning set here. It was quicker to disable these checks than to completely rewrite the EdgeML codebase.  Make sure to revert these changes after working with this project

### Build, Flash and Monitor

```sh
make flash monitor
```


### Adding model and test dataset to device

- Run ProtoNNTrainer on separate device to generate model and params
- Move model and params to single flat folder
- create spiffs image using mkspiffs tool <https://github.com/igrr/mkspiffs>.
  * Make sure mkspiffs settings match esp-idf setting for spiffs
  * shuttle/ contains model, params, and a test files for Statlog shuttle dataset ready to be used with mkspiffs


```sh
mkspiffs -c shuttle/ -b 4096 -p 256 -s 0x100000 spiffs.bin

python esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 115200 write_flash -z 0x110000 spiffs.bin
```
