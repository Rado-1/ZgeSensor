# How to use ZGESensor library in ZGameEditor project #

## Usage in project ##

  1. Create ZGE project or use existing one.
  1. Include _ZExternalLibrary_ component for ZGESensor library to the _OnLoaded_ section of _ZApplication_.
  1. Initialize the library (`sensorInitLib()`) and used sensors (`sensorUse()`). This code is usually placed to the _OnLoaded_ section of _ZApplication_ or _OnStart_ section of _AppState_ component.
  1. Regularly update values from sensors by calling `sensorUpdateData()` function. This is usually placed to _OnUpdate_ section of _ZApplication_ or _AppState_ components.
  1. Use actual values of sensors (obtained by `sensorGetData()` function) in your code to control behavior of models and other objects.
  1. It is recommended to handle disabling sensors (`sensorDisable()`) on pausing the application (handled by _KeyPress_ component with _CharCode_ == 255), and enabling sensors (`sensorEnable()`) on resuming the application (_KeyPress_' _CharCode_ == 254).
  1. Finally, stop the library (`sensorStopLib()`) when application or state is terminated. This is usually placed to _OnClose_ section of _ZApplication_ component or _OnLeave_ section of _AppState_.

## Compiling APK ##

  1. Compile your Android application, e.g., use _Project / Android: Build APK (debug)_ menu item.
  1. Place `libZGESensor.so` file to the `libs/armeabi` folder.
  1. Compile the APK again.
  1. Deploy the APK file to your device and install it in Android OS.

_Note 1: The 1st and 2nd steps are required only when the application is built the first time. After that, a single compilation produces correct APK._

_Note 2: Use the step 2 also in the case when updating to a newer version of the ZGESensor library._