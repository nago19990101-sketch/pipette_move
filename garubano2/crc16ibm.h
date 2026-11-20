#pragma once

//#include "stdafx.h"
//#include "windows.h"
#include "string"

extern uint16_t crc16_table[256];

void make_crc16_table();

uint16_t crc16(uint8_t *buf, size_t len);