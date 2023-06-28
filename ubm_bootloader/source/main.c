/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the UBM bootloader code example
*              for ModusToolbox.
*
* Related Document: See README.md
*
*******************************************************************************
* $ Copyright 2023-YEAR Cypress Semiconductor $
*******************************************************************************/

/* Drive header files */
#include "cycfg.h"
#include "cy_result.h"
#include "cyhal.h"
#include "cybsp.h"

#include "cycfg_clocks.h"
#include "cycfg_peripherals.h"
#include "cycfg_pins.h"

/* Flash PAL header files */
#ifdef CY_BOOT_USE_EXTERNAL_FLASH
#include "flash_qspi.h"
#endif

/* MCUboot header files */
#include "bootutil/image.h"
#include "bootutil/bootutil.h"
#include "bootutil/sign_key.h"
#include "bootutil/bootutil_log.h"
#include "bootutil/fault_injection_hardening.h"

/* Watchdog header file */
#include "watchdog.h"
#include "cyw_platform_utils.h"

/*******************************************************************************
* Macros
********************************************************************************/
/* Slave Select line to which the external memory is connected.
 * Acceptable values are:
 * 0 - SMIF disabled (no external memory)
 * 1, 2, 3, or 4 - slave select line to which the memory module is connected. 
 */
#define QSPI_SLAVE_SELECT_LINE              (1UL)

/* WDT time out for reset mode, in milliseconds. */
#define WDT_TIME_OUT_MS                     (4000UL)

/* General MCUBoot module error */
#define CY_RSLT_MODULE_MCUBOOTAPP           (0x500U)
#define CY_RSLT_MODULE_MCUBOOTAPP_MAIN      (0x51U)
#define MCUBOOTAPP_RSLT_ERR \
    (CY_RSLT_CREATE_EX(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MCUBOOTAPP, CY_RSLT_MODULE_MCUBOOTAPP_MAIN, 0))

/******************************************************************************
 * Function Name: deinit_hw
 ******************************************************************************
 * Summary:
 *  This function performs the necessary hardware de-initialization.
 *
 ******************************************************************************/
static void hw_deinit(void)
{
#if defined(CY_BOOT_USE_EXTERNAL_FLASH) && !defined(MCUBOOT_ENC_IMAGES_XIP) && !defined(USE_XIP)
    qspi_deinit(QSPI_SLAVE_SELECT_LINE);
#endif
}


/******************************************************************************
 * Function Name: do_boot
 ******************************************************************************
 * Summary:
 *  This function extracts the image address and enables CM4 to let it boot
 *  from that address.
 *
 * Parameters:
 *  rsp - Pointer to a structure holding the address to boot from. 
 * 
 * Return:
 *  bool
 *
 ******************************************************************************/
static bool do_boot(struct boot_rsp *rsp)
{
    uintptr_t flash_base = 0;

    if (rsp != NULL)
    {
        int rc = flash_device_base(rsp->br_flash_dev_id, &flash_base);

        if (0 == rc)
        {
            fih_uint app_addr = fih_uint_encode(flash_base +
                                                rsp->br_image_off +
                                                rsp->br_hdr->ih_hdr_size);

             rc = flash_device_base(rsp->br_flash_dev_id, &flash_base);
            if ((rc != 0) || fih_uint_not_eq(fih_uint_encode(flash_base +
                                             rsp->br_image_off +
                                             rsp->br_hdr->ih_hdr_size),
                                             app_addr))
            {
                return false;
            }


            hw_deinit();

#ifdef USE_XIP
            qspi_set_mode(CY_SMIF_MEMORY);
#endif

            /* Jump to Application on M4 from here.*/
            psoc6_launch_cm4_app(app_addr);
            return true;
        }
        else
        {
            return false;
        }
    }

    return false;
}


/******************************************************************************
 * Function Name: main
 ******************************************************************************
 * Summary:
 *  System entrance point. This function initializes peripherals, initializes
 *  retarget IO, and performs a boot by calling the MCUboot functions. 
 *
 * Parameters:
 *  void
 *
 * Return:
 *  int
 *
 ******************************************************************************/
int main(void)
{
    struct boot_rsp rsp;
    cy_rslt_t rc = MCUBOOTAPP_RSLT_ERR;
    bool boot_succeeded = false;
    fih_int fih_rc = FIH_FAILURE;

    /* Initialize system resources and peripherals. */
    cybsp_init();

    /* enable global interrupts */
    __enable_irq();

#ifdef CY_BOOT_USE_EXTERNAL_FLASH
    cy_en_smif_status_t qspi_status = qspi_init_sfdp(QSPI_SLAVE_SELECT_LINE);

    if (CY_SMIF_SUCCESS == qspi_status)
    {
        rc = CY_RSLT_SUCCESS;
    }
    else
    {
        rc = MCUBOOTAPP_RSLT_ERR;
    }

#endif

    (void)memset(&rsp, 0, sizeof(rsp));
    FIH_CALL(boot_go, fih_rc, &rsp);

    if (true == fih_eq(fih_rc, FIH_SUCCESS))
    {


        /* Initialize watchdog timer. it should be updated from user app
        * to mark successful start up of this app. if the watchdog is not updated,
        * reset will be initiated by watchdog timer and swap revert operation started
        * to roll back to operable image.
        */
        rc = cy_wdg_init(WDT_TIME_OUT_MS);

        if (CY_RSLT_SUCCESS == rc)
        {
            boot_succeeded = do_boot(&rsp);

        }
    }

    while (true)
    {
        if (!boot_succeeded)
        {
            __WFI();
        }
    }
}

/* [] END OF FILE */

