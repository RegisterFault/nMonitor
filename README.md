# nMonitor
Another ncurses-based graph/system stats monitor, designed primarily with laptops and power consumption in mind.

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
