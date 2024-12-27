# onewire

A simple interface, or building block to simplify writing your own code to use 1-Wire devices with a microcontroller of your choosing.
The code is heavily based of Analog Devices application notes, but with my own changes to modernize it and have a consistent coding style.

## Usage

There is a branch per every platform. So first clone the branch of your specific platform.

`git clone -b <your-platform> git@github.com:mignacio/onewire.git`

Then, just include `onewire.h` and use the code as you need.

Example (Write byte to all devices on the bus):
```
#include <onewire.h>

void main(){
    uint8_t 1w_pin = 19; // 19 is the pin number where your 1-Wire bus is connected.
    ow_bus_init(1w_pin); // Initialize the gpio peripheral for that pin to act as a 1-Wire bus.

    ow_touch_reset(1w_pin); // Start transaction with a touch reset.
    ow_write_byte(1w_pin, 0xCC); // Skip rom command.
    ow_write_byte(1_pin, 0x44); // DS18B20 convert temp command.
}
```

If there is no branch for your platform, then you need to create one.

Fill in the platform dependent functions with the code of your MCU or OS to drive, release and read the bus, and wait for certain microseconds, if you try to compile with these empty you´ll get warnings.
Once that's done, include "onewire.h" in your code and you should be good to go. All functions need a gpio or pin number to know which 1-Wire bus to address at runtime.