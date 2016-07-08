# A4Project

The Project is installed for Dissertation

Systemenvironment: Windows 7 x64

Software:
Visual Studio Community 2015
OpenNI 2.2 + Nite 2.2
Unity 5.3.4f1
ARToolKit 5
Vuforia 4
LabVIEW 2013
Python 3.5 + PyWin32 3.5
Kinect SDK 1.8

Notes:

1. Working directory of Visual Studio 2015 can be found with
      right click current Project in Solution Explorer >> Properties >> Cofiguration Properties >> Debugging
    All Files in OpenNI2\Redist have to be copied into the working directory.
	
2. Installation of OpenNI+Nite+SensorKinect can be found in following link
	http://fivedots.coe.psu.ac.th/~ad/kinect/index.html
	use x86, don't use x64, because the old version ARToolKit is x86. Using x64 can cause compile problem.
	And before installation, don't forget to delete all application and root folder of openNI, Kinect, PrimeSense and Nite.
	The Installtion of openNI+Nite is seperate from ARToolKit.
3. openVRML version: OpenVRML-0.14.3-win32.zip (for now)
4. ARTK version: ARToolKit-2.72.1-bin-win32.zip (for now)
5. msvcp71d.dll, msvcr71d.dll have to be downloaded and copy them to ARToolKit/bin, windows/system32 and windows/sysWOW64
6. Every new project, Data, lib & wrl folders have to be copied and placed into the project folder with header and cpp files
7. Use code from c file, sometime can change the c file directly to cpp file by changing the extension name
8. All programming environment to win7 with x86 or 32, don't use x64

