# Z906-Home-Assistant
This is a Home Assistant integration using Mosquitto Broker to control the Logitech Z906.

<br/><hr/><br/>

## Thank you [zarpli](https://github.com/zarpli)

I want to thank [zarpli](https://github.com/zarpli) for creating the [Logitech-Z906](https://github.com/zarpli/Logitech-Z906) repo that made this integration possible. Please follow his tutorial to connect the ESP32 to the speaker.

<br/>

## Not the final product
Currently this is a very rough sketch and the only things currently working are the volume control on all speakers (main, rear, subwoofer, center) and powering on and off the system. I will also be improving the code a lot more. (I'm not a C++ developer, I do web stuff haha)

<br/>

## Small issue

For some reason my speaker set is a newer version or some sort as I could not read the version number or the status of the device trough the repo of [zarpli](https://github.com/zarpli). I did receive data but was unable to uncypher it. I'm not certain if I'm able to uncypher this. If not, all the help would be appreciated

<br/>

## Todo:
- [ ] Automatic shutdown
- [ ] Reading and showing version
- [ ] Reading and showing current volume (main, rear, center sub)
- [ ] Reading and showing current input
- [ ] Reading and showing current effect
- [ ] Reading and showing power up time
- [ ] Muting/Unmuting

<br/>

## Installation (ESP32)
1. Read the wiring information on [Logitech-Z906](https://github.com/zarpli/Logitech-Z906) and connect it to your `ESP32`. The only wires u need is `GND`, `TX` and `RX`. Connect `TX` to `pin 17` and `RX` to `pin 16`. I also included a `NPN BC548` to turn off and on the device with `pin 15` on the `DE-15` connector. This is connected to `pin 32` on my `ESP32`. I do recommend doing this, else the speaker is turned on 24/7 and could damage it. (Wiring diagram coming soon)
2. Download the [pubsubclient](https://github.com/knolleary/pubsubclient) repo as a zip file.
3. Open `Arduino IDE` and go to `Sketch` -> `Include Library` -> `Add .ZIP Library...` and import both the `Z906.zip` (included in this repo), and the cloned `pubsubclient-master.zip`.
4. Upload your code and it should work!

*If you have any questions, feel free to add me on Discord `BreadJS#0813` or open an issue!*