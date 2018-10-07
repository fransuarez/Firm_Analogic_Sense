/*
 * ciaaEEPROM.h
 *
 *  Created on: 19/7/2018
 *      Author: fran
 */

#ifndef MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAEEPROM_H_
#define MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAEEPROM_H_

void EEPROM_init (void);

/**
 * @brief       Write a byte in EEPROM memory
 * @param       addr : Address relative to eeprom start (0x0000 to 0x3F7F)
 * @param       value : Byte to be written in specified address
 * @return      void
 */
void EEPROM_writeByte(uint32_t addr,uint8_t value);

/**
 * @brief       Read a byte from EEPROM memory
 * @param       addr : Address relative to eeprom start (0x0000 to 0x3F7F)
 * @return      byte value read
 */
uint8_t EEPROM_readByte(uint32_t addr);



#endif /* MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAEEPROM_H_ */
