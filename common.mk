################################################################################
# \file common.mk
# \version 1.0
#
# \brief
# Settings for all projects
#
################################################################################
# \copyright
# $ Copyright 2023-YEAR Cypress Semiconductor Apache2 $
################################################################################

# Target board/hardware (BSP).
# To change the target, it is recommended to use the Library manager
# ('make modlibs' from command line), which will also update Eclipse IDE launch
# configurations. If TARGET is manually edited, ensure TARGET_<BSP>.mtb with a
# valid URL exists in the application, run 'make getlibs' to fetch BSP contents
# and update or regenerate launch configurations for your IDE.
# You may have your custom PSoC61 KIT defined here !
#TARGET=PSoC61_KIT 
TARGET=ELROND_BACKPLANE

# Name of toolchain to use. Options include:
#
# GCC_ARM -- GCC provided with ModusToolbox IDE
# ARM     -- ARM Compiler (must be installed separately)
# IAR     -- IAR Compiler (must be installed separately)
#
# See also: CY_COMPILER_PATH below
TOOLCHAIN=GCC_ARM

# Default build configuration. Options include:
#
# Debug -- build with minimal optimizations, focus on debugging.
# Release -- build with full optimizations
# Custom -- build with custom configuration, set the optimization flag in CFLAGS
CONFIG=Debug

include ../common_app.mk
