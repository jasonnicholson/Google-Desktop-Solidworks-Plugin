The repo is no longer maintained.

# Notes:
I did not write this plugin.  I paid a programing expert to help me with this.  If you are programming expert, please take a look at compiling the project and using it.  You could help patch any bugs you come across.


# To install from binary:
YOu must have SolidWorks installed on your machine.  You must have Google Desktop Installed.  Download the "GoogleDesktopSolidworksPlugin Binaries.zip" and follow the install instruction on the inside of the zip file.


# Compiling:
There is a single "solution" for the project (built in Visual Studio 2010) which encompasses all of the applicable projects.  The projects are as follows:

/common - GoogleDesktopComponentCommon project.  This is built into the Plugin project as it contains the GoogleDesktop registration / unregistration calls.  This came from Google /GoogleDesktopSolidworksPlugin - GoogleDesktopSolidworksPlugin project.  This is the actual plugin project.  It is what gets called to handle the registered extensions when encountered.  It returns all of the file data: name / location, date, type, and most importantly content.  The content is pulled by calling the readFile method built in to the IFilterFileReader project /IFilterFileReader - IFilterFileReader project. This project is COM enabled so that it can be accessed by the plugin /GDSSolidworksPluginSetup - GDSSolidworksPluginSetup project.  This is the WIX project for the installer.  It's supposed to just install the required build files then execute the regsvr32 command at the appropriate time.  The wix installer is not working well all the time.




