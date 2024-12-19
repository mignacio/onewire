#ifndef _ONEWIRE_
#define _ONEWIRE_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// Platform dependent functions.

// Drive the bus to a high or low logic state.
void drive_bus(uint8_t gpio, uint8_t data);

// Set the gpio to high impedance so the pull up resistor can drive it
// high.
void release_bus(uint8_t gpio);

// Read gpio value.
uint8_t read_bus(uint8_t gpio);

// Initialize gpio peripheral.
uint8_t ow_bus_init(uint8_t gpio);

// Platform INdependent functions.

// Performs a touch reset on the 1-wire gpio.
// Returns 1 if no device is present.
uint8_t ow_touch_reset(uint8_t gpio);

// Write and read one bit from the 1-wire gpio.
void ow_write_bit(uint8_t gpio, uint8_t bit);
uint8_t ow_read_bit(uint8_t gpio);

// Write or read a complete byte from the 1-wire gpio.
void ow_write_byte(uint8_t gpio, uint8_t byte);
uint8_t ow_read_byte(uint8_t gpio);

// Write a 1-Wire data byte and return the sampled result.
uint8_t ow_touch_byte(uint8_t gpio, uint8_t data);

// Write a block of 1-wire data bytes and return the sampled result in the same buffer.
void ow_block(uint8_t gpio, uint8_t *data, int data_len);

uint8_t docrc8(uint8_t value);

// ROM search functions

// Reset the search state for find the 'first' devices on the gpio.
bool ow_search_first(uint8_t gpio, uint8_t found_rom[8]);

// Search for the 'next' devices on the gpio.
bool ow_search_next(uint8_t gpio, uint8_t found_rom[8]);

// Perform the 1-wire search algorithm using the existing search state.
// Returns true if a devices has been found, its ROM code will be stored in found_rom.
bool ow_search(uint8_t gpio, uint8_t found_rom[8]);

// Setup the search state to look for devices that share the same family_code
// on the next call to ow_search_next().
void ow_target_setup(uint8_t family_code);

// Functions not yet implemented.
/*
// Set all devices on the bus to overdrive speed. Return '1' if at least one overdrive
// capable device is detected.
int ow_overdrive_skip(unsigned char *data, int data_len);
// Setup search to skip the current device type on the next call.
void ow_family_skip_setup();
*/

#ifdef __cplusplus
}
#endif
#endif