# falldetection

This is the fall detection using machine learning

in this fall detection, I can achieve the 98% accuracy , and the precision is 96%, the recall is 98%. but it may vary when test with wearable device.


#Highlights and Specialties:

I refer the paper [A Real-time Patient Monitoring Framework
for Fall Detection](http://research.cs.queensu.ca/home/farhana/assets/Sample_MSc_Project.pdf)

#dataset

use the MobiFall and MobiAct datasets, you can download it [here](https://bmi.teicrete.gr/the-mobifall-and-mobiact-datasets/)

#file description

----client/FallDector<br>
  this is the wearable test application, it is build and running in samsung watch device. it will collect the sensor data, and send it to server
  
----fall_detect_svc.joblib<br>
  this is the generated model, you can load it without trainning.
  
----fall_detection_server.py<br>
  this is a simple server application, running it before running client application.

----tain.ipynb
  this is the training process, I am lazy, and do not reorganized the code, and remove the log, you may need to prepare the machine learing environment first, and download the dataset.
  
