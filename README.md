# PicoIPCameraDaemon

A daemon that exposes the eye tracking and face tracking camera on the Pico 4 Pro and Enterprise.  
Note: requires a rooted device!

## Running

### Standalone
1. Download the binary `picoipcamerad` in the releases.
2. Then run the following commands
```shell
adb push picoipcamerad /data/local/tmp
adb shell ip addr show wlan0 | grep "inet "
adb shell ./data/local/tmp/picoipcamerad
```
3. The daemon will now be running on port 9100.
4. You should see, in the previous output the following:  
`inet 192.168.0.xx/24 brd 192.168.0.255 scope global wlan0`
5. Go to: http://192.168.0.xx:9100/face
6. What a beatiful face!
7. To kill the daemon: `pkill -f picoipcamerad`

### Magisk

You can install the ZIP file from the releases tab.

All endpoints:
- http://192.168.0.xx:9100/face
- http://192.168.0.xx:9100/left
- http://192.168.0.xx:9100/right

## Building

1. Install the Android NDK, version: r21e (21.4.7075529).   
https://developer.android.com/ndk/downloads
2. Add the root path `ANDROID_NDK_ROOT` to your environment variables.
3. 
```bash
git submodule init
git submodule update --recursive --remote
```
4. Install `make`, `cmake` and `ninja`
5. Run `make`.

## TODO
More features to come.
- Dashboard via an API to manage daemon.
- Changing the LED brightness individually.
- Changing camera parameters (ISO, EV, shutterspeed).
- General code cleanup, tiny refactoring, and proper testing.
