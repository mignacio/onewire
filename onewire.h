#ifndef _ONEWIRE_
#define _ONEWIRE_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// Platform dependent functions.

/**
 * Drive the bus to a high or low logic state.
 * @param[gpio] Pin number of the 1-Wire bus.
 * @param[data] You can drive the bus high 1 or low 0
*/
void drive_bus(uint8_t gpio, uint8_t data);

/**
 * Set the gpio to high impedance so the pull up resistor can drive it high.
 * @param[gpio] Pin number of the 1-Wire bus.
 */
void release_bus(uint8_t gpio);

/**
 * Read the value of the bus.
 * @param[gpio] Pin number of the 1-Wire bus.
 */
uint8_t read_bus(uint8_t gpio);

// Initialize gpio peripheral.
/**
 * Initialize gpio peripheral for the 1-Wire bus.
 * @param[gpio] Pin number of the 1-Wire bus.
 */
uint8_t ow_bus_init(uint8_t gpio);

// Platform INdependent functions.

/**
 * Performs a touch reset on the 1-Wire gpio.
 * @param[gpio] Pin number of the 1-Wire bus.
 * @return 0 if OK, 1 if no device is present.
 */
uint8_t ow_touch_reset(uint8_t gpio);

/**
 * Write a bit on the 1-Wire bus.
 * @param[gpio] Pin number of the 1-Wire bus.
 * @param[bit] Bit to be written, 1 or 0.
 */
void ow_write_bit(uint8_t gpio, uint8_t bit);

/**
 * Reads a bit from the 1-Wire bus.
 * @param[gpip] Pin number of the 1-Wire bus.
 * @return Value read from the bus.
 */
uint8_t ow_read_bit(uint8_t gpio);

/**
 * Write a byte on the 1-Wire bus
 * @param[gpio] Pin number of the 1-Wire bus.
 * @param[byte] Byte to write.
 */
void ow_write_byte(uint8_t gpio, uint8_t byte);

/**
 * Read a byte from the 1-Wire bus.
 * @param[gpio] Pin number of the 1-Wire bus.
 * @return Byte read from the bus.
 */
uint8_t ow_read_byte(uint8_t gpio);

/**
 * Write a byte on the 1-Wire bus, after every
 * written bit, a inmediate read is performed and the the read byte is returned.
 * @param[gpio] Pin number of the 1-Wire bus.
 * @param[data] Byte to be written.
 * @return Byte read from the bus.
 */
uint8_t ow_touch_byte(uint8_t gpio, uint8_t data);

/**
 * Write a block of 1-wire data bytes and return the sampled result in the same buffer.
 * @param[gpio] Pin number of the 1-Wire bus.
 * @param[data] Pointer to an array of bytes to be written.
 * @param[data_len] Lenght of the data array.
 */
void ow_block(uint8_t gpio, uint8_t *data, int data_len);

uint8_t docrc8(uint8_t value);

// ROM search functions

/**
 * Reset the search state for find the 'first' devices on the gpio.
 * @param[gpio] Pin number of the 1-Wire bus.
 * @param [found_rom[8]] 8 byte array to store the found ROM.
 * @return True if a device was found, false if not.
 */
bool ow_search_first(uint8_t gpio, uint8_t found_rom[8]);

/**
 * Search for the 'next' devices on the gpio.
 * @param[gpio] Pin number of the 1-Wire bus.
 * @param[found_rom[8]] 8 byte array to store the found ROM.
 * @param True if a device was found, false if not.
 */
bool ow_search_next(uint8_t gpio, uint8_t found_rom[8]);

/**
 * Perform the 1-wire search algorithm using the existing search state.
 * @param[gpio] Pin number of the 1-Wire bus.
 * @param[found_rom[8]] 8 byte array to store the found ROM.
 * @return True if a devices has been found, false if not.
 */
bool ow_search(uint8_t gpio, uint8_t found_rom[8]);
/**
 * Setup the search state to look for devices that share the same family_code
 * on the next call to ow_search_next().
 * @param[family_code] Byte representing a family of 1-Wire devices.
*/
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