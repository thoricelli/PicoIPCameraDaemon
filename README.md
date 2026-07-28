# PicoIPCameraDaemon

A daemon that exposes the eye tracking and face tracking camera on the Pico 4 Pro and Enterprise.  
Note: requires a rooted device!

## Limitations
This daemon talks to the camera via the normal Android CameraService, and with Android 10 enforcing a strict one-camera-per-application with no way to make the camera shared, the `pxreyeservice` (the service responsible for parsing camera data normally) will not work whilst the daemon is active.

If you still want `pxreyeservice` to work, you will have to use another workaround like Toonlink's `eyed` daemon which can be found in the OwoMushi Discord server.

## Running

1. Download the binary `picoipcamerad` in the releases.
2. Then run the following commands
```shell
adb push picoipcamerad /data/local/tmp
adb shell ip addr show wlan0 | grep "inet "
adb shell ./picoipcamerad
```
3. The daemon will now be running on port 9100.
4. You should see, in the previous output the following:  
`inet 192.168.0.xx/24 brd 192.168.0.255 scope global wlan0`
5. Go to: http://192.168.0.xx:9100/face
6. What a beatiful face!
7. To kill the daemon: `pkill picoipcamerad`

All endpoints:
- http://192.168.0.xx:9100/face
- http://192.168.0.xx:9100/left
- http://192.168.0.xx:9100/right

## Building

1. Install the Android NDK, version: r21e (21.4.7075529).   
https://developer.android.com/ndk/downloads
2. Add the root path `ANDROID_NDK_ROOT` to your environment variables.
3. `git submodule update --recursive --remote`
3. Run `make`.

## TODO
More features to come.
- Dashboard via an API to manage daemon.
- Changing the LED brightness individually.
- Changing camera parameters (ISO, EV, shutterspeed).
- General code cleanup, tiny refactoring, and proper testing.