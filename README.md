# HollowClock
Remix of https://www.thingiverse.com/thing:5636482

The project lacks some fundamental things like a real time clock and a power source. So I fixed that.
1) Adding a DS3231 realtime clock lets the time setting remain after power loss.
2) I added a LiIo 18650 battery.
3) I used a (self designed) ESP32/ESP-C3 based controller "UniMiniC3" so time can be set by mobile phone app via Bluetooth.
Publish of this PCB will come some day here at my GitHub, but the already published "UniMiniESP8266" will do nicely. 
A "D1 mini" controller should  work, too, but requires some wiring and a ULN2003N board as the original project does.
4) A case for the electronics and the 18650 cell was added as I did not want to redesign the gear box.
5) To cover the minute hand with another color, two designs are added.

So have fun with it.
