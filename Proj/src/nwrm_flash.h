#ifndef __NWRM_FLASH_H
#define __NWRM_FLASH_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


//#define FLASH_PAGE_SIZE            	0x200

/** Number of words in a flash page */
#define FLASH_E2P_SIZE_WORD        	(FLASH_PAGE_SIZE / sizeof(uint32_t))




typedef struct _NWRM_DEVICE {
    uint32_t    Serial;            	/**< Serial number (SIGFOX ID). */
    uint32_t    Key1;				/**< Reserved. */
    uint64_t    Key2;				/**< Reserved. */
    uint8_t     ModResult;			/**< Module manufacturing test result. */
    uint8_t     ProdResult;			/**< Product manufacturing test result. */
    uint8_t     LedMask1;			/**< Obsolete. */
    uint8_t     LedMask2;			/**< Obsolete. */
} NWRM_DEVICE;


void NWRM_FLASH_Write(void *buffer, uint32_t count);
bool NWRM_FLASH_Read(void *buffer, uint32_t count);
bool NWRM_FLASH_DeviceRead(NWRM_DEVICE *device);



#ifdef __cplusplus
}
#endif

#endif // __NWRM_FLASH_H
