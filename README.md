## MCC Hat Test for Linux (hattest)
**Info:** **hattest** uses the **mcchats** library to access and control **MccHat** devices on the **Raspberry Pi** platform. **hattest** implements most of the **mcchats** library functions. It's designed to run on Raspian stretch arm platform (Raspberry Pi).

**Author:** MccDv

## About
The **hattest** program requires installation of the **mcchats** library, the **Qt** base packages, and the latest Rpi updates. 

### Prerequisites:
---------------
Building the **hattest** program requires C/C++ compilers and qmake tool (if building locally), **Qt** base packages, and the **mcchats** library. The following describes how these prerequisites can be installed on a Raspian stretch distribution.
  
  - Update Raspbian installation packages
  
  ```
     $ sudo apt-get update
     $ sudo apt-get install git

  ```
  - Install Qt base packages
  
  ```
     $ sudo apt install qtbase5-dev
     $ sudo apt install qt5-default
  ```
  - May also need spi package
  
  ```
     $ sudo apt-get install at-spi2-core
  ``` 

### Build Instructions
---------------------

It's possible to build this on Rpi, but that would be a fairly long process. It's better to use a cross-compiler and build on a desktop, then deploy to Rpi. Additional dependancies would likely be needed. Installing Qt (version 5.7.1 has been tested).

If you want to just run the binary, install the prerequisites above on the Rpi, and download the binary from **release**.
