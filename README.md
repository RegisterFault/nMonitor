# nMonitor
Another ncurses-based graph/system stats monitor, designed primarily with laptops and power consumption in mind.

This project is an elaborate excuse for me to practice C systems programming and to mess around with linux interactions with firmware.
I'm also playing around with some of the older approaches and practices in c development, such as using lex to handle complex string
parsing and utilizing autotools for the build system.


## Building 

Depends on `libncurses-dev` or `ncurses-devel` package.

To build: 
```bash
./configure
make
sudo make install
```
To Run:
```bash
nmonitor
```
or 
```bash
sudo nmonitor
```

For more detailed cpu information. 

Note: detailed information requires `msr` kernel module to be loaded, and that module is the reason for root privileges. If it isn't aready loaded, you can do so with:
```bash
sudo modprobe msr
```

Root privileges are also necessary for nmonitor to get access to the RAM type and speed, as that
is parsed directly from the DMI tables in `/sys/firmware/dmi/tables/DMI`
