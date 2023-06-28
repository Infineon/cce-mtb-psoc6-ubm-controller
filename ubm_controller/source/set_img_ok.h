/******************************************************************************
* File Name:   set_img_ok.h
*
* Description: This file contains the macro definitions required for
*              set_img_ok.c file in MCUBoot library.
*
* Related Document: See README.md
*
*******************************************************************************
* $ Copyright 2023-YEAR Cypress Semiconductor $
*******************************************************************************/

#if !defined(SET_IMG_OK_H)
#define SET_IMG_OK_H

#include "cy_flash.h"
#include "flash_qspi.h"
#include "sysflash/sysflash.h"

#define FLASH_ROW_BUF_SZ        CY_FLASH_ALIGN
#define IMG_TRAILER_SZ          CY_FLASH_ALIGN

#define USER_SWAP_IMAGE_OK_OFFS (24)
#define USER_SWAP_IMAGE_OK      (1)
#define IMG_OK_ADDR             (PRIMARY_IMG_START + USER_APP_SIZE - USER_SWAP_IMAGE_OK_OFFS)

#define IMG_OK_SET_FAILED       -1
#define IMG_OK_ALREADY_SET      1
#define IMG_OK_SET_SUCCESS      0

#endif /* SET_IMG_OK_H */
