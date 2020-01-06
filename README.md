## MCC DaqHat Test for Linux (mdihattest)
**Info:** **mdihattest** uses the **daqhats** library to access and control **Mcc DaqHat** devices on the **Raspberry Pi** platform. **mdihattest** implements most of the **daqhats** library functions. It's designed to run on Raspian stretch arm platform (Raspberry Pi).

**Author:** MccDv

## About
The **mdihattest** program requires installation of the **daqhats** library, the **Qt** base packages, and the latest Rpi updates. 

### Prerequisites:
---------------
Building the **mdihattest** program requires C/C++ compilers and qmake tool (if building locally), **Qt** base packages, and the **daqhats** library. The following describes how these prerequisites can be installed on a Raspian stretch distribution.
  
  - Update Raspbian installation packages
  
  ```sh
     $ sudo apt-get update
     $ sudo apt-get install git
  ```
  
  - Install Qt base packages
  
  ```sh
     $ sudo apt install qtbase5-dev
     $ sudo apt install qt5-default
  ```
  
  - May also need spi package
  
  ```sh
     $ sudo apt-get install at-spi2-core
  ``` 

### Build Instructions
---------------------

Download the test application to the root of your home folder.

  ```sh
     $ cd ~
     $ git clone https://github.com/MccDv/mdihattest.git
     $ cd mdihattest/
  ```

Edit **mdihattest.pro** to select a build for the version of the daqhats library you want to build against. 
Comment out any version beyond the one you intend to build against.

- Run qmake and then make
  
  ```sh
     $ qmake
     $ make
  ```

If you want to just run the binary rather than building the application, install the prerequisites above on the Rpi, and download the binary from **release**.
