#pragma once

#include <stdint.h>

uint16_t calc_crc16(const void *buf, uint8_t len, uint16_t crc);
void calc_crc16_fly(uint16_t *pCRC16, uint8_t r_byte);