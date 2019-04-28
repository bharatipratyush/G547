# STM32 Based ALSA Audio Driver

The Advanced Linux Sound Architecture (ALSA) provides audio and MIDI functionality to the Linux operating system. ALSA has the following significant features:

- Efficient support for all types of audio interfaces, from consumer sound cards to professional multichannel audio interfaces.
- Fully modularized sound drivers.
- SMP and thread-safe design.
- User space library (alsa-lib) to simplify application programming and provide higher level functionality.
- Support for the older Open Sound System (OSS) API, providing binary compatibility for most OSS programs.
    
This project uses an STM32F103C8 micrcontroller, to essentially emulate a soundcard device. The controller uses its on-chip 12-bit ADC and communicates over a Virtual Comm Port.This feature, in this projects early stage, has been the reason for unrealistically low sample rates. But! It can surely be improved over time, incorporating block device drivers, and ADC with DMA access. In the __init function, I have used a dummy device declaration to act as a parent to the soundcard device, and another struct *device to be linked with struct snd_card *card. The callback functions are fairly easy to understand.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

What things you need to install the software:
```
* STM32CubeMX Code Configurator
* STM32F103C8 Microcontroller Board
* ST-Link Programmer (for flashing the chip)
* A Linux Distribution OS
* Keil uVision 5 IDE
* ALSA Libraries Installed
```

### Installing

0. Clone everything!
1. Flash the firmware from the uVision project in /firmware dirctly onto the chip, using the ST-Link.
2. Visit https://www.alsa-project.org/wiki/Download and get the latest ALSA-lib. Installation guide comes in a README with the download
3. In /project run cmd:  
```
$ sudo make all
```
4. Load the kernel object file, hence loading the driver
```
$ sudo insmod demo6.ko
```
5. Compile the userscape file 
```
$ gcc -o run userapp.c -lasound
```
6. Run the userspace file and observe


As of 28/4/19 the userapp shall yeild garbage values as the ISR (Interrupt Service Routine) related to data capture has not been implemented.

## Fundamentals of an ALSA Driver

Like a simple char driver this too relies on various callbacks to conduct various operations such as updating operating parameters, changing the hardware state, copying data to and from the userspace. In the files included, the .code does not implement interrupts that the hardware ought to send to the OS to schedule the analog-to-digital conversion. In its current state the code will register a device that will be recognised by all programs as a low level SoundCard. One can verify this using 

```
$ arecord -l
$ arecord -L
```

And we're mostly done with /project folder. The /POC folder contains a simple testing file that reads from the COM port and writes to a file, simply for verifying proof of concept.

## Caution

Be sure to have the STM32 plugged in before loading the kernel module. One can check for output using putty and directly reading from the COM port. Your system may crash otherwise.



