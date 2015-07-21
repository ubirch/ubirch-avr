//
// Created by Matthias Jugel on 20/07/15.
//

#ifndef UBIRCH_ISL29125_H
#define UBIRCH_ISL29125_H

// ISL29125 Registers
#define ISL_R_DEVICE_ID     0x00

// config registers
#define ISL_R_COLOR_MODE    0x01
#define ISL_R_FILTERING     0x02
#define ISL_R_INTERRUPT     0x03

// threshold registers
#define ISL_R_THRESHOLD_LL  0x04
#define ISL_R_THRESHOLD_LH  0x05
#define ISL_R_THRESHOLD_HL  0x06
#define ISL_R_THRESHOLD_HH  0x07

// general ans sensor status registers
#define ISL_R_STATUS        0x08
#define ISL_R_GREEN_L       0x09
#define ISL_R_GREEN_H       0x0A
#define ISL_R_RED_L         0x0B
#define ISL_R_RED_H         0x0C
#define ISL_R_BLUE_L        0x0D
#define ISL_R_BLUE_H        0x0E

// reset command
#define ISL_CMD_RESET       0x46

// ISL_R_DEVICE_ID values
#define ISL_DEVICE_ID 0x7d

// ISL_R_MODE values
#define ISL_MODE_POWERDOWN  0b000000 // B0-2 color mode selection
#define ISL_MODE_G          0b000001
#define ISL_MODE_R          0b000010
#define ISL_MODE_B          0b000011
#define ISL_MODE_STANDBY    0b000100
#define ISL_MODE_RGB        0b000101
#define ISL_MODE_RG         0b000110
#define ISL_MODE_GB         0b000111

// the lux selection also indicates sensible bit width (more bits offer more lux)
#define ISL_MODE_375LUX     0b000000 // B3 0 default is 375 lux
#define ISL_MODE_10KLUX     0b001000 // B3 1
#define ISL_MODE_12BIT      0b010000 // B4 1 12 bit operation
#define ISL_MODE_16BIT      0b000000 // B4 0 16 bit operation
#define ISL_MODE_INT_TRIG   0b100000 // set INT pin to trigger sampling

// ISL_R_FILTERING values (the actual filter values can be beweel 0x00 (NONE) and 0x3F (MAX)
#define ISL_FILTER_IR_RANGE 0x80 // default is not capping the values (top & bottom)
#define ISL_FILTER_IR_NONE  0x00 // no IR filtering (or low)
#define ISL_FILTER_IR_MAX   0x3F // maximum IR filter

// ISL_R_INTERRPT values (how to trigger interrupts, if selected)
#define ISL_INTERRUPT_NONE  0b00000 // do not trigger interrupts
#define ISL_INTERRUPT_GREEN 0b00001 // trigger interrupt on green
#define ISL_INTERRUPT_RED   0b00010 // trigger interrupt on red
#define ISL_INTERRUPT_BLUE  0b00011 // trigger interrupt on blue

// the default mode triggers interrupts based on thresholds
#define ISL_INT_ON_THRSLD   0x00 // threshold triggering
#define ISL_INT_ON_SAMPLE   0x10 // trigger interrupt on a finished sample cycle

// status flags
#define ISL_STATUS_INT      0b000001 // interrupt was triggered
#define ISL_STATUS_ADC_DONE 0b000010 // conversion done
#define ISL_STATUS_BROWNOUT 0b000100 // power down or brownout occured
#define ISL_STATUS_GREEN    0b010000 // green being converted
#define ISL_STATUS_RED      0b100000 // red being converted
#define ISL_STATUS_BLUE     0b110000 // blue being converted
#define ISL_STATUS_RGB      (ISL_STATUS_GREEN|ISL_STATUS_RED|ISL_STATUS_BLUE)

#endif //UBIRCH_ISL29125_H
