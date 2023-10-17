# PSoC&trade; 61 UBM controller application

*Disclaimer: This is a community code example (CCE) released for the benefit of the community users. These projects have only been tested for the listed BSPs, tools versions, and toolchains documented in this readme. They are intended to demonstrate how a solution / concept / use-case can be achieved on a particular device. For official code examples, please click [here](https://github.com/Infineon/Code-Examples-for-ModusToolbox-Software).*

This is a basic code example to showcase the Universal Backplane Management (UBM) middleware features. The code example explains how to configure the UBM hardware and illustrates the firmware update using the [MCUboot](https://github.com/mcu-tools/mcuboot/releases/tag/v1.8.3-cypress) library.

The UBM middleware provides the implementation of UBM. PSoC&trade; 61 is used as UBM controller in the UBM hardware. Because there is no BSP available for PSoC&trade; 61 in ModusToolbox&trade;, you need to either use the custom BSP (`TARGET_ELROND_BACKPLANE`) shipped with this code example or create a custom BSP for PSoC&trade; 61 by following the instructions in [Creating a custom BSP for PSoC&trade; 61](#creating-a-custom-bsp-for-psoctrade-61).

## Requirements

- [ModusToolbox&trade;](https://www.infineon.com/modustoolbox) v3.1 or later (tested with v3.1)
- Board support package (BSP) minimum required version: 4.0.0
- Programming language: C



## Supported toolchains (make variable 'TOOLCHAIN')

- GNU Arm&reg; Embedded Compiler v10.3.1 (`GCC_ARM`) – Default value of `TOOLCHAIN`



## Supported kit

This code example uses a customized UBM hardware setup with PSoC&trade; 61 MCU in it. A custom BSP (TARGET_ELROND_BACKPLANE) for the UBM hardware is shipped with the code example. You need to import this BSP while creating the application.


Use any one of the [PSoC&trade; 6 development kits](https://documentation.infineon.com/html/psoc6/hsg1651214227031.html) by replacing the MCU on the board with PSoC&trade; 61 MCU.


## Hardware setup

This code example uses a custom UBM hardware setup. The UBM hardware setup has a UBM midplane and UBM backplane, which contain the UBM controller. [PSoC&trade; 61](https://www.infineon.com/cms/en/product/microcontroller/32-bit-psoc-arm-cortex-microcontroller/psoc-6-32-bit-arm-cortex-m4-mcu/cy8c6136bzi-f14/) is used as the UBM controller.


## Software setup

This code example requires no additional software or tools.

The UBM requires the following:

 - [emeeprom](https://github.com/Infineon/emeeprom) v2.20.0.

 - [MCUboot](https://github.com/mcu-tools/mcuboot) v1.8.3.

See the [UBM middleware release notes](./RELEASE.md) for release-specific information.



## Quick start

In API Reference, see the [Quick Start Guide](https://infineon.github.io/ubm/html/index.html) section for step-by-step instruction on how to enable the UBM middleware.


## Using the code example

> **Note:** It is recommended to use a new workspace.

Create the project and open it using one of the following:

> **Note:** It is recommended to use the custom BSP(TARGET_ELROND_BACKPLANE) provided with the code example while creating the application.

<details><summary><b>In Eclipse IDE for ModusToolbox&trade; software</b></summary>

1. Clone the PSoC&trade; 61 UBM controller application to the workspace. For example, by using "Git Bash" the following command clones the application.  

   ```
   git clone  https://github.com/Infineon/cce-mtb-psoc6-ubm-controller
   ```
2. Open the Eclipse IDE for Modustoolbox and click the **New Application** link in the **Quick Panel** (or, use **File** > **New** > **ModusToolbox&trade; Application**). This launches the [Project Creator](https://www.infineon.com/ModusToolboxProjectCreator) tool.

3. Pick a custom BSP shipped with the example by selecting **Project Creator - Choose Board Support Package (BSP)** dialog (i.e., after opening Project Creator, click **Browse** > **Directory** and select the custom BSP (TARGET_ELROND_BACKPLANE)).

   > **Note:** Custom BSP TARGET_ELROND_BACKPLANE is shipped with the PSoC&trade; 61 UBM controller application. You can find the TARGET_ELROND_BACKPLANE custom BSP in the PSoC&trade; 61 UBM controller application directory.

   > **Note:** In this step, it is recomended to use the BSP(TARGET_ELROND_BACKPLANE) shipped with the application because there are no official PSoC&trade; 61 kit. You need to either use the custom BSP (TARGET_ELROND_BACKPLANE) shipped or build your own BSP; later sections describe the required steps.

4. In the **Project Creator - Select Application** dialog, click **Browse for Application** and select the PSoC&trade; 61 UBM controller application from the clonned directory (as mentioned in step 1) to import the applicaton.

5. (Optional) Change the suggested **New Application Name**.

6. The **Application(s) Root Path** defaults to the Eclipse workspace, which is the desired location for the application. To store the application in a different location, you can change the *Application(s) Root Path* value. Applications that share libraries should be in the same root path.

7. Click **Create** to complete the application creation process.

For more details, see the [Eclipse IDE for ModusToolbox&trade; user guide](https://www.infineon.com/MTBEclipseIDEUserGuide) (locally available at *{ModusToolbox&trade; install directory}/ide_{version}/docs/mt_ide_user_guide.pdf*).

</details>

<details><summary><b>In command-line interface (CLI)</b></summary>

ModusToolbox&trade; provides the Project Creator as both a GUI tool and the command line tool, "project-creator-cli". The CLI tool can be used to create applications from a CLI terminal or from within batch files or shell scripts. This tool is available in the *{ModusToolbox&trade; install directory}/tools_{version}/project-creator/* directory.

Use a CLI terminal to invoke the 'project-creator-cli' tool. On Windows, use the command line "modus-shell" program provided in the ModusToolbox&trade; installation instead of a standard Windows command-line application. This shell provides access to all ModusToolbox&trade; tools. You can access it by typing "modus-shell" in the search box in the Windows menu. In Linux and macOS, you can use any terminal application.

The 'project-creator-cli' tool has the following arguments:

Argument | Description | Required/optional
---------|-------------|-----------
`--board-id` | Defined in the <id> field of the [BSP](https://github.com/infineon?q=bsp-manifest&type=&language=&sort=) manifest | Required
`--app-id`   | Defined in the <id> field of the [CE](https://github.com/infineon?q=ce-manifest&type=&language=&sort=) manifest | Required
`--target-dir`| Specify the directory in which the application is to be created if you prefer not to use the default current working directory | Optional
`--user-app-name`| Specify the name of the application if you prefer to have a name other than the example's default name | Optional

<br>

The following example clones the "[PSoC&trade; 61 UBM controller application](https://github.com/Infineon/cce-mtb-psoc6-ubm-controller)" application with the desired name "cce-mtb-psoc6-ubm-controller" configured for the *CY8CKIT-062S2-43012* BSP into the specified working directory, *C:/mtb_projects*:

   ```
   project-creator-cli --board-id CY8CKIT-062S2-43012 --app-id cce-mtb-psoc6-ubm-controller --user-app-name cce-mtb-psoc6-ubm-controller --target-dir "C:/mtb_projects"
   ```
> **Note:** To work with custom BSP (TARGET_ELROND_BACKPLANE), use the [Library Manager](https://www.infineon.com/ModusToolboxLibraryManager) to choose the custom BSP.

> **Note:** The project-creator-cli tool uses the `git clone` and `make getlibs` commands to fetch the repository and import the required libraries. For details, see the "Project creator tools" section of the [ModusToolbox&trade; tools package user guide](https://www.infineon.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mtb_user_guide.pdf*).

</details>

<details><summary><b>In third-party IDEs</b></summary>

Use one of the following options:

- **Use the standalone [Project Creator](https://www.infineon.com/ModusToolboxProjectCreator) tool:**

   1. Clone the PSoC&trade; 61 UBM controller application. For example, by using "Git Bash", the following command clones the application.
   ```
   git clone  https://github.com/Infineon/cce-mtb-psoc6-ubm-controller
   ```
   2. Launch Project Creator from the Windows Start menu or from *{ModusToolbox&trade; install directory}/tools_{version}/project-creator/project-creator.exe*.

   3. After opening the project creator to use our custom BSP (TARGET_ELROND_BACKPLANE), click **Browse for BSP** > **Directory** and select the custom BSP (TARGET_ELROND_BACKPLANE)  and click **Next**.

     > **Note** Custom BSP TARGET_ELROND_BACKPLANE is shipped with the PSoC&trade; 61 UBM controller application. User can find the TARGET_ELROND_BACKPLANE custom BSP inside the PSoC&trade; 61 UBM controller application directory.

   4. In the **Project Creator - Select Application** dialog, click **Browse for Application** and select the PSoC&trade; 61 UBM controller application from the clonned directory to import the applicaton.

   5. In the **Select Application** screen, select the appropriate IDE from the **Target IDE** drop-down menu.

   6. Click **Create** and follow the instructions printed in the bottom pane to import or open the exported project in the respective IDE.

<br>

- **Use command-line interface (CLI):**

   1. Follow the instructions from the **In command-line interface (CLI)** section to create the application, and then import the libraries using the `make getlibs` command.

   2. Export the application to a supported IDE using the `make <ide>` command.

   3. Follow the instructions displayed in the terminal to create or import the application as an IDE project.

For a list of supported IDEs and more details, see the "Exporting to IDEs" section of the [ModusToolbox&trade; user guide](https://www.infineon.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mtb_user_guide.pdf*).

</details>



## Creating a custom BSP for PSoC&trade; 61

ModusToolbox&trade; does not have a BSP for PSoC&trade; 61 that is used as the UBM controller. Therefore, a custom BSP (TARGET_ELROND_BACKPLANE) is shipped with the code example. You can either import this BSP or can create a custom BSP for PSoC&trade; 61 using either of the two methods detailed below.

### Method 1: Using a generic BSP and BSP Assistant

See [BSP Assistant user guide](https://www.infineon.com/dgdl/infineon-ModusToolbox_BSP_Assistant_1.0_User_Guide-UserManual-v02_00-EN.pdf?fileId=8ac78c8c8386267f0183a972f45c59af) for detailed instructions on creating a new BSP for PSoC&trade; 61 from a generic BSP.


### Method 2: Base on *CY8CKIT-062S2-43012* BSP

The BSP for PSoC&trade; 61 can be created from the *CY8CKIT-062S2-43012* kit with some changes made for PSoC&trade; 61. Follow the steps detailed in the [Custom BSP for PSoC&trade; 61](https://github.com/Infineon/cce-mtb-psoc61-mcuboot-bootloader#custom-bsp-for-psoc61) section to create a BSP for PSoC&trade; 61.

> **Note:** Ensure that the pins used in the backplane control signals are not reused for any other resources.

> **Note:** You can use CLI version of the BSP Assistant tool for these configurations. See the [BSP Assistant user guide](https://www.infineon.com/dgdl/infineon-ModusToolbox_BSP_Assistant_1.0_User_Guide-UserManual-v02_00-EN.pdf?fileId=8ac78c8c8386267f0183a972f45c59af).



## Operation

You should use a proper UBM hardware setup to run this PSoC&trade; 61 UBM controller application code example.

This document expects you to be familiar with MCUboot and its concepts. See [MCUboot documentation](https://github.com/mcu-tools/mcuboot) to learn more.

This example bundles two applications: the ubm_bootloader and the ubm_controller run by CM4. You need to build and program the applications in the following order. Do not start building the applications yet: follow the [Step-by-step instructions](#step-by-step-instructions).


### Step-by-step instructions

1. Connect the UBM controller to your PC using the provided USB cable through the MiniProg4 USB connector.

2. Install the dependent modules for the [imgtool](https://pypi.org/project/imgtool/) Python module for image signing and key management.

   MCUboot already includes this module, but not the dependent modules. Do the following:

   1. Open a CLI terminal and navigate to the *\<mtb_shared>/mcuboot/\<tag>/scripts* directory.
   
   2. Run the following command to ensure that the required modules are installed or already present ("Requirement already satisfied:" is printed).

      ```
      python -m pip install -r requirements.txt
      ```
3. Ensure that the following configurations are made before building the application:

   1. Open *ubm_bootloader/shared_config.mk* and ensure that `FLASH_MAP` is set to `psoc62_swap_single_custom.json`.

   2. Open *ubm_bootloader/flashmap/psoc62_swap_single_custom.json* and ensure that `application_1` and `upgrade_size` are set to `0x20000`.

   3. Open *ubm_controller/imports/ubm/mtb_ubm_config.h* and ensure that the macros `MTB_UBM_UPGRADE_AREA_START_ADDRESS` and `MTB_UBM_UPGRADE_AREA_SIZE` are set as follows:

      ```
      #define MTB_UBM_UPGRADE_AREA_START_ADDRESS       (0x10018000U)
      #define MTB_UBM_UPGRADE_AREA_SIZE                (0x20000U)
      ```
   4. Ensure that the PSoC&trade; 61 pins used to map DFC and HFC (see the `ubm_backplane_control_signals` structure in *ubm_controller/main.c*) are not reused anywhere else in the application.

4. Build the ubm_bootloader application.

   <details><summary><b>Using Eclipse IDE for ModusToolbox&trade;</b></summary>

      1. Select the 'bootloader' application in the Project Explorer.

      2. In the **Quick Panel**, scroll down, and click **Build Application**.

   </details>

    <details><summary><b>Using CLI</b></summary>

     From the terminal, navigate to *\<application>/ubm_bootloader* and execute the `make build` command to build application using the default toolchain to the default target. The default toolchain and target are specified in the application's Makefile but you can override those values manually:

      ```
      make build TOOLCHAIN=<toolchain>
      ```

      Example:
      ```
      make build TOOLCHAIN=GCC_ARM
      ```
   </details>

5. Program the ubm_bootloader application using [Cypress Programmer](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.cypressprogrammer).
6. Build the UBM_controller application in BOOT mode.

   <details><summary><b>Using Eclipse IDE for ModusToolbox&trade;</b></summary>

      1. Select the 'bootloader' application in the Project Explorer.

      2. In the **Quick Panel**, scroll down, and click **Build Application**.

      </details>

   <details><summary><b>Using CLI</b></summary>

     From the terminal, navigate to *\<application>/ubm_controller* and execute the `make build` command to build application using the default toolchain to the default target. The default toolchain and target are specified in the application's Makefile but you can override those values manually:

      ```
      make build TOOLCHAIN=<toolchain>
      ```

      Example:
      ```
      make build TOOLCHAIN=GCC_ARM
      ```
   </details>

7. Progam the ubm_controller application using the **Cypress Programmer**  

8. After programming, the ubm_bootloader starts automatically and lets CM4 run the UBM_controller application.

9. Build (Do **NOT** program) the UBM_controller application in UPGRADE mode.

   <details><summary><b>Using Eclipse IDE for ModusToolbox&trade;</b></summary>

      1. Select the 'UBM_controller' application in Project Explorer.

      2. Edit the Makefile and update the value of the `IMG_TYPE` variable to `UPGRADE`.

      3. In the **Quick Panel**, scroll down, and click **Build Application**.

   </details>

10. Program the UPGRADE image using the Scrutiny tool. See [Firmware update using the Scrutiny tool](#firmware-update-using-the-scrutiny-tool).

  > **Note:** The Scrutiny tool is owned by Broadcom Corporation. You must adhere to the Broadcom Corporation policies to obtain and use the tool. Currently commands that will upgrade the controller image are not supported by the current version of the Scrutiny tool.

11. UBM middleware resets the silicon after downloading the new application (upgrade image), only if the "VERIFY IMAGE" returns a success. This will start the bootloader automatically, upgrades the image by copying the image from the secondary slot into the primary slot, and lets CM4 run the UBM_controller application.

> **Note:** Ensure that the Scrutiny tool takes care of the CRC signing and image verification. 

> **Note:** You can build the combined image for the bootloader and UBM_controller applications using the `make build` CLI command in the *\<application>* directory but during the linking stage, there might be an error stating multiple definition of symbols for UBM_controller for `BOOT` and `UPGRADE` image. Currently the solution to the problem has been addressed in the following code section of the *\<application>/UBM_controller/Makefile* which ignores the build artifacts of the other `IMG_TYPE`. For example, if `BOOT` is selected as `IMG_TYPE`, the *\<application>/UBM_controller/build/UPGRADE/* build directory artifacts will be ignored during the compilation and linking of the `BOOT` image.

```
ifeq ($(IMG_TYPE), BOOT)
CY_IGNORE+=build/UPGRADE
else
ifeq ($(IMG_TYPE), UPGRADE)
CY_IGNORE+=build/BOOT
endif
endif
```

For programming the individual builds of the bootloader app and UBM_controller app, use the `make program_proj` CLI command as mentioned in the above steps.


## Debugging

Debugging pins are disabled in the BSP because debugging pins are used for the UBM middleware initialization in the UBM hardware setup. Therefore, debugging is disabled in the PSoC&trade; 61 UBM controller application.


## Design and implementation

As explained at the beginning, this code example bundles two applications: the bootloader application and the PSoC&trade; 61 UBM controller application. The PSoC&trade; 61 UBM controller application is directly programmed into the flash to quickly evaluate the MCUboot operation.

This code example uses ModusToolbox&trade; software resources such as BSPs and PSoC&trade; 61 MCU to provide a rich development experience that aligns well with other code examples based on ModusToolbox&trade; software.

Design and implementation are similar to that of the [mtb-example-psoc6-mcuboot-basic](https://github.com/Infineon/mtb-example-psoc6-mcuboot-basic) example except that both bootloader and applications are designed to run from CM4 CPU and tested on only PSoC&trade; 61 device.

The PSoC&trade; 61 UBM controller application is updated to run the UBM middleware; this application will initialize the UBM middleware with UBM backplane control signals and UBM backplane configurations.
The `mtb_ubm_init()` function call will initialize the UBM middleware.

Parameters for the `mtb_ubm_init()` function:

Parameter | Description
----------|------------
`mtb_stc_ubm_backplane_cfg_t` <br> ubm_backplane_configuration | These parameters have the configurational values required to set up the UBM middleware. <br> Holds the configuration value for the following: <br> 1. `num_of_hfc` - Total number of HFC to configured <br> <br> 2. `num_of_dfc` - Total number of DFC to configured <br> 3. `num_of_routes` - Total number of routes to be configured  <br> 4. `starting_slot` - Starting slot address <br> 5. `overview_area` – Overview area configuration that holds the UBM FRU overview area configuration structure <br>  6. `fru_config` – Storage configuration for the FRU <br> 7. `silicon_identity` – Have the UBM firmware version, device code, pcie_vendor ID, vendor specific <br> 8. `ses_event_handler` - APP handler for the SES Array Device Slot Control Element <br> 9. `bifurcate_port` - Indicates if the DFC port link width shall be bifurcated <br>  10. `backplane_info` – Holds the details of backplane type & backplane numbers <br> 11. `capabilities` – Holds the I/O signal configurations <br>  12. `route_information` – Have the all routing information of all the routes <br> 
`mtb_stc_ubm_backplane_control_signals_t` <br> ubm_backplane_control_signals | These parameters contain the GPIO definition for HFC and DFC I/O signals. The number of elements of this configurational array should match the number of HFCs and the number of DFCs configured. These signal definitions are a combination of SFF-TA-1005, SFF-8639, SFF-TA-1001, SFF-TA-1009, SAS, and SATA standards. Change the pin settings to match your specifications.
`mtb_stc_ubm_context_t` <br> ubm_context | Contains internal middleware data structures; not intended to be changed by the application.

`mtb_ubm_init()` function returns meaningful error codes which are detailed in the [UBM Middleware linrary](https://infineon.github.io/ubm/html/group__group__ubm__enums.html#ga7edd9650e9144861643adbf7aefbcc48).

## Firmware update using the Scrutiny tool

The Scrutiny tool will make the application to download the updated image and write the image into the secondary slot that is available in flash memory. When the UBM initialization is successful, the host will communicate with the UBM controller by I2C (the UBM controller as the slave and the host as the master); the host can send UBM controller commands to the UBM controller using the Scrutiny tool.

### Sequence of Scrutiny commands

The following sequence of commands is used in the Scrutiny tool to download the updated image to the UBM controller.

Sl. No. | Command | Description | Code
--------|---------|-------------|-----
1 | **Enter Programmable mode** | Unlocks the UBM controller for image update mode. This command internally calls the `handle_enter_programmable_update_mode()` handler function. | `0x20`
2 | **Programmable Mode Data Transfer** | Defines the subcommands that are used to update the UBM controller image | `0x21`
3 | **GET NON VOLATILE GEOMETRY** | Returns the non-volatile storage structure and size of programmable segments |  `0x01`
4 |**ERASE**| Erases a segment of the non-volatile storage specified by the sector number and sector index | `0x02`
5 |**ERASE STATUS** |Verifies the status of the `ERASE` command | `0x03`
6 |**PROGRAM** | Writes the UBM controller image to a segment of non-volatile storage in the UBM controller. Non-volatile storage segment is specified by the sector number and sector index. | `0x04`
7 |**PROGRAM STATUS** | Verifies the status of the `PROGRAM` command | `0x05`
8 |**VERIFY** |Verifies a segment of non-volatile storage by specifying the sector number and sector index | `0x06`
9 |**VERIFY STATUS** | Returns the status of the last `VERIFY` command and the checksum of the non-volatile storage segment specified in the last `VERIFY` command | `0x07`
10 |**VERIFY IMAGE** | Verifies an image in the non-volatile storage. A specific image can be verified by giving the image number. | `0x08`
11 |**VERIFY IMAGE STATUS**| Returns the validity of an image based on the image number specified in the last `VERIFY IMAGE` command | `0x09`
12 |**SET ACTIVE IMAGE** |Sets a specified image to use when the UBM controller exits programmable update mode | `0x0A`
13 | **ACTIVE IMAGE STATUS** | Returns the status of the `SET ACTIVE IMAGE` command | `0x0b`
14| **Exit Programmable Update Mode** | Makes the UBM controller exit programmable update mode | `0x22`

<br>

Once the valid image is present in the non-volatile memory, MCUboot will automatically take care of switching the application to the upgraded UBM controller image.


## More information

For more information, see the following documents:

- [UBM middleware release notes](./RELEASE.md)

- [SFF-TA-1005](https://members.snia.org/document/dl/27167)

- [UBM middleware API reference manual](https://infineon.github.io/ubm/html/index.html)

- [ModusToolbox&trade; software environment, quick start guide, documentation, and videos](https://www.infineon.com/modustoolbox)

- [Infineon Technologies web site](https://www.infineon.com)

- [Code examples for ModusToolbox&trade; software](https://github.com/Infineon/Code-Examples-for-ModusToolbox-Software)

- [ModusToolbox&trade; Device Configurator tool guide](https://www.infineon.com/ModusToolboxDeviceConfig)

- [PSoC&trade; 6 MCU datasheets](https://www.infineon.com/cms/en/search.html#!view=downloads&term=psoc6&doc_group=Data%20Sheet)

- [PSoC&trade; 6 MCU technical reference manuals](https://www.infineon.com/cms/en/search.html#!view=downloads&term=psoc6&doc_group=Additional%20Technical%20Information)


## Related resources

Resources  | Links
-----------|----------------------------------
Application notes  | [AN228571](https://www.infineon.com/AN228571) – Getting started with PSoC&trade; 6 MCU on ModusToolbox&trade; <br>  [AN215656](https://www.infineon.com/AN215656) – PSoC&trade; 6 MCU: Dual-CPU system design <br> [AN79953](https://www.infineon.com/AN79953) – Getting started with PSoC&trade; 4 <br>  [AN85951](https://www.infineon.com/AN85951) – PSoC&trade; 4 and PSoC&trade; 6 MCU CAPSENSE&trade; design guide
Code examples  | [Using ModusToolbox&trade;](https://github.com/Infineon/Code-Examples-for-ModusToolbox-Software) on GitHub <br> [Using PSoC&trade; Creator](https://www.infineon.com/cms/en/design-support/software/code-examples/psoc-3-4-5-code-examples-for-psoc-creator)
Device documentation | [PSoC&trade; 6 MCU datasheets](https://documentation.infineon.com/html/psoc6/bnm1651211483724.html) <br> [PSoC&trade; 6 technical reference manuals](https://documentation.infineon.com/html/psoc6/zrs1651212645947.html)<br> [PSoC&trade; 4 datasheets](https://www.infineon.com/cms/en/search.html?intc=searchkwr-return#!view=downloads&term=psoc%204&doc_group=Data%20Sheet) <br>[PSoC&trade; 4 technical reference manuals](https://www.infineon.com/cms/en/search.html#!term=psoc%204%20technical%20reference%20manual&view=all)
Development kits | Select your kits from the [Evaluation board finder](https://www.infineon.com/cms/en/design-support/finder-selection-tools/product-finder/evaluation-board).
Libraries on GitHub  | [mtb-pdl-cat1](https://github.com/Infineon/mtb-pdl-cat1) – PSoC&trade; 6 Peripheral Driver Library (PDL)  <br> [mtb-hal-cat1](https://github.com/Infineon/mtb-hal-cat1) – Hardware Abstraction Layer (HAL) library <br> [retarget-io](https://github.com/Infineon/retarget-io) – Utility library to retarget STDIO messages to a UART port <br>  [mtb-pdl-cat2](https://github.com/Infineon/mtb-pdl-cat2) – PSoC&trade; 4 Peripheral Driver Library (PDL) <br>  [mtb-hal-cat2](https://github.com/Infineon/mtb-hal-cat2) – Hardware Abstraction Layer (HAL) library
Middleware on GitHub  | [capsense](https://github.com/Infineon/capsense) – CAPSENSE&trade; library and documents <br> [psoc6-middleware](https://github.com/Infineon/modustoolbox-software#psoc-6-middleware-libraries) – Links to all PSoC&trade; 6 MCU middleware
Tools  | [ModusToolbox&trade;](https://www.infineon.com/modustoolbox) – ModusToolbox&trade; software is a collection of easy-to-use libraries and tools enabling rapid development with Infineon MCUs for applications ranging from wireless and cloud-connected systems, edge AI/ML, embedded sense and control, to wired USB connectivity using PSoC&trade; Industrial/IoT MCUs, AIROC&trade; Wi-Fi and Bluetooth&reg; connectivity devices. <br> [PSoC&trade; Creator](https://www.infineon.com/cms/en/design-support/tools/sdk/psoc-software/psoc-creator/) – IDE for PSoC&trade; and FM0+ MCU development.

<br />

## Other resources

Infineon provides a wealth of data at www.infineon.com to help you select the right device, and quickly and effectively integrate it into your design.

For PSoC&trade; 6 MCU devices, see [How to design with PSoC&trade; 6 MCU – KBA223067](https://community.infineon.com/docs/DOC-14644) in the Infineon Developer community.

## Document history

Document title: *CCE238145* – *PSoC&trade; 61 UBM controller application*   

 Version | Description of change
 ------- | ---------------------
 1.0.0   | New code example 
 2.0.0   | Updated to support UBM Middleware v1.0.0 <br />



All referenced product or service names and trademarks are the property of their respective owners.

---------------------------------------------------------

© Cypress Semiconductor Corporation, 2023. This document is the property of Cypress Semiconductor Corporation, an Infineon Technologies company, and its affiliates ("Cypress").  This document, including any software or firmware included or referenced in this document ("Software"), is owned by Cypress under the intellectual property laws and treaties of the United States and other countries worldwide.  Cypress reserves all rights under such laws and treaties and does not, except as specifically stated in this paragraph, grant any license under its patents, copyrights, trademarks, or other intellectual property rights.  If the Software is not accompanied by a license agreement and you do not otherwise have a written agreement with Cypress governing the use of the Software, then Cypress hereby grants you a personal, non-exclusive, nontransferable license (without the right to sublicense) (1) under its copyright rights in the Software (a) for Software provided in source code form, to modify and reproduce the Software solely for use with Cypress hardware products, only internally within your organization, and (b) to distribute the Software in binary code form externally to end users (either directly or indirectly through resellers and distributors), solely for use on Cypress hardware product units, and (2) under those claims of Cypress’s patents that are infringed by the Software (as provided by Cypress, unmodified) to make, use, distribute, and import the Software solely for use with Cypress hardware products.  Any other use, reproduction, modification, translation, or compilation of the Software is prohibited.
<br>
TO THE EXTENT PERMITTED BY APPLICABLE LAW, CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS DOCUMENT OR ANY SOFTWARE OR ACCOMPANYING HARDWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  No computing device can be absolutely secure.  Therefore, despite security measures implemented in Cypress hardware or software products, Cypress shall have no liability arising out of any security breach, such as unauthorized access to or use of a Cypress product. CYPRESS DOES NOT REPRESENT, WARRANT, OR GUARANTEE THAT CYPRESS PRODUCTS, OR SYSTEMS CREATED USING CYPRESS PRODUCTS, WILL BE FREE FROM CORRUPTION, ATTACK, VIRUSES, INTERFERENCE, HACKING, DATA LOSS OR THEFT, OR OTHER SECURITY INTRUSION (collectively, "Security Breach").  Cypress disclaims any liability relating to any Security Breach, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any Security Breach.  In addition, the products described in these materials may contain design defects or errors known as errata which may cause the product to deviate from published specifications. To the extent permitted by applicable law, Cypress reserves the right to make changes to this document without further notice. Cypress does not assume any liability arising out of the application or use of any product or circuit described in this document. Any information provided in this document, including any sample design information or programming code, is provided only for reference purposes.  It is the responsibility of the user of this document to properly design, program, and test the functionality and safety of any application made of this information and any resulting product.  "High-Risk Device" means any device or system whose failure could cause personal injury, death, or property damage.  Examples of High-Risk Devices are weapons, nuclear installations, surgical implants, and other medical devices.  "Critical Component" means any component of a High-Risk Device whose failure to perform can be reasonably expected to cause, directly or indirectly, the failure of the High-Risk Device, or to affect its safety or effectiveness.  Cypress is not liable, in whole or in part, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any use of a Cypress product as a Critical Component in a High-Risk Device. You shall indemnify and hold Cypress, including its affiliates, and its directors, officers, employees, agents, distributors, and assigns harmless from and against all claims, costs, damages, and expenses, arising out of any claim, including claims for product liability, personal injury or death, or property damage arising from any use of a Cypress product as a Critical Component in a High-Risk Device. Cypress products are not intended or authorized for use as a Critical Component in any High-Risk Device except to the limited extent that (i) Cypress's published data sheet for the product explicitly states Cypress has qualified the product for use in a specific High-Risk Device, or (ii) Cypress has given you advance written authorization to use the product as a Critical Component in the specific High-Risk Device and you have signed a separate indemnification agreement.
<br>
Cypress, the Cypress logo, and combinations thereof, ModusToolbox, PSoC, CAPSENSE, EZ-USB, F-RAM, and TRAVEO are trademarks or registered trademarks of Cypress or a subsidiary of Cypress in the United States or in other countries. For a more complete list of Cypress trademarks, visit www.infineon.com. Other names and brands may be claimed as property of their respective owners.
