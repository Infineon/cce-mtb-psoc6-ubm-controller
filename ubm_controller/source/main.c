/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the UBM Controller application
*              for the UBM bootloader code example for ModusToolbox.
*
* Related Document: See README.md
*
*
*
*******************************************************************************
* $ Copyright 2023-YEAR Cypress Semiconductor $
*******************************************************************************/
/******************************************************************************
 * Recommended Clock frequency Settings
 *
 * Clock frequency for CM0 ---------- 72MHz
 * Clock frequency for CM4 ---------- 144MHz
 * Peripherals clock frequency ------ 72MHz
 *
 ******************************************************************************/

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

/* FW version for Boot and Upgrade image */
#ifdef BOOT_IMAGE
    #define IMAGE "BOOT"
    #define FW_VERSION 0x10
#elif defined(UPGRADE_IMAGE)
    #define IMAGE "UPGRADE"
    #define FW_VERSION 0x20
#endif

/*******************************************************************************
* Function Prototypes
********************************************************************************/

/* Configuration structures for ubm_init function */
mtb_stc_ubm_backplane_cfg_t ubm_backplane_configuration;
mtb_stc_ubm_backplane_control_signals_t ubm_backplane_control_signals;
mtb_stc_ubm_context_t ubm_context;

/* The size of data to store in EEPROM. Note the flash size used will be
 * the closest multiple of the flash row size */
#define DATA_SIZE                       (256)
/* Simple Mode is turned on */
#define SIMPLE_MODE                     (1U)
/* Increases the flash endurance by twice */
#define WEAR_LEVELING                   (2U)
/* The Redundant Copy is turned off */
#define REDUNDANT_COPY                  (0U)
/* The Blocking Write is turned on */
#define BLOCKING_WRITE                  (1U)

/* UBM Configuration structures */
CY_ALIGN(CY_EM_EEPROM_FLASH_SIZEOF_ROW)
const uint8_t emEepromStorage[CY_EM_EEPROM_GET_PHYSICAL_SIZE(DATA_SIZE, SIMPLE_MODE, WEAR_LEVELING, REDUNDANT_COPY)] __attribute__((used, section(".cy_em_eeprom"))) = { 0U };
cy_stc_eeprom_config_t eepromConfig =
{
    .eepromSize = DATA_SIZE,
    .simpleMode = SIMPLE_MODE,
    .wearLevelingFactor = WEAR_LEVELING,
    .redundantCopy = REDUNDANT_COPY,
    .blockingWrite = BLOCKING_WRITE,
    .userFlashStartAddr = (uint32_t) & (emEepromStorage[0U]),
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
        .perst_management_override = 0x00U,                                 /* Indicates the DFC PERST# behavior when a Drive has been installed. */
        .smbus_reset_control = false                                        /* Controls the DFC SMBRST# signal for all DFCs associated under the HFC. */
    },
    .maximum_power_per_dfc = 0x00U,                                         /* Maximum power per DFC. */
    .mux_channel_count = MTB_UBM_FRU_OA_2W_MUX_NO_MUX,                      /* Mux channel type. */
    .enable_bit_location = MTB_UBM_FRU_OA_2W_MUX_ENABLE_NA,            /* Enable bit location. */
    .mux_type = MTB_UBM_FRU_OA_2W_MUX_CH_ENABLE_LOC,                     /* Mux type. */
};

mtb_stc_ubm_backplane_cfg_t ubm_backplane_configuration =
{
    .num_of_hfc = 4U,                       /* Number of the HFCs in the backplane */
    .num_of_dfc = 4U,                       /* Number of the DFCs in the backplane */
    .num_of_routes = 16U,                    /* Number of the routes in the backplane */
    .starting_slot = 0x00U,                 /* UBM starting slot */
    .overview_area = &overview_area,        /* Overview area configuration */
    .fru_config = &eepromConfig,            /* Storage configuration for the FRU */
    .ses_event_handler = NULL,              /* APP handler for the SES Array Device Slot Control Element */
    .bifurcate_port = false,                /* Indicates if the DFC port link width shall be bifurcated */
    .silicon_identity =
    {
        .pcie_vendor_id = 0xaa55U,          /* PCIe Vendor ID */
        .device_code = 0xface8d00U,         /* UBM Controller Device code */
        .fw_version_minor = 0x00U,          /* UBM Controller Image Version Minor */
        .fw_version_major = FW_VERSION,          /* UBM Controller Image Version Minor */
        .vendor_specific = 0x1234U,         /* UBM Controller vendor-specific data */
    },
    .backplane_info =
    {
        .backplane_type = 0x00U,            /* Backplane type */
        .backplane_number = 0x0AU,          /* Backplane number */
    },
    .capabilities =
    {
        .clock_routing = true,              /* Indicates availability of high speed differential clock routing (i.e., RefClk)
                                               from the Host Facing Connector to the Drive Facing Connector */
        .slot_power_control = true,         /* Indicates if the Drive Facing Connectors support Power Disable (i.e., PwrDIS signal) */
        .pcie_reset_control = true,       /* Indicates if PCIe Reset Control is supported */
        .dual_port = false,                  /* Indicates if Dual Port DFC connectors are routed */
        .i2c_reset_operation = MTB_UBM_CAP_2WIRE_RESET_OP_2W_FRU_CONTROLLER_MUX, /* Indicates the 2WIRE_RESET# signal support */
        .change_detect_interrupt = true,    /* Indicates if the CHANGE_DETECT# signal interrupt operation is supported */
        .dfc_change_count_supported = true, /* Indicates if the change count is maintained per individual DFC Status and Control Command Descriptor */
        .prsnt_reported = true,             /* Indicates if the PRSNT# signal is reported */
        .ifdet_reported = true,             /* Indicates if the IFDET# signal is reported */
        .ifdet2_reported = true,            /* Indicates if the IFDET2# signal is reported */
        .perst_override_supported = true,   /* Indicates if the UBM Controller supports the DFC
                                               PERST# Management Override field in the Features Command */
        .smb_reset_supported = true,        /* Indicates if the UBM Controller supports control over the DFC
                                               SMBRST# signals (e.g. See SFF-TA-1009) for all DFCs managed by the HFC */
    },
    .route_information =
    {
        {
            .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED,   /* UBM controller type */
			.ubm_ctrl_slave_addr = 0x60U,                  /* UBM Controller 2Wire slave address */
            .drive_connector_idx = 0x00U,                        /* Indicates the DFC identity */
            .drive_types_supported =                            /* Indicates which drive types are supported in the DFC */
            {
                .sff_ta_1001 = true,                            /* SFF-TA-1001 PCIe */
                .gen_z = false,                                 /* Gen-Z */
                .sas_sata = true,                               /* SAS/SATA */
                .quad_pcie = true,                              /* Quad PCIe */
                .dfc_empty = true,                              /* DFC Empty */
            },
            .drive_link_width = MTB_UBM_LINK_WIDTH_X1,          /* Indicates the number of lanes in the port */
            .port_type = MTB_UBM_PORT_TYPE_CONVERGED,          /* Indicates the connector port type which is routed from the DFC to the HFC */
            .domain = MTB_UBM_PORT_DOMAIN_PRIMARY,              /* Indicates if this route is describing the primary or secondary port of a DFC */
            .max_sata_line_rate = MTB_UBM_SATA_6GBS_RATE,   /* Max SATA Link Rate */
            .max_pcie_line_rate = MTB_UBM_PCIE_4_RATE,   /* Max PCIe Link Rate */
            .max_sas_line_rate = MTB_UBM_SAS_NO_RATE_LIMIT,     /* Max SAS Link Rate */
            .hfc_starting_phy_lane = 0x0U,                      /* Indicates the HFC starting lane */
            .hfc_identifier = 0x0U,                             /* Indicates the HFC identity */
            .slot_offset = 0x00U,                               /* Indicates the backplane slot offset for the DFC */
        },
        {
            .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED,   /* UBM controller type */
            .ubm_ctrl_slave_addr = 0x60U,                       /* UBM Controller 2Wire slave address */
            .drive_connector_idx = 0x01U,                        /* Indicates the DFC identity */
            .drive_types_supported =                            /* Indicates which drive types are supported in the DFC */
            {
                .sff_ta_1001 = true,                            /* SFF-TA-1001 PCIe */
                .gen_z = false,                                 /* Gen-Z */
                .sas_sata = true,                               /* SAS/SATA */
                .quad_pcie = true,                              /* Quad PCIe */
                .dfc_empty = true,                              /* DFC Empty */
            },
            .drive_link_width = MTB_UBM_LINK_WIDTH_X1,          /* Indicates the number of lanes in the port */
            .port_type = MTB_UBM_PORT_TYPE_CONVERGED,          /* Indicates the connector port type which is routed from the DFC to the HFC */
            .domain = MTB_UBM_PORT_DOMAIN_PRIMARY,              /* Indicates if this route is describing the primary or secondary port of a DFC */
            .max_sata_line_rate = MTB_UBM_SATA_NO_RATE_LIMIT,   /* Max SATA Link Rate */
            .max_pcie_line_rate = MTB_UBM_PCIE_NO_RATE_LIMIT,   /* Max PCIe Link Rate */
            .max_sas_line_rate = MTB_UBM_SAS_NO_RATE_LIMIT,     /* Max SAS Link Rate */
            .hfc_starting_phy_lane = 0x1U,                      /* Indicates the HFC starting lane */
            .hfc_identifier = 0x0U,                             /* Indicates the HFC identity */
            .slot_offset = 0x01U,                               /* Indicates the backplane slot offset for the DFC */
        },
		{
            .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED,   /* UBM controller type */
			.ubm_ctrl_slave_addr = 0x60U,                  /* UBM Controller 2Wire slave address */
            .drive_connector_idx = 0x02U,                        /* Indicates the DFC identity */
            .drive_types_supported =                            /* Indicates which drive types are supported in the DFC */
            {
                .sff_ta_1001 = true,                            /* SFF-TA-1001 PCIe */
                .gen_z = false,                                 /* Gen-Z */
                .sas_sata = true,                               /* SAS/SATA */
                .quad_pcie = true,                              /* Quad PCIe */
                .dfc_empty = true,                              /* DFC Empty */
            },
            .drive_link_width = MTB_UBM_LINK_WIDTH_X1,          /* Indicates the number of lanes in the port */
            .port_type = MTB_UBM_PORT_TYPE_CONVERGED,          /* Indicates the connector port type which is routed from the DFC to the HFC */
            .domain = MTB_UBM_PORT_DOMAIN_PRIMARY,              /* Indicates if this route is describing the primary or secondary port of a DFC */
            .max_sata_line_rate = MTB_UBM_SATA_6GBS_RATE,   /* Max SATA Link Rate */
            .max_pcie_line_rate = MTB_UBM_PCIE_4_RATE,   /* Max PCIe Link Rate */
            .max_sas_line_rate = MTB_UBM_SAS_NO_RATE_LIMIT,     /* Max SAS Link Rate */
            .hfc_starting_phy_lane = 0x2U,                      /* Indicates the HFC starting lane */
            .hfc_identifier = 0x0U,                             /* Indicates the HFC identity */
            .slot_offset = 0x02U,                               /* Indicates the backplane slot offset for the DFC */
        },
        {
            .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED,   /* UBM controller type */
            .ubm_ctrl_slave_addr = 0x60U,                       /* UBM Controller 2Wire slave address */
            .drive_connector_idx = 0x03U,                        /* Indicates the DFC identity */
            .drive_types_supported =                            /* Indicates which drive types are supported in the DFC */
            {
                .sff_ta_1001 = true,                            /* SFF-TA-1001 PCIe */
                .gen_z = false,                                 /* Gen-Z */
                .sas_sata = true,                               /* SAS/SATA */
                .quad_pcie = true,                              /* Quad PCIe */
                .dfc_empty = true,                              /* DFC Empty */
            },
            .drive_link_width = MTB_UBM_LINK_WIDTH_X1,          /* Indicates the number of lanes in the port */
            .port_type = MTB_UBM_PORT_TYPE_CONVERGED,          /* Indicates the connector port type which is routed from the DFC to the HFC */
            .domain = MTB_UBM_PORT_DOMAIN_PRIMARY,              /* Indicates if this route is describing the primary or secondary port of a DFC */
            .max_sata_line_rate = MTB_UBM_SATA_NO_RATE_LIMIT,   /* Max SATA Link Rate */
            .max_pcie_line_rate = MTB_UBM_PCIE_NO_RATE_LIMIT,   /* Max PCIe Link Rate */
            .max_sas_line_rate = MTB_UBM_SAS_NO_RATE_LIMIT,     /* Max SAS Link Rate */
            .hfc_starting_phy_lane = 0x3U,                      /* Indicates the HFC starting lane */
            .hfc_identifier = 0x0U,                             /* Indicates the HFC identity */
            .slot_offset = 0x03U,                               /* Indicates the backplane slot offset for the DFC */
        },

		 {
            .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED,   /* UBM controller type */
			.ubm_ctrl_slave_addr = 0x60U,                  /* UBM Controller 2Wire slave address */
            .drive_connector_idx = 0x00U,                        /* Indicates the DFC identity */
            .drive_types_supported =                            /* Indicates which drive types are supported in the DFC */
            {
                .sff_ta_1001 = true,                            /* SFF-TA-1001 PCIe */
                .gen_z = false,                                 /* Gen-Z */
                .sas_sata = true,                               /* SAS/SATA */
                .quad_pcie = true,                              /* Quad PCIe */
                .dfc_empty = true,                              /* DFC Empty */
            },
            .drive_link_width = MTB_UBM_LINK_WIDTH_X1,          /* Indicates the number of lanes in the port */
            .port_type = MTB_UBM_PORT_TYPE_CONVERGED,          /* Indicates the connector port type which is routed from the DFC to the HFC */
            .domain = MTB_UBM_PORT_DOMAIN_SECONDARY,              /* Indicates if this route is describing the primary or secondary port of a DFC */
            .max_sata_line_rate = MTB_UBM_SATA_6GBS_RATE,   /* Max SATA Link Rate */
            .max_pcie_line_rate = MTB_UBM_PCIE_4_RATE,   /* Max PCIe Link Rate */
            .max_sas_line_rate = MTB_UBM_SAS_NO_RATE_LIMIT,     /* Max SAS Link Rate */
            .hfc_starting_phy_lane = 0x0U,                      /* Indicates the HFC starting lane */
            .hfc_identifier = 0x1U,                             /* Indicates the HFC identity */
            .slot_offset = 0x04U,                               /* Indicates the backplane slot offset for the DFC */
        },
        {
            .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED,   /* UBM controller type */
            .ubm_ctrl_slave_addr = 0x60U,                       /* UBM Controller 2Wire slave address */
            .drive_connector_idx = 0x01U,                        /* Indicates the DFC identity */
            .drive_types_supported =                            /* Indicates which drive types are supported in the DFC */
            {
                .sff_ta_1001 = true,                            /* SFF-TA-1001 PCIe */
                .gen_z = false,                                 /* Gen-Z */
                .sas_sata = true,                               /* SAS/SATA */
                .quad_pcie = true,                              /* Quad PCIe */
                .dfc_empty = true,                              /* DFC Empty */
            },
            .drive_link_width = MTB_UBM_LINK_WIDTH_X1,          /* Indicates the number of lanes in the port */
            .port_type = MTB_UBM_PORT_TYPE_CONVERGED,          /* Indicates the connector port type which is routed from the DFC to the HFC */
            .domain = MTB_UBM_PORT_DOMAIN_SECONDARY,              /* Indicates if this route is describing the primary or secondary port of a DFC */
            .max_sata_line_rate = MTB_UBM_SATA_NO_RATE_LIMIT,   /* Max SATA Link Rate */
            .max_pcie_line_rate = MTB_UBM_PCIE_NO_RATE_LIMIT,   /* Max PCIe Link Rate */
            .max_sas_line_rate = MTB_UBM_SAS_NO_RATE_LIMIT,     /* Max SAS Link Rate */
            .hfc_starting_phy_lane = 0x1U,                      /* Indicates the HFC starting lane */
            .hfc_identifier = 0x1U,                             /* Indicates the HFC identity */
            .slot_offset = 0x05U,                               /* Indicates the backplane slot offset for the DFC */
        },
		{
            .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED,   /* UBM controller type */
			.ubm_ctrl_slave_addr = 0x60U,                  /* UBM Controller 2Wire slave address */
            .drive_connector_idx = 0x02U,                        /* Indicates the DFC identity */
            .drive_types_supported =                            /* Indicates which drive types are supported in the DFC */
            {
                .sff_ta_1001 = true,                            /* SFF-TA-1001 PCIe */
                .gen_z = false,                                 /* Gen-Z */
                .sas_sata = true,                               /* SAS/SATA */
                .quad_pcie = true,                              /* Quad PCIe */
                .dfc_empty = true,                              /* DFC Empty */
            },
            .drive_link_width = MTB_UBM_LINK_WIDTH_X1,          /* Indicates the number of lanes in the port */
            .port_type = MTB_UBM_PORT_TYPE_CONVERGED,          /* Indicates the connector port type which is routed from the DFC to the HFC */
            .domain = MTB_UBM_PORT_DOMAIN_SECONDARY,              /* Indicates if this route is describing the primary or secondary port of a DFC */
            .max_sata_line_rate = MTB_UBM_SATA_6GBS_RATE,   /* Max SATA Link Rate */
            .max_pcie_line_rate = MTB_UBM_PCIE_4_RATE,   /* Max PCIe Link Rate */
            .max_sas_line_rate = MTB_UBM_SAS_NO_RATE_LIMIT,     /* Max SAS Link Rate */
            .hfc_starting_phy_lane = 0x2U,                      /* Indicates the HFC starting lane */
            .hfc_identifier = 0x1U,                             /* Indicates the HFC identity */
            .slot_offset = 0x06U,                               /* Indicates the backplane slot offset for the DFC */
        },
        {
            .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED,   /* UBM controller type */
            .ubm_ctrl_slave_addr = 0x60U,                       /* UBM Controller 2Wire slave address */
            .drive_connector_idx = 0x03U,                        /* Indicates the DFC identity */
            .drive_types_supported =                            /* Indicates which drive types are supported in the DFC */
            {
                .sff_ta_1001 = true,                            /* SFF-TA-1001 PCIe */
                .gen_z = false,                                 /* Gen-Z */
                .sas_sata = true,                               /* SAS/SATA */
                .quad_pcie = true,                              /* Quad PCIe */
                .dfc_empty = true,                              /* DFC Empty */
            },
            .drive_link_width = MTB_UBM_LINK_WIDTH_X1,          /* Indicates the number of lanes in the port */
            .port_type = MTB_UBM_PORT_TYPE_CONVERGED,          /* Indicates the connector port type which is routed from the DFC to the HFC */
            .domain = MTB_UBM_PORT_DOMAIN_SECONDARY,              /* Indicates if this route is describing the primary or secondary port of a DFC */
            .max_sata_line_rate = MTB_UBM_SATA_NO_RATE_LIMIT,   /* Max SATA Link Rate */
            .max_pcie_line_rate = MTB_UBM_PCIE_NO_RATE_LIMIT,   /* Max PCIe Link Rate */
            .max_sas_line_rate = MTB_UBM_SAS_NO_RATE_LIMIT,     /* Max SAS Link Rate */
            .hfc_starting_phy_lane = 0x3U,                      /* Indicates the HFC starting lane */
            .hfc_identifier = 0x1U,                             /* Indicates the HFC identity */
            .slot_offset = 0x07U,                               /* Indicates the backplane slot offset for the DFC */
        },

		{
            .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED,   /* UBM controller type */
			.ubm_ctrl_slave_addr = 0x60U,                  /* UBM Controller 2Wire slave address */
            .drive_connector_idx = 0x04U,                        /* Indicates the DFC identity */
            .drive_types_supported =                            /* Indicates which drive types are supported in the DFC */
            {
                .sff_ta_1001 = true,                            /* SFF-TA-1001 PCIe */
                .gen_z = false,                                 /* Gen-Z */
                .sas_sata = true,                               /* SAS/SATA */
                .quad_pcie = true,                              /* Quad PCIe */
                .dfc_empty = true,                              /* DFC Empty */
            },
            .drive_link_width = MTB_UBM_LINK_WIDTH_X1,          /* Indicates the number of lanes in the port */
            .port_type = MTB_UBM_PORT_TYPE_SEGREGATED,          /* Indicates the connector port type which is routed from the DFC to the HFC */
            .domain = MTB_UBM_PORT_DOMAIN_PRIMARY,              /* Indicates if this route is describing the primary or secondary port of a DFC */
            .max_sata_line_rate = MTB_UBM_SATA_6GBS_RATE,   /* Max SATA Link Rate */
            .max_pcie_line_rate = MTB_UBM_PCIE_4_RATE,   /* Max PCIe Link Rate */
            .max_sas_line_rate = MTB_UBM_SAS_NO_RATE_LIMIT,     /* Max SAS Link Rate */
            .hfc_starting_phy_lane = 0x0U,                      /* Indicates the HFC starting lane */
            .hfc_identifier = 0x2U,                             /* Indicates the HFC identity */
            .slot_offset = 0x00U,                               /* Indicates the backplane slot offset for the DFC */
        },
        {
            .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED,   /* UBM controller type */
            .ubm_ctrl_slave_addr = 0x60U,                       /* UBM Controller 2Wire slave address */
            .drive_connector_idx = 0x05U,                        /* Indicates the DFC identity */
            .drive_types_supported =                            /* Indicates which drive types are supported in the DFC */
            {
                .sff_ta_1001 = true,                            /* SFF-TA-1001 PCIe */
                .gen_z = false,                                 /* Gen-Z */
                .sas_sata = true,                               /* SAS/SATA */
                .quad_pcie = true,                              /* Quad PCIe */
                .dfc_empty = true,                              /* DFC Empty */
            },
            .drive_link_width = MTB_UBM_LINK_WIDTH_X1,          /* Indicates the number of lanes in the port */
            .port_type = MTB_UBM_PORT_TYPE_SEGREGATED,          /* Indicates the connector port type which is routed from the DFC to the HFC */
            .domain = MTB_UBM_PORT_DOMAIN_PRIMARY,              /* Indicates if this route is describing the primary or secondary port of a DFC */
            .max_sata_line_rate = MTB_UBM_SATA_NO_RATE_LIMIT,   /* Max SATA Link Rate */
            .max_pcie_line_rate = MTB_UBM_PCIE_NO_RATE_LIMIT,   /* Max PCIe Link Rate */
            .max_sas_line_rate = MTB_UBM_SAS_NO_RATE_LIMIT,     /* Max SAS Link Rate */
            .hfc_starting_phy_lane = 0x1U,                      /* Indicates the HFC starting lane */
            .hfc_identifier = 0x2U,                             /* Indicates the HFC identity */
            .slot_offset = 0x01U,                               /* Indicates the backplane slot offset for the DFC */
        },
		        {
            .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED,   /* UBM controller type */
			.ubm_ctrl_slave_addr = 0x60U,                  /* UBM Controller 2Wire slave address */
            .drive_connector_idx = 0x06U,                        /* Indicates the DFC identity */
            .drive_types_supported =                            /* Indicates which drive types are supported in the DFC */
            {
                .sff_ta_1001 = true,                            /* SFF-TA-1001 PCIe */
                .gen_z = false,                                 /* Gen-Z */
                .sas_sata = true,                               /* SAS/SATA */
                .quad_pcie = true,                              /* Quad PCIe */
                .dfc_empty = true,                              /* DFC Empty */
            },
            .drive_link_width = MTB_UBM_LINK_WIDTH_X1,          /* Indicates the number of lanes in the port */
            .port_type = MTB_UBM_PORT_TYPE_SEGREGATED,          /* Indicates the connector port type which is routed from the DFC to the HFC */
            .domain = MTB_UBM_PORT_DOMAIN_PRIMARY,              /* Indicates if this route is describing the primary or secondary port of a DFC */
            .max_sata_line_rate = MTB_UBM_SATA_6GBS_RATE,   /* Max SATA Link Rate */
            .max_pcie_line_rate = MTB_UBM_PCIE_4_RATE,   /* Max PCIe Link Rate */
            .max_sas_line_rate = MTB_UBM_SAS_NO_RATE_LIMIT,     /* Max SAS Link Rate */
            .hfc_starting_phy_lane = 0x2U,                      /* Indicates the HFC starting lane */
            .hfc_identifier = 0x2U,                             /* Indicates the HFC identity */
            .slot_offset = 0x02U,                               /* Indicates the backplane slot offset for the DFC */
        },
        {
            .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED,   /* UBM controller type */
            .ubm_ctrl_slave_addr = 0x60U,                       /* UBM Controller 2Wire slave address */
            .drive_connector_idx = 0x07U,                        /* Indicates the DFC identity */
            .drive_types_supported =                            /* Indicates which drive types are supported in the DFC */
            {
                .sff_ta_1001 = true,                            /* SFF-TA-1001 PCIe */
                .gen_z = false,                                 /* Gen-Z */
                .sas_sata = true,                               /* SAS/SATA */
                .quad_pcie = true,                              /* Quad PCIe */
                .dfc_empty = true,                              /* DFC Empty */
            },
            .drive_link_width = MTB_UBM_LINK_WIDTH_X1,          /* Indicates the number of lanes in the port */
            .port_type = MTB_UBM_PORT_TYPE_SEGREGATED,          /* Indicates the connector port type which is routed from the DFC to the HFC */
            .domain = MTB_UBM_PORT_DOMAIN_PRIMARY,              /* Indicates if this route is describing the primary or secondary port of a DFC */
            .max_sata_line_rate = MTB_UBM_SATA_NO_RATE_LIMIT,   /* Max SATA Link Rate */
            .max_pcie_line_rate = MTB_UBM_PCIE_NO_RATE_LIMIT,   /* Max PCIe Link Rate */
            .max_sas_line_rate = MTB_UBM_SAS_NO_RATE_LIMIT,     /* Max SAS Link Rate */
            .hfc_starting_phy_lane = 0x3U,                      /* Indicates the HFC starting lane */
            .hfc_identifier = 0x2U,                             /* Indicates the HFC identity */
            .slot_offset = 0x03U,                               /* Indicates the backplane slot offset for the DFC */
        },

		        {
            .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED,   /* UBM controller type */
			.ubm_ctrl_slave_addr = 0x60U,                  /* UBM Controller 2Wire slave address */
            .drive_connector_idx = 0x04U,                        /* Indicates the DFC identity */
            .drive_types_supported =                            /* Indicates which drive types are supported in the DFC */
            {
                .sff_ta_1001 = true,                            /* SFF-TA-1001 PCIe */
                .gen_z = false,                                 /* Gen-Z */
                .sas_sata = true,                               /* SAS/SATA */
                .quad_pcie = true,                              /* Quad PCIe */
                .dfc_empty = true,                              /* DFC Empty */
            },
            .drive_link_width = MTB_UBM_LINK_WIDTH_X1,          /* Indicates the number of lanes in the port */
            .port_type = MTB_UBM_PORT_TYPE_SEGREGATED,          /* Indicates the connector port type which is routed from the DFC to the HFC */
            .domain = MTB_UBM_PORT_DOMAIN_SECONDARY,              /* Indicates if this route is describing the primary or secondary port of a DFC */
            .max_sata_line_rate = MTB_UBM_SATA_6GBS_RATE,   /* Max SATA Link Rate */
            .max_pcie_line_rate = MTB_UBM_PCIE_4_RATE,   /* Max PCIe Link Rate */
            .max_sas_line_rate = MTB_UBM_SAS_NO_RATE_LIMIT,     /* Max SAS Link Rate */
            .hfc_starting_phy_lane = 0x0U,                      /* Indicates the HFC starting lane */
            .hfc_identifier = 0x3U,                             /* Indicates the HFC identity */
            .slot_offset = 0x04U,                               /* Indicates the backplane slot offset for the DFC */
        },
        {
            .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED,   /* UBM controller type */
            .ubm_ctrl_slave_addr = 0x60U,                       /* UBM Controller 2Wire slave address */
            .drive_connector_idx = 0x05U,                        /* Indicates the DFC identity */
            .drive_types_supported =                            /* Indicates which drive types are supported in the DFC */
            {
                .sff_ta_1001 = true,                            /* SFF-TA-1001 PCIe */
                .gen_z = false,                                 /* Gen-Z */
                .sas_sata = true,                               /* SAS/SATA */
                .quad_pcie = true,                              /* Quad PCIe */
                .dfc_empty = true,                              /* DFC Empty */
            },
            .drive_link_width = MTB_UBM_LINK_WIDTH_X1,          /* Indicates the number of lanes in the port */
            .port_type = MTB_UBM_PORT_TYPE_SEGREGATED,          /* Indicates the connector port type which is routed from the DFC to the HFC */
            .domain = MTB_UBM_PORT_DOMAIN_SECONDARY,              /* Indicates if this route is describing the primary or secondary port of a DFC */
            .max_sata_line_rate = MTB_UBM_SATA_NO_RATE_LIMIT,   /* Max SATA Link Rate */
            .max_pcie_line_rate = MTB_UBM_PCIE_NO_RATE_LIMIT,   /* Max PCIe Link Rate */
            .max_sas_line_rate = MTB_UBM_SAS_NO_RATE_LIMIT,     /* Max SAS Link Rate */
            .hfc_starting_phy_lane = 0x1U,                      /* Indicates the HFC starting lane */
            .hfc_identifier = 0x3U,                             /* Indicates the HFC identity */
            .slot_offset = 0x05U,                               /* Indicates the backplane slot offset for the DFC */
        },
		        {
            .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED,   /* UBM controller type */
			.ubm_ctrl_slave_addr = 0x60U,                  /* UBM Controller 2Wire slave address */
            .drive_connector_idx = 0x06U,                        /* Indicates the DFC identity */
            .drive_types_supported =                            /* Indicates which drive types are supported in the DFC */
            {
                .sff_ta_1001 = true,                            /* SFF-TA-1001 PCIe */
                .gen_z = false,                                 /* Gen-Z */
                .sas_sata = true,                               /* SAS/SATA */
                .quad_pcie = true,                              /* Quad PCIe */
                .dfc_empty = true,                              /* DFC Empty */
            },
            .drive_link_width = MTB_UBM_LINK_WIDTH_X1,          /* Indicates the number of lanes in the port */
            .port_type = MTB_UBM_PORT_TYPE_SEGREGATED,          /* Indicates the connector port type which is routed from the DFC to the HFC */
            .domain = MTB_UBM_PORT_DOMAIN_SECONDARY,              /* Indicates if this route is describing the primary or secondary port of a DFC */
            .max_sata_line_rate = MTB_UBM_SATA_6GBS_RATE,   /* Max SATA Link Rate */
            .max_pcie_line_rate = MTB_UBM_PCIE_4_RATE,   /* Max PCIe Link Rate */
            .max_sas_line_rate = MTB_UBM_SAS_NO_RATE_LIMIT,     /* Max SAS Link Rate */
            .hfc_starting_phy_lane = 0x2U,                      /* Indicates the HFC starting lane */
            .hfc_identifier = 0x3U,                             /* Indicates the HFC identity */
            .slot_offset = 0x06U,                               /* Indicates the backplane slot offset for the DFC */
        },
        {
            .ubm_ctrl_type = MTB_UBM_CONTROLLER_SPEC_DEFINED,   /* UBM controller type */
            .ubm_ctrl_slave_addr = 0x60U,                       /* UBM Controller 2Wire slave address */
            .drive_connector_idx = 0x07U,                        /* Indicates the DFC identity */
            .drive_types_supported =                            /* Indicates which drive types are supported in the DFC */
            {
                .sff_ta_1001 = true,                            /* SFF-TA-1001 PCIe */
                .gen_z = false,                                 /* Gen-Z */
                .sas_sata = true,                               /* SAS/SATA */
                .quad_pcie = true,                              /* Quad PCIe */
                .dfc_empty = true,                              /* DFC Empty */
            },
            .drive_link_width = MTB_UBM_LINK_WIDTH_X1,          /* Indicates the number of lanes in the port */
            .port_type = MTB_UBM_PORT_TYPE_SEGREGATED,          /* Indicates the connector port type which is routed from the DFC to the HFC */
            .domain = MTB_UBM_PORT_DOMAIN_SECONDARY,              /* Indicates if this route is describing the primary or secondary port of a DFC */
            .max_sata_line_rate = MTB_UBM_SATA_NO_RATE_LIMIT,   /* Max SATA Link Rate */
            .max_pcie_line_rate = MTB_UBM_PCIE_NO_RATE_LIMIT,   /* Max PCIe Link Rate */
            .max_sas_line_rate = MTB_UBM_SAS_NO_RATE_LIMIT,     /* Max SAS Link Rate */
            .hfc_starting_phy_lane = 0x3U,                      /* Indicates the HFC starting lane */
            .hfc_identifier = 0x3U,                             /* Indicates the HFC identity */
            .slot_offset = 0x07U,                               /* Indicates the backplane slot offset for the DFC */
        },
    }
};

mtb_stc_ubm_backplane_control_signals_t ubm_backplane_control_signals =
{
    .dfc_io =
    {
    		{.ifdet = P3_5, .ifdet2 = P7_2, .prsnt = P0_5, .persta = P9_7, .perstb = P5_6, .pwrdis = P4_0,  .refclken = P5_5,  .dualporten = P3_3},
			{.ifdet = P2_5, .ifdet2 = P0_4, .prsnt = P6_4, .persta = P2_6, .perstb = P12_2,.pwrdis = P11_7, .refclken = P12_1, .dualporten = P3_0},
			{.ifdet = P1_5, .ifdet2 = P7_1, .prsnt = P6_3, .persta = P2_7, .perstb = P8_6, .pwrdis = P8_3,  .refclken = P8_5,  .dualporten = P1_3},
			{.ifdet = P9_5, .ifdet2 = P8_1, .prsnt = P6_5, .persta = P9_6, .perstb = P8_0, .pwrdis = P7_7,  .refclken = P7_6,  .dualporten = P0_0},
    },
    .hfc_io =
    {
    		{.sda = P0_3, .scl = P0_2, .i2c_reset = P0_1, .change_detect = P13_3, .bp_type  = NC, .perst = P3_2 },
    		{.sda = P2_1, .scl = P2_0, .i2c_reset = P2_3, .change_detect = P13_1, .bp_type  = NC, .perst = P2_2 },
    		{.sda = P6_1, .scl = P6_0, .i2c_reset = P6_2, .change_detect = P13_2, .bp_type  = NC, .perst = P1_2 },
    		{.sda = P9_1, .scl = P9_0, .i2c_reset = P9_3, .change_detect = P13_0, .bp_type  = NC, .perst = P9_2 },
    }
};

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

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    /* Enable global interrupts */
    __enable_irq();
    

    CY_ASSERT(result == CY_RSLT_SUCCESS);
    
    (void) result; /* To avoid compiler warning in release build */

    mtb_en_ubm_status_t status = mtb_ubm_init(&ubm_backplane_configuration, &ubm_backplane_control_signals, &ubm_context);

    if (status != MTB_UBM_STATUS_SUCCESS)
    {
    	CY_ASSERT(0);
    }


    for (;;)
    {

    	/* User application */
    }

    return 0;
}

/* [] END OF FILE */

