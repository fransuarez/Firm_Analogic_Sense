/*
 * ciaaEEPROM.h
 *
 *  Created on: 19/7/2018
 *      Author: fran
 */

#ifndef MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAEEPROM_H_
#define MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAEEPROM_H_

#define EEPROM_ADDRESS_LOW	0x0000
#define EEPROM_ADDRESS_HIGH 0x3F7F

/*
 * @brief Macro defines for EEPROM Auto Programming register
 */
#define EEPROM_AUTOP_OFF     		  (0)		//!<Auto programming off.
#define EEPROM_AUTOP_1WORDWRITTEN     (1)		//!< Erase/program cycle is triggered after 1 word is written.
#define EEPROM_AUTOP_LASTWORDWRITTEN  (2)		//!< Erase/program cycle is triggered after a write to AHB address ending with ......1111100 (last word of a page).
#define EEPROM_PAGE_CHAR_SIZE		  (128)

void EEPROM_init (uint8_t);

/**
 * @brief       Write a byte in EEPROM memory
 * @param       addr : Address relative to eeprom start (0x0000 to 0x3F7F)
 * @param       value : Byte to be written in specified address
 * @return      void
 */
int EEPROM_writeByte (uint32_t , uint8_t );

/**
 * @brief       Read a byte from EEPROM memory
 * @param       addr : Address relative to eeprom start (0x0000 to 0x3F7F)
 * @return      byte value read
 */
int EEPROM_readByte (uint32_t , uint8_t * );

int EEPROM_writeWord (uint32_t , uint32_t );

int EEPROM_readWord (uint32_t , uint32_t * );

int EEPROM_writePage (uint32_t , uint8_t * );

int EEPROM_readPage (uint32_t , uint8_t* );

void EEPROM_writeEnd_ISR (void);


#endif /* MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAEEPROM_H_ */
