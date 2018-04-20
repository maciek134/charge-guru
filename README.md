ChargeGuru
=====
This is an application to control SkyRC B6* chargers. It's a replacement for the ChargeMaster application
provided by the manufacturer, which is badly designed and doesn't work on GNU/Linux. It's written in Qt5.

![screenshot](https://i.imgur.com/SSo5eYT.png)

Installation
------------
Dependencies:
```
qt5 qtcharts libusb-1.0 libb6
```

Grab `libb6` from https://github.com/maciek134/libb6.

Run these commands compile the software:
```bash
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release
$ make
```

Either run the programs that use it as root (**not recommended**) or create an udev rule similar to this one:
```udev
SUBSYSTEM=="usb", ATTRS{idVendor}=="0000", ATTRS{idProduct}=="0001", MODE:="666", GROUP="plugdev"
KERNEL=="hidraw*", ATTRS{idVendor}=="0000", ATTRS{idProduct}=="0001", MODE="0666", GROUP="plugdev"
```

Connect the charger to an USB port and start the program.
