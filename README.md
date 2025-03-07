# Z906-Home-Assistant
This is my version of connecting your Z906 to the MQTT broker in Home Assistant.


## Thank you [zarpli](https://github.com/zarpli)

I want to thank [zarpli](https://github.com/zarpli) for creating the [Logitech-Z906](https://github.com/zarpli/Logitech-Z906) repo that made this integ


## Not the final product
Currently this is a very rough sketch and the only things currently working are the volume control on all speakers (main, rear, subwoofer, center) and powering on and off the system. I will also be improving the code a lot more. (I'm not a C++ developer, I do web stuff haha)


## Small issue

For some reason my speaker set is a newer version or some sort as I could not read the version number or the status of the device trough the repo of [zarpli](https://github.com/zarpli). I did receive data but was unable to uncypher it. I'm not certain if I'm able to uncypher this. If not, all the help would be appreciated


## Functions:
- [x] Show Z906 version
- [x] Show Temperature
- [x] Show if Dolby is active
- [x] Set input
- [x] Set effect
- [x] Volume up/down for main channel
- [x] Volume up/down for rear channel
- [x] Volume up/down for sub channel
- [x] Volume up/down for center channel
- [x] Save current settings
- [x] Turn on/off system


## Todo:
- [ ] Automatic shutdown
- [ ] Volume slider for main, rear, center and sub channel
- [ ] Show power up times
- [ ] Muting/Unmuting
- [ ] Show project firmware version
- [ ] Connect your Console to the ESP32 for hard control

*I'm planning to fix lots of issues and make this **THE** Z906 integration.**


## Installation (ESP32)
1. **[Connections]:** Connect `TX` to `GPIO 17` and `RX` to `GPIO 16`. I connected `GPIO 32` to `PIN 15` on the `DE-15` connector. I do recommend doing this instead of connecting `PING 15` to `GND` (on the amplifier side), else the speaker is turned on 24/7 and could damage it.
2. **[Packages]:** Install the following required libraries in ArduinoIDE:
    - PubSubClient
    - ArduinoJson
3. **[Success]:** Upload your code and it should work!


## Note
*It could be possible that the whole system is not working, This is a known issue with older/newer version of the Z906. Try changing `0x18` to `0x17` on line `8` in `Z906.h`*


## Thanks

*If you have any questions, feel free to add me on Discord `BreadJS` or open an issue!*