# Pd++ with Wwise

This is an example of how I got Pd++ to work as part of a Wwise plugin.  NOTE: Wwise uses its own build scripts, notably, the wp.py script that lives in the application scripts folder (at least on Windows it does).

The Wwise workflow is a bit clunky compared to other platforms.  You will want to start here: https://www.audiokinetic.com/en/public-library/2024.1.6_8842/?source=SDK&id=effectplugin_tools.html 

I plan to make a short tutorial on how to go about this in more detail.  For now you could theoretically copy this folder and run the following command: 
```
python "%WWISEROOT%/Scripts/Build/Plugins/wp.py" build -c Release -x x64 -t vc170 Authoring
```
If you want to build for any platform other than the Wwise authoring software these are the options: Android, Authoring, Authoring_Windows, Authoring_Linux, Authoring_Mac, iOS, Linux, LinuxAuto, Mac, NX, OpenHarmony, PS4, PS5, QNX, tvOS, visionOS, Windows_vc160, Windows_vc170, WinGC, XboxOneGC, XboxSeriesX.  Just make sure you have the proper SDKs.

Otherwise, the above command will build the plugin for the Wwise middleware only, not for the platform you are targeting.  So if you are integrating with Unity or Wwise, you will have to build for your specific architecture. Remember if you are targeting Mac/iOS you have to have access to a Mac. 

Look at the "Packaging Your Plug-in" page from the above link if you want to have the plugin be installed via the Wwise Launcher.  (I told you it was clunky)

