# senfOS (this name might be changed in future updates)

<!-- Video Demo: https://www.youtube.com/watch?v=yMOm_irrVgQ -->
[![senfOS Demo](https://img.youtube.com/vi/yMOm_irrVgQ/maxresdefault.jpg)](https://www.youtube.com/watch?v=yMOm_irrVgQ)

Watch the demo video by clicking the image above ☝️

## Description & Features

senfOS™ 0.1 (Made in Germany) is powered by the Raspberry Pico 2W and Waveshare OLED 1.3 module. It features a small built-in http server with a small lightweight-filesystem to host html websites. Website visitors are able to send messages directly to your screen. It can connect to Wi-Fi networks or if no WiFi network is provided, it will automatically create a Wi-Fi hotspot that you can use to connect to and view the website. 

Interesting for developers: 
- Built-in http client to fetch content and display on the OLED driver
- Modified OLED library (minor changes)
- Control library for the on-board WiFi-led
- and more features will be added step-by-step.

Made in Germany. Built with C/C++, PicoSDK and Waveshare library.

## Roadmap

- [ ] Using an GPIO extender board to connect sensors and modules and output them on the example website
- [ ] Extend libs/myhttp to also support POST requests (currently only GET works) for specific API requests
- [ ] Integrating a tiny LLM to monitor the hardware/software in case of failure or to fulfill small tasks
- [ ] Extending support for an eInk paper module from Waveshare
- [ ] Redesign the example page to make it look more appealing
- [ ] Implement HTTPS for webserver
- [ ] Dynamic DNS feature to update the DNS using a free DynDNS service

### Hardware Used
- [Raspberry Pico W 2](https://www.raspberrypi.com/products/raspberry-pi-pico-2/?variant=pico-2-w)
- [Waveshare OLED 1.3" Display](https://www.waveshare.com/pico-oled-1.3.htm)
- [Raspberry Pi USB Micro Cable](https://www.welectron.com/Offizielles-Raspberry-Pi-micro-USB-Kabel)
- [TP-Link](https://www.tp-link.com/de/home-networking/mifi/m7350/)
- [Ja!mobil SIM card](https://www.jamobil.de/)

### Software Used
- [PicoSDK](https://github.com/raspberrypi/pico-sdk)
- [Waveshare Library](https://github.com/waveshare/Pico_code)

### Software Used For Coding & Building Process
- [CMake](https://cmake.org/)
- [Visual Studio Code (VSCode)](https://code.visualstudio.com/download)
- [Raspberry Pi Pico VSCode Extension](https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico)

## How To Build

### Hardware

Solder the pins like shown in the picture:

![Soldering Instructions](https://i.imgur.com/nMtbgJg.png)

After you've soldered the pins, mount the display like this:

![Display Mounting Instructions](https://i.imgur.com/fwDtnKr.png)

As you can see the usb micro port is on the right side parallely aligning with "Pico-OLED-1.3" of the oled driver.

### Software

#### Using macOS

Install VS Code and the VS Code Extension for Raspberry Pico (use the official extension from above). Create a first project using the Pico Extension (on the left side bar you can see the extension, click on that) and create a new C++ project.

![Raspberry Pico Extension Usage](https://i.imgur.com/79uftAe.png)

After you clicked "New C/C++ Project", type a project name and select all "Features" like shown above. Also select "Generate C++ code", "Enable C++ RTTI", "Enable C++ Exceptions".

![First project feature settings](https://i.imgur.com/9FxARNB.png)

![Create project](https://i.imgur.com/SGKTDXT.png)

Select everything like in the pictures shown above, then click on "Create". It can take up to 15 minutes until everything is downloaded. Let it finish downloading (it will install PicoSDK and CMake)!

Download the project using `git`:
```bash
git clone https://github.com/qubit999/senfOS.git
```

After that you can instead import a project, select this project source code and import it. Import the project folder "os_".

Now you're ready to play around and build.

#### Settings

You might want to set your own Wi-Fi Network, you can do this by editing the file "settings.h" and set your own Wi-Fi network and password. Currently it only supports WPA2-PSK Network but you can edit the file "libs/mywifi/mywifi.cpp" and replace this parameter "CYW43_AUTH_WPA2_AES_PSK" in line 28 with any WiFi encryption setting you like. Just type "CYW43_AUTH" and Visual Code suggests a few different options.

```cpp settings.h
#define SSID "FRITZBOX 1234"
#define PASSWORD "01234567890ABC"
#define APN "internet.t-d1.de" // APN for German T-Mobile SIM card, will be implemented later
#define HOTSPOT_NAME "senf-hotspot"
#define HOTSPOT_PASS "mittelscharf"
// HTTP SERVER PORT can be changed in ./myhttpserver/lwipopts.h => line 57
// The HTTP Server serves the same files for HOTSPOT MODE and WI-FI MODE
```

You can also set a different hotspot name and passwort.

If you want to host your own html files, you can add your files to "./myhttpserver/fs" but bear in mind, you would also need to change the code of "./myhttpserver/myhttpserver.cpp". If you simply want to host a single page, you can just replace the "index.html" file.

The default port of the http server is set to "9999". If you plan to change it, you can do so by editing ./myhttpserver/lwipopts.h => line 57.

If you want to use a SIM card for hosting your server, you need to change APN settings of your LTE Router to point to an APN which gives you a public IPv4 address. In Germany you can only use "Telekom Network" to get a free public IPv4 address and you need to set the APN to "internet.t-d1.de". 

After that you need to set up port forwarding. If you use the TP-Link 4G LTE Router, you can't use the external port "80" since this port is reserved for the Router Web Interface but you can set any other port you like. Set the internal port to "9999" (or the port you specified in line 57 of ./myhttpserver/lwipopts.h) and the external port to "9999" for example. Your website will be accessible with the external port, as an example "http://83.123.70.19:9999" (but only http protocol, https is not implemented yet). Change to your default WiFi network before you browse the website.

### Build Process

Connect the Micro USB cable to your MacBook. Hold down the bootselect button while plugging the Raspberry Pico in. A volume will pop up in Finder (make sure you see that Volume).

When you open the Raspberry Pico extension, you need to click "Clean CMake", then "Configure CMake" and "Run Project". It will start flashing and automatically reboot the Pico. There will be also an UF2 image inside the /build directory. The build directory will be automatically created when you hit "Clean CMake".

Finish. Have fun.

## License

I haven't decided yet. As of now: Please use this project only for non-commercial purposes. No warranty or liability at all. Don't remove the boot screen (it's advertisement).

## Support Development Of The Project

If you use this project and you want to support the development, please [buy me a coffee](https://ko-fi.com/alexsla). 

email for business: regatta-42.pommel@icloud.com

## Bugs or Feature Requests

Open an issue on GitHub.