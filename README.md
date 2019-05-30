# Sound-Direction-Detection
This project is to implement a sound direction detection solution to guide the robot to orient to the sound source for further action.

Here are some implementation details:
Steps to setup software:
  1. Install Raspbian image on SDcard for Raspberry Pi
  2. Install Matrix Creator software by following Matrix documentation (I installed Matrix Core and python setup)
  3. Install ODAS for sound detection (https://www.hackster.io/97259/direction-of-arrival-for-matrix-voice-creator-using-odas-b7a15b)

Steps to setup hardware:
  1. Connect maxtrix to Raspberry Pi and insert SD card and boot Raspbery Pi
  2. Connect UART usb cable of Stewart system to Raspberry Pi.

Steps to run:
  1. Replace the file ~/odas/demo/matrix-demos/matrix-odas.cpp with the attached file
  2. Copy the uartproc.cpp to ~/odas/demo/matrix-demos/
  3. Update ~/odas/CMakeLists.txt to add uartproc.cpp to the matrix-odas executable build
  4. regenerate the make file and make
  5. run "cd ~/odas/bin/", and "./matrix-odas" in one console and "./odaslive -vc ../config/matrix-demo/matrix_createor.cfg" in another console
  6. Or alternatively run "./matrix-odas &; ./odaslive -vc ../config/matrix-demo/matrix_createor.cfg" in the same console

Notes/Issues:
  1. The tuning of the sound sensitivity can be done in file matrix-odas.cpp. Please serach keyword "FBTASK"
  2. I created another thread in matrix-odas.cpp to poll the angle data every 2 seconds. this can be changed to any time interval with attention that the UART transmission needs 100+ms to ensure the data is OK and also the robot moves a little bit slowly so fast angle update will not help much.
  3. The UART port name keeps changing among "ACM0", "ACM1", "ACM2" during the execution. I have a ugly workaround in uartproc.cpp.
  4. The matrix board doesn't fit to robot with all calbles plugged in.
  5. When matrix board sits on robot, there could be a angle detection confusion problem. For example, if the robot turns to 45 degrees direction when it hears a sound in that direction, if the sound keeps ringing there, the matrix board will detect "0 degree"  and pass this information to robot. Then robot will turn to "0 degree" which is not thwhere the sound comes from.  We need some compensation for the robot physical orientation.
