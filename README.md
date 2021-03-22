# SmarterDog
Backend for CleverDog cloud CCTV

## Introduction

Back in 2017, I wrote a report on reverse engineering a WiFi camera of dubious quality (see https://eriknl.github.io/reverse-engineering/2017/09/07/WiFi-camera.html for the full story).
After concluding this thing was certainly not safe to use with its default cloud backend I created a proof of concept implementation of the protocols involved and managed to convert the video stream into a standard RTSP stream. From this proof of concept I then created this somewhat cleaner code base and I have been using this ever since to use the camera.
In the years since then I have had some people asking me to help them with their own devices, I hope this repository is useful to those that want to try and recreate the backend and have fun with their cameras.

## Disclaimer

Please understand this software is just a proof of concept and I am putting this out there with no guarantees it will even work for your device or for more recent firmware versions.

## How to build and use

Since this application has to run 24/7 make sure to clone the repository to the device you will be running it off of. I have had success with both x86_64 CentOS and a Raspberry Pi 3.
Make sure you have the required build environment, you will need Qt with network support (no gui). Then run `qmake` and finally build with `make`.
Since this is application is not a proper forking daemon you will need to create a script to run it through systemd or your init system of choice (on a Raspberry Pi you could start it from `/etc/rc.local`)

You can scan for cameras in your local network with the `-s` switch, it will list the IP address of any detected cameras along with their CID. After obtaining a valid CID you can then start streaming it with the `-S` switch followed by a comma separated list of CIDs.
When running in streaming mode you can access the stream for a given CID with `rtsp://<yourhost>:8086/CID=<yourcid>`.
