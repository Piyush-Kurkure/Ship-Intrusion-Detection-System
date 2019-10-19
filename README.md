# Ship-Intrusion-Detection-System
An automated system using DNN algorithm that would be capable of detection of real-time ships in a video frames.
## Installation
All the code has been tested on Ubuntu 16.04 (64 bit), Qt creater 5.7, OpenCV 3.2.0, C++.</br>
To run the project directly:
```shell
Copy the SIDS_APPLICATION folder to your system.
Open the directory and simply run the Application-x86_64.AppImage.
```
OR</br>
If you have compiled the project as given in ”INSTRUCTIONS_TO_COMPILE”:</br>
```
You need to copy the “yolov3-tiny.cfg” & “ yolov3-tiny.weights” from “SIDS_APPLICATION” to the the folder where the Application-x86_64.AppImage is generated.
Now simply run the application.
```
## Demo
1. This is the User Interface of the Intrusion system showing 2 options: </br>* Offline Video selection mode </br>* Online Camera selection mode.</br>
<img width="600" alt="UI" src="https://user-images.githubusercontent.com/26343062/67138494-d5a42a80-f1f8-11e9-88df-9c86bb50371a.png">

2. Detection of Ships in Offline video selection mode:
<img width="600" alt="rear" src="https://user-images.githubusercontent.com/26343062/67138495-d89f1b00-f1f8-11e9-894a-43656c0e6739.png">

3. Detection of ships in Online camera selection mode:
<img width="600" alt="front" src="https://user-images.githubusercontent.com/26343062/67138496-da68de80-f1f8-11e9-8695-a5e85835232c.png">
