# MultiTerminal
This tool is very usefull when multiple threads in an embedded RTOS share a common stdout stream.
In regular terminals reading the output becomes difficult as many lines originating from different threads are tangled in one long stream of text.
To tackle this I've created MultiTerminal, using MultiTerminal you can create a single Window for each thread!
See screenshots below.

## How it works
MultiTerm is an application based on [Qt 5.2.1](http://qt-project.org/), which makes MultTerm multiplatform.
Qt makes the use of networking very easy, and that is one of the reasons why MultiTerm contains a TCP Server.
When launched, MultiTerminal starts a TCP server for one stream, you can either manually connect a tool or [IoT](http://en.wikipedia.org/wiki/Internet_of_Things) device, or use the SerialDriver that performs COM -> TCP tunneling. (Linux and Mac are not supported by the driver yet)
The physical access to the serialport is a completely separeted process using tcp (loopback) to get to MultiTerminal.
This way, it is possible to either use TCP or COM/TTY, or even locate the physical port on another device.
Also, whenever the driver app crashes no data will be lost. 

## Binaries
Prepared binaries, including Qt framework dll's are available on [my website](http://jeroen3.nl/terminal)

##Screenshots
![alt text](http://dl.dropboxusercontent.com/s/xxmy6cwc8tym2ft/mainwindow.png "MultiTerminal main window")
![alt text](http://dl.dropbox.com/s/evc4kwwqhoj13jn/views.png "MultiTerminal filtered views")
Screenshots of a beta version

#License
Licensed under GPLv2, see LICENSE.md.
