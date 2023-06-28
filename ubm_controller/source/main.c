/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the UBM Controller application 
*              for the UBM bootloader code example for ModusToolbox.
*
* Related Document: See README.md
*
*******************************************************************************
* $ Copyright 2023-YEAR Cypress Semiconductor $
*******************************************************************************/

#include "cybsp.h"
#include "cyhal.h"

/* MCUboot header file */
#include "bootutil/bootutil.h"

/* Watchdog header file */
#include "watchdog.h"

/* UBM header file */
#include "mtb_ubm.h"
#include "mtb_ubm_config.h"

/*******************************************************************************
* Macros
********************************************************************************/
/* The size of data to store in EEPROM. Note flash size used will be
 * closest multiple of flash row size */
#define DATA_SIZE                       (256)

 /* The Simple Mode is turned off */
#define SIMPLE_MODE                     (1u)

/* Increases the flash endurance twice */
#define WEAR_LEVELING                   (2u)

/* The Redundant Copy is turned off */
#define REDUNDANT_COPY                  (0u)

/* The Blocking Write is turned on */
#define BLOCKING_WRITE                  (1u)

/*******************************************************************************
* Global Variables
********************************************************************************/
CY_ALIGN(CY_EM_EEPROM_FLASH_SIZEOF_ROW)
const uint8_t emEepromStorage[CY_EM_EEPROM_GET_PHYSICAL_SIZE(DATA_SIZE, SIMPLE_MODE, WEAR_LEVELING, REDUNDANT_COPY)] = { 0u };

cy_stc_eeprom_config_t eepromConfig =
{
    .eepromSize = DATA_SIZE,
    .simpleMode = SIMPLE_MODE,
    .wearLevelingFactor = WEAR_LEVELING,
    .redundantCopy = REDUNDANT_COPY,
    .blockingWrite = BLOCKING_WRITE,
    .userFlashStartAddr = (uint32_t) & (emEepromStorage[0u]),
};

mtb_stc_ubm_fru_oa_config_t overview_area =
{
    .two_wire_device_arrangement = MTB_UBM_FRU_OA_2WIRE_ARRANGEMENT_NO_MUX, /* Two-wire device arrangement. */
    .two_wire_mux_address = 0x00U,                                          /* Two-wire mux adress. */
    .two_wire_max_byte_count = MTB_UBM_FRU_OA_2WIRE_MUX_BYTE_CNT_32BYTES,   /* Two-wire max byte count. */
    .ubm_max_time_limit = 0x04U,                                            /* Device max. time limit. */
    .ubm_controller_features =
    {
        .read_checksum_creation = true,                                     /* Indicates whether to add the checksum to the read phase of the two wire transaction. */
        .write_checksum_checking = true,                                    /* Indicates whether to verify the checksum on the write phase of the two wire transaction. */
        .cprsnt_legacy_mode = false,                                        /* Indicates the behavior of the CPRSNT#/CHANGE_DETECT# signal. */
        .pcie_reset_change_count_mask = false,                              /* Indicates if a change to the PCIe Reset field causes the Change Count field to increment. */
        .drive_type_installed_change_count_mask = false,                    /* Indicates if a change to the Drive Type Installed field causes the Change Count field to increment. */
        .operational_state_change_count_mask = false,                       /* Indicates if a change to the Operational State field causes the Change Count field to increment. */
        .perst_management_override = 0x00,                                  /* Indicates the DFC PERST# behavior when a Drive has been installed. */
        .smbus_reset_control = false                                        /* Controls the DFC SMBRST# signal for all DFCs associated under the HFC. */
    },
    .maximum_power_per_dfc = 0x00U,                                         /* Maximum power per DFC. */
    .mux_channel_count = MTB_UBM_FRU_OA_2W_MUX_NO_MUX,                      /* Mux channel count. */
    .enable_bit_location = MTB_UBM_FRU_OA_2W_MUX_ENABLE_NA,                 /* Enable bit location. */
    .mux_type = MTB_UBM_FRU_OA_2W_MUX_CH_ENABLE_LOC,                        /* Mux type. */
};

mtb_stc_ubm_backplane_cfg_t ubm_backplane_configuration =
{
    .fru_config = &eepromConfig,            /* Storage configuration for the FRU */
    .overview_area = &overview_area,        /* Overview area configuration */
    .hpt0_signal_support = true,            /* Indicates if HPT0 signal is routed in the backplane.*/
    .starting_slot = 0x00U,                 /* UBM starting slot. Zero by Default. */

    .silicon_identity =
    {
        .pcie_vendor_id = 0xaa55,           /* PCIe Vendor ID */
        .device_code = 0xface8d00,          /* UBM Controller Device code */
        .fw_version_minor = 0x12,           /* UBM Controller Image Version Minor */
        .fw_version_major = 0x21,           /* UBM Controller Image Version Minor */
        .vendor_specific = 0x1234,          /* UBM Controller vendor-specific data */
    },

    /* .backplane_info. This is Used in UBM MW to determine Backplane type. */
        .backplane_info =
    {
        .backplane_type = 0x05U,            /* Backplane type */
        .backplane_number = 0x0AU,          /* Backplane number */
    },

    /* Customer specific section.*/
    .capabilities =
    {
        .clock_routing = true,              /* Indicates availability of high speed differential clock routing (i.e., RefClk)
                                               from the Host Facing Connector to the Drive Facing Connector */
        .slot_power_control = true,         /* Indicates if the Drive Facing Connectors support Power Disable (i.e., PwrDIS signal) */
        .pcie_reset_control = true,         /* Indicates if PCIe Reset Control is supported */
        .dual_port = true,                  /* Indicates if Dual Port DFC connectors are routed */
        .i2c_reset_operation = MTB_UBM_CAP_2WIRE_RESET_OP_FRU_CONTROLLER, /* Indicates the 2WIRE_RESET# signal support */
        .change_detect_interrupt = true,    /* Indicates if the CHANGE_DETECT# signal interrupt operation is supported */
        .dfc_change_count = true,           /* Indicates if the change count is maintained per individual DFC Status and Control Command Descriptor */
        .prsnt_reported = true,             /* Indicates if the PRSNT# signal is reported */
        .ifdet_reported = true,             /* Indicates if the IFDET# signal is reported */
        .ifdet2_reported = true,            /* Indicates if the IFDET2# signal is reported */
        .perst_override_supported = true,   /* Indicates if the UBM Controller supports the DFC
                                               PERST# Management Override field in the Features Command */
        .smb_reset_supported = true,        /* Indicates if the UBM Controller supports control over the DFC
                                               SMBRST# signals (e.g. See SFF-TA-1009) for all DFCs managed by the HFC */
    },

    .hfc_routing =
    {
        {
         /* HFC Decription sub section */
         .drive_link_width = MTB_UBM_LINK_WIDTH_X1,         // Number of PCI Express lines per Disk. X1 or X4 for the Midplanes we have
        .drive_connector_idx = 0x00,                        // Line number. Comes from the Lead Customer Example
        .port_type = MTB_UBM_PORT_TYPE_CONVERGED,           // Port Type. Depends on NOT supported for UBM 0.5
        .domain = MTB_UBM_PORT_DOMAIN_PRIMARY,              // NOT supported for UBM 0.5
        .hfc_starting_phy_lane = 0x00,                      // HFC Connector number on board.  Comes from the Lead Customer Example
        .hfc_identifier = 0x00,                             // Comes from the Lead Customer Example
        .ubm_ctrl_slave_addr = 0x30U,                       // I2C Address for this drive_connector_idx. Any valid I2C Address
        .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED,   // Comes from the Lead Customer Example. Default value should work

        /* DFC Decription sub section. */
        .dfc_status_and_control = 0x00U,                    // Comes from the Lead Customer Example
        .sff_ta_1001_support = 0x01U,                       // SFF_TA Supported. Default value should work
        .gen_z_support = 0x00U,                             // Default value should work
        .sas_sata_support = 0x01U,                          // Default value should work
        .quad_pcie_support = 0x00U,                         // Default value should work
        .dfc_empty = 0x01U,                                 // Drive detected: 01 - Drive is installed
        /* Other */
        .max_sata_rate = MTB_UBM_SATA_6GBS_RATE,            // Default value should work
        .max_pcie_rate = MTB_UBM_PCIE_4_RATE,               // Default value should work
        .max_sas_rate = MTB_UBM_SAS_4_RATE,                 // Default value should work
        .slot_offset = 0x00U,                               // DFC number
        },

        {.drive_link_width = MTB_UBM_LINK_WIDTH_X1, .drive_connector_idx = 0x01, .port_type = MTB_UBM_PORT_TYPE_CONVERGED, .domain = MTB_UBM_PORT_DOMAIN_PRIMARY, .hfc_starting_phy_lane = 0x01, .hfc_identifier = 0x00, .ubm_ctrl_slave_addr = 0x30U, .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED, .dfc_status_and_control = 0x01U, .sff_ta_1001_support = 0x01U, .gen_z_support = 0x00U, .sas_sata_support = 0x01U, .quad_pcie_support = 0x00U, .dfc_empty = 0x01U, .max_sata_rate = MTB_UBM_SATA_6GBS_RATE, .max_pcie_rate = MTB_UBM_PCIE_4_RATE, .max_sas_rate = MTB_UBM_SAS_4_RATE, .slot_offset = 0x01U, },
        {.drive_link_width = MTB_UBM_LINK_WIDTH_X1, .drive_connector_idx = 0x02, .port_type = MTB_UBM_PORT_TYPE_CONVERGED, .domain = MTB_UBM_PORT_DOMAIN_PRIMARY, .hfc_starting_phy_lane = 0x02, .hfc_identifier = 0x00, .ubm_ctrl_slave_addr = 0x30U, .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED, .dfc_status_and_control = 0x02U, .sff_ta_1001_support = 0x01U, .gen_z_support = 0x00U, .sas_sata_support = 0x01U, .quad_pcie_support = 0x00U, .dfc_empty = 0x01U, .max_sata_rate = MTB_UBM_SATA_6GBS_RATE, .max_pcie_rate = MTB_UBM_PCIE_4_RATE, .max_sas_rate = MTB_UBM_SAS_4_RATE, .slot_offset = 0x02U, },
        {.drive_link_width = MTB_UBM_LINK_WIDTH_X1, .drive_connector_idx = 0x03, .port_type = MTB_UBM_PORT_TYPE_CONVERGED, .domain = MTB_UBM_PORT_DOMAIN_PRIMARY, .hfc_starting_phy_lane = 0x03, .hfc_identifier = 0x00, .ubm_ctrl_slave_addr = 0x30U, .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED, .dfc_status_and_control = 0x03U, .sff_ta_1001_support = 0x01U, .gen_z_support = 0x00U, .sas_sata_support = 0x01U, .quad_pcie_support = 0x00U, .dfc_empty = 0x01U, .max_sata_rate = MTB_UBM_SATA_6GBS_RATE, .max_pcie_rate = MTB_UBM_PCIE_4_RATE, .max_sas_rate = MTB_UBM_SAS_4_RATE, .slot_offset = 0x03U, },
        {.drive_link_width = MTB_UBM_LINK_WIDTH_X1, .drive_connector_idx = 0x00, .port_type = MTB_UBM_PORT_TYPE_CONVERGED, .domain = MTB_UBM_PORT_DOMAIN_SECONDARY, .hfc_starting_phy_lane = 0x00, .hfc_identifier = 0x01, .ubm_ctrl_slave_addr = 0x30U, .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED, .dfc_status_and_control = 0x00U, .sff_ta_1001_support = 0x01U, .gen_z_support = 0x00U, .sas_sata_support = 0x01U, .quad_pcie_support = 0x00U, .dfc_empty = 0x01U, .max_sata_rate = MTB_UBM_SATA_6GBS_RATE, .max_pcie_rate = MTB_UBM_PCIE_4_RATE, .max_sas_rate = MTB_UBM_SAS_4_RATE, .slot_offset = 0x00U, },
        {.drive_link_width = MTB_UBM_LINK_WIDTH_X1, .drive_connector_idx = 0x01, .port_type = MTB_UBM_PORT_TYPE_CONVERGED, .domain = MTB_UBM_PORT_DOMAIN_SECONDARY, .hfc_starting_phy_lane = 0x01, .hfc_identifier = 0x01, .ubm_ctrl_slave_addr = 0x30U, .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED, .dfc_status_and_control = 0x01U, .sff_ta_1001_support = 0x01U, .gen_z_support = 0x00U, .sas_sata_support = 0x01U, .quad_pcie_support = 0x00U, .dfc_empty = 0x01U, .max_sata_rate = MTB_UBM_SATA_6GBS_RATE, .max_pcie_rate = MTB_UBM_PCIE_4_RATE, .max_sas_rate = MTB_UBM_SAS_4_RATE, .slot_offset = 0x01U, },
        {.drive_link_width = MTB_UBM_LINK_WIDTH_X1, .drive_connector_idx = 0x02, .port_type = MTB_UBM_PORT_TYPE_CONVERGED, .domain = MTB_UBM_PORT_DOMAIN_SECONDARY, .hfc_starting_phy_lane = 0x02, .hfc_identifier = 0x01, .ubm_ctrl_slave_addr = 0x30U, .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED, .dfc_status_and_control = 0x02U, .sff_ta_1001_support = 0x01U, .gen_z_support = 0x00U, .sas_sata_support = 0x01U, .quad_pcie_support = 0x00U, .dfc_empty = 0x01U, .max_sata_rate = MTB_UBM_SATA_6GBS_RATE, .max_pcie_rate = MTB_UBM_PCIE_4_RATE, .max_sas_rate = MTB_UBM_SAS_4_RATE, .slot_offset = 0x02U, },
        {.drive_link_width = MTB_UBM_LINK_WIDTH_X1, .drive_connector_idx = 0x03, .port_type = MTB_UBM_PORT_TYPE_CONVERGED, .domain = MTB_UBM_PORT_DOMAIN_SECONDARY, .hfc_starting_phy_lane = 0x03, .hfc_identifier = 0x01, .ubm_ctrl_slave_addr = 0x30U, .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED, .dfc_status_and_control = 0x03U, .sff_ta_1001_support = 0x01U, .gen_z_support = 0x00U, .sas_sata_support = 0x01U, .quad_pcie_support = 0x00U, .dfc_empty = 0x01U, .max_sata_rate = MTB_UBM_SATA_6GBS_RATE, .max_pcie_rate = MTB_UBM_PCIE_4_RATE, .max_sas_rate = MTB_UBM_SAS_4_RATE, .slot_offset = 0x03U, },
        {.drive_link_width = MTB_UBM_LINK_WIDTH_X1, .drive_connector_idx = 0x04, .port_type = MTB_UBM_PORT_TYPE_SEGREGATED, .domain = MTB_UBM_PORT_DOMAIN_PRIMARY, .hfc_starting_phy_lane = 0x00, .hfc_identifier = 0x02, .ubm_ctrl_slave_addr = 0x30U, .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED, .dfc_status_and_control = 0x00U, .sff_ta_1001_support = 0x01U, .gen_z_support = 0x00U, .sas_sata_support = 0x01U, .quad_pcie_support = 0x00U, .dfc_empty = 0x01U, .max_sata_rate = MTB_UBM_SATA_6GBS_RATE, .max_pcie_rate = MTB_UBM_PCIE_4_RATE, .max_sas_rate = MTB_UBM_SAS_4_RATE, .slot_offset = 0x00U, },
        {.drive_link_width = MTB_UBM_LINK_WIDTH_X1, .drive_connector_idx = 0x05, .port_type = MTB_UBM_PORT_TYPE_SEGREGATED, .domain = MTB_UBM_PORT_DOMAIN_PRIMARY, .hfc_starting_phy_lane = 0x01, .hfc_identifier = 0x02, .ubm_ctrl_slave_addr = 0x30U, .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED, .dfc_status_and_control = 0x01U, .sff_ta_1001_support = 0x01U, .gen_z_support = 0x00U, .sas_sata_support = 0x01U, .quad_pcie_support = 0x00U, .dfc_empty = 0x01U, .max_sata_rate = MTB_UBM_SATA_6GBS_RATE, .max_pcie_rate = MTB_UBM_PCIE_4_RATE, .max_sas_rate = MTB_UBM_SAS_4_RATE, .slot_offset = 0x01U, },
        {.drive_link_width = MTB_UBM_LINK_WIDTH_X1, .drive_connector_idx = 0x06, .port_type = MTB_UBM_PORT_TYPE_SEGREGATED, .domain = MTB_UBM_PORT_DOMAIN_PRIMARY, .hfc_starting_phy_lane = 0x02, .hfc_identifier = 0x02, .ubm_ctrl_slave_addr = 0x30U, .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED, .dfc_status_and_control = 0x02U, .sff_ta_1001_support = 0x01U, .gen_z_support = 0x00U, .sas_sata_support = 0x01U, .quad_pcie_support = 0x00U, .dfc_empty = 0x01U, .max_sata_rate = MTB_UBM_SATA_6GBS_RATE, .max_pcie_rate = MTB_UBM_PCIE_4_RATE, .max_sas_rate = MTB_UBM_SAS_4_RATE, .slot_offset = 0x02U, },
        {.drive_link_width = MTB_UBM_LINK_WIDTH_X1, .drive_connector_idx = 0x07, .port_type = MTB_UBM_PORT_TYPE_SEGREGATED, .domain = MTB_UBM_PORT_DOMAIN_PRIMARY, .hfc_starting_phy_lane = 0x03, .hfc_identifier = 0x02, .ubm_ctrl_slave_addr = 0x30U, .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED, .dfc_status_and_control = 0x03U, .sff_ta_1001_support = 0x01U, .gen_z_support = 0x00U, .sas_sata_support = 0x01U, .quad_pcie_support = 0x00U, .dfc_empty = 0x01U, .max_sata_rate = MTB_UBM_SATA_6GBS_RATE, .max_pcie_rate = MTB_UBM_PCIE_4_RATE, .max_sas_rate = MTB_UBM_SAS_4_RATE, .slot_offset = 0x03U, },
        {.drive_link_width = MTB_UBM_LINK_WIDTH_X1, .drive_connector_idx = 0x04, .port_type = MTB_UBM_PORT_TYPE_SEGREGATED, .domain = MTB_UBM_PORT_DOMAIN_SECONDARY, .hfc_starting_phy_lane = 0x00, .hfc_identifier = 0x03, .ubm_ctrl_slave_addr = 0x30U, .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED, .dfc_status_and_control = 0x00U, .sff_ta_1001_support = 0x01U, .gen_z_support = 0x00U, .sas_sata_support = 0x01U, .quad_pcie_support = 0x00U, .dfc_empty = 0x01U, .max_sata_rate = MTB_UBM_SATA_6GBS_RATE, .max_pcie_rate = MTB_UBM_PCIE_4_RATE, .max_sas_rate = MTB_UBM_SAS_4_RATE, .slot_offset = 0x00U, },
        {.drive_link_width = MTB_UBM_LINK_WIDTH_X1, .drive_connector_idx = 0x05, .port_type = MTB_UBM_PORT_TYPE_SEGREGATED, .domain = MTB_UBM_PORT_DOMAIN_SECONDARY, .hfc_starting_phy_lane = 0x01, .hfc_identifier = 0x03, .ubm_ctrl_slave_addr = 0x30U, .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED, .dfc_status_and_control = 0x01U, .sff_ta_1001_support = 0x01U, .gen_z_support = 0x00U, .sas_sata_support = 0x01U, .quad_pcie_support = 0x00U, .dfc_empty = 0x01U, .max_sata_rate = MTB_UBM_SATA_6GBS_RATE, .max_pcie_rate = MTB_UBM_PCIE_4_RATE, .max_sas_rate = MTB_UBM_SAS_4_RATE, .slot_offset = 0x01U, },
        {.drive_link_width = MTB_UBM_LINK_WIDTH_X1, .drive_connector_idx = 0x06, .port_type = MTB_UBM_PORT_TYPE_SEGREGATED, .domain = MTB_UBM_PORT_DOMAIN_SECONDARY, .hfc_starting_phy_lane = 0x02, .hfc_identifier = 0x03, .ubm_ctrl_slave_addr = 0x30U, .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED, .dfc_status_and_control = 0x02U, .sff_ta_1001_support = 0x01U, .gen_z_support = 0x00U, .sas_sata_support = 0x01U, .quad_pcie_support = 0x00U, .dfc_empty = 0x01U, .max_sata_rate = MTB_UBM_SATA_6GBS_RATE, .max_pcie_rate = MTB_UBM_PCIE_4_RATE, .max_sas_rate = MTB_UBM_SAS_4_RATE, .slot_offset = 0x02U, },
        {.drive_link_width = MTB_UBM_LINK_WIDTH_X1, .drive_connector_idx = 0x07, .port_type = MTB_UBM_PORT_TYPE_SEGREGATED, .domain = MTB_UBM_PORT_DOMAIN_SECONDARY, .hfc_starting_phy_lane = 0x03, .hfc_identifier = 0x03, .ubm_ctrl_slave_addr = 0x30U, .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED, .dfc_status_and_control = 0x03U, .sff_ta_1001_support = 0x01U, .gen_z_support = 0x00U, .sas_sata_support = 0x01U, .quad_pcie_support = 0x00U, .dfc_empty = 0x01U, .max_sata_rate = MTB_UBM_SATA_6GBS_RATE, .max_pcie_rate = MTB_UBM_PCIE_4_RATE, .max_sas_rate = MTB_UBM_SAS_4_RATE, .slot_offset = 0x03U, },
    }
};

/* This section maps PSoC 6 MCU Pins to DFC and HFC.*/

mtb_stc_ubm_backplane_control_signals_t ubm_backplane_control_signals =
{
    .dfc_io =
    {
        {.ifdet = P3_5, .ifdet2 = P7_2, .prsnt = P0_5, .actdetect = P3_4, .persta = P9_7, .perstb = P5_6, .pwrdis = P4_0, .refclken = P5_5, .dualporten = P3_3},
        {.ifdet = P2_5, .ifdet2 = P0_4, .prsnt = P6_4, .actdetect = P2_4, .persta = P2_6, .perstb = P12_2, .pwrdis = P11_7, .refclken = P12_1, .dualporten = P3_0},
        {.ifdet = P1_5, .ifdet2 = P7_1, .prsnt = P6_3, .actdetect = P1_4, .persta = P2_7, .perstb = P8_6, .pwrdis = P8_3, .refclken = P8_5, .dualporten = P1_3},
        {.ifdet = P9_5, .ifdet2 = P8_1, .prsnt = P6_5, .actdetect = P9_4, .persta = P9_6, .perstb = P8_0, .pwrdis = P7_7, .refclken = P7_6, .dualporten = P0_0},
    },

    .hfc_io =
    {
        {.sda = P0_3, .scl = P0_2, .i2c_reset = P0_1, .change_detect = P13_3, .bp_type  = P5_7, .perst = P3_2 },
        {.sda = P2_1, .scl = P2_0, .i2c_reset = P2_3, .change_detect = P13_1, .bp_type  = P12_3, .perst = P2_2 },
        {.sda = P6_1, .scl = P6_0, .i2c_reset = P6_2, .change_detect = P13_2, .bp_type  = P11_0, .perst = P1_2 },
        {.sda = P9_1, .scl = P9_0, .i2c_reset = P9_3, .change_detect = P13_0, .bp_type  = P7_4, .perst = P9_2 },
    }
};

static mtb_stc_ubm_context_t cy_ubm_context;

/*******************************************************************************
* Function Prototypes
********************************************************************************/

/******************************************************************************
 * Function Name: main
 ******************************************************************************
 * Summary:
 *  System entrance point. This function initializes system resources & 
 *  peripherals, initializes retarget IO, and toggles the user LED at a
 *  configured interval. 
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
    /* Update watchdog timer to mark successful start up of application */
    cy_wdg_kick();
    cy_wdg_free();

    cy_rslt_t result = CY_RSLT_SUCCESS;
    bool status = true;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    /* Enable global interrupts */
    __enable_irq();
    
    (void) result; /* To avoid compiler warning in release build */

    /* Initialize the UBM algorithm */
    status =  mtb_ubm_init(&ubm_backplane_configuration, &ubm_backplane_control_signals, &cy_ubm_context);
    if (status != false)
    {
        CY_ASSERT(0);
    }
    for (;;)
    {
        /* User Application */

    }

    return 0;
}

/* [] END OF FILE */

