#include "onewire.h"
#include <string.h>

// OW timing in microseconds
const int   A = 6,
            B = 64,
            C = 60,
            D = 10,
            E = 9,
            F = 55,
            G = 0,
            H = 480,
            I = 70,
            J = 410;

void ow_delay_us(){
    #ifndef PLATFORM
        #error "No platform code for 'ow_delay_us()'."
    #endif
}

void drive_bus(uint8_t gpio, uint8_t data){
    #ifndef PLATFORM
        #error "No platform code for 'drive_bus()'."
    #endif
}

void release_bus(uint8_t gpio){
    #ifndef PLATFORM
        #error "No platform code for 'release_bus()'."
    #endif
}

uint8_t read_bus(uint8_t gpio){
    uint8_t return_val = 0;
    #ifndef PLATFORM
        #error "No platform code for 'read_bus()'."
    #endif
    return return_val & 0x01;
}

uint8_t ow_bus_init(uint8_t gpio){
    uint8_t err = 0;
    #ifndef PLATFORM
        #error "No platform code for 'ow_bus_init()'."
    #endif
    return err;
};

// Return 1 if device is NOT present.
uint8_t ow_touch_reset(uint8_t gpio){

    drive_bus(gpio, 1);

    ow_delay_us(G);

    drive_bus(gpio, 0);

    ow_delay_us(H);
    release_bus(gpio);

    ow_delay_us(I);
    uint8_t result = read_bus(gpio);

    ow_delay_us(J);

    return result;
};

void ow_write_bit(uint8_t gpio, uint8_t bit){
    if(bit){
        drive_bus(gpio, 0);
        ow_delay_us(A);

        release_bus(gpio);
        ow_delay_us(B);
    }else{
        drive_bus(gpio, 0);
        ow_delay_us(C);

        release_bus(gpio);
        ow_delay_us(D);
    }
};

uint8_t ow_read_bit(uint8_t gpio){

    drive_bus(gpio, 0);
    ow_delay_us(A);

    release_bus(gpio);
    ow_delay_us(E);

    uint8_t result = read_bus(gpio);
    ow_delay_us(F);

    return result;
};

void ow_write_byte(uint8_t gpio, uint8_t byte){
    uint8_t bit = byte;
    for(uint8_t indx = 0; indx < 8; indx++){
        ow_write_bit(gpio, bit & 0x01);
        bit >>= 1;
    }
};

uint8_t ow_read_byte(uint8_t gpio){
    uint8_t byte = 0;
    for(uint8_t indx = 0; indx < 8; indx++){
        byte >>= 1;
        if(ow_read_bit(gpio)){
            byte |= 0x80;
        }
    }
    return byte;
};

uint8_t ow_touch_byte(uint8_t gpio, uint8_t data){
    int result = 0;
    for (uint8_t indx = 0; indx < 8; indx++){
        // shift the result to get it ready for the next bit
        result >>= 1;
        // If sending a '1' then read a bit else write a '0'
        if (data & 0x01){
            if (ow_read_bit(gpio)){
                result |= 0x80;
            }
        }else{
            ow_write_bit(gpio, 0);
        }
        // shift the data byte for the next bit
        data >>= 1;
    }
    return result;
}

void ow_block(uint8_t gpio, uint8_t *data, int data_len){
    for(uint8_t indx = 0; indx < data_len; indx++){
        data[indx] = ow_touch_byte(gpio, data[indx]);
    }
}

// Global search state
static uint8_t ROM_NO[8] = {0};
static uint8_t last_discrepancy = 0;
static uint8_t last_family_discrepancy = 0;
static bool last_device_flag = false;
static uint8_t crc8 = 0;

bool ow_search_first(uint8_t gpio, uint8_t found_rom[8]){
    // reset the search state
    last_discrepancy = 0;
    last_device_flag = false;
    last_family_discrepancy = 0;

    return ow_search(gpio, found_rom);
}

bool ow_search_next(uint8_t gpio, uint8_t found_rom[8]){
    return ow_search(gpio, found_rom);
}

bool ow_search(uint8_t gpio, uint8_t found_rom[8]){
    int id_bit_number;
    int last_zero, rom_byte_number;
    bool search_result;
    int id_bit, cmp_id_bit;
    uint8_t rom_byte_mask, search_direction;

    //initialize for search
    id_bit_number = 1;
    last_zero = 0;
    rom_byte_number = 0;
    rom_byte_mask = 1;
    search_result = false;
    crc8 = 0;

    if(last_device_flag == false){
        if(ow_touch_reset(gpio) == 1){
            // reset the search
            last_discrepancy = 0;
            last_device_flag = false;
            last_family_discrepancy = 0;
            return false;
        }

        // Send search command (no alarm).
        ow_write_byte(gpio, 0xF0);

        do{
            // Read bit and its complement.
            id_bit = ow_read_bit(gpio);
            cmp_id_bit = ow_read_bit(gpio);

            // Check for no devices on the gpio.
            if((id_bit == 1) && (cmp_id_bit == 1)){
                break;
            }else{
                if(id_bit != cmp_id_bit){
                    search_direction = id_bit;
                }else{
                    if(id_bit_number < last_discrepancy){
                        search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
                    }else{
                        search_direction = (id_bit_number == last_discrepancy);
                    }
                    // If 0 was picked record its position in last_zero.
                    if(search_direction == 0){
                        last_zero = id_bit_number;

                        // Check for last discrepancy in family.
                        if(last_zero < 9){
                            last_family_discrepancy = last_zero;
                        }
                    }
                }
                // Set or clear the bit in the ROM byte rom_byte_number
                // with mask rom_byte_mask
                if(search_direction == 1){
                    ROM_NO[rom_byte_number] |= rom_byte_mask;
                }else{
                    ROM_NO[rom_byte_number] &= ~rom_byte_mask;
                }

                ow_write_bit(gpio, search_direction);

                // Increment the byte counter, id_bit_number and shift rom_byte_mask.
                id_bit_number++;
                rom_byte_mask <<= 1;

                // If the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask.
                if(rom_byte_mask == 0){
                    docrc8(ROM_NO[rom_byte_number]); //accumulate the CRC
                    rom_byte_number++;
                    rom_byte_mask = 1;
                }
            }
        }while(rom_byte_number < 8); // Loop through all rom bytes.

        if(!((id_bit_number < 65) || (crc8 != 0))){
            last_discrepancy = last_zero;

            if(last_discrepancy == 0){
                last_device_flag = true;
            }

            search_result = true;
            memcpy(found_rom, ROM_NO, 8 * sizeof(uint8_t));
        }
    }

    // If no devices were found reset counters so next 'search' will be like a first.
    if(!search_result || !ROM_NO[0]){
        last_discrepancy = 0;
        last_device_flag = false;
        last_family_discrepancy = 0;
        search_result = false;
    }

    return search_result;
}

// Setup the search to find devices with the same 'family_code' on the next call
// to ow_search_next().
void ow_target_setup(uint8_t family_code){
    ROM_NO[0] = family_code;
    for(uint8_t indx = 1; indx < 8; indx++){
        ROM_NO[indx] = 0;
    }

    last_discrepancy = 64;
    last_family_discrepancy = 0;
    last_device_flag = false;

}

static uint8_t dscrc_table[] = {
0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
157,195, 33,127,252,162, 64, 30, 95, 1,227,189, 62, 96,130,220,
35,125,159,193, 66, 28,254,160,225,191, 93, 3,128,222, 60, 98,
190,224, 2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89, 7,
219,133,103, 57,186,228, 6, 88, 25, 71,165,251,120, 38,196,154,
101, 59,217,135, 4, 90,184,230,167,249, 27, 69,198,152,122, 36,
248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91, 5,231,185,
140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
202,148,118, 40,171,245, 23, 73, 8, 86,180,234,105, 55,213,139,
87, 9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};

uint8_t docrc8(uint8_t value){
    crc8 = dscrc_table[crc8 ^ value];
    return crc8;
}