# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.13.1)

if(CONFIG_ZMK_RGB_UNDERGLOW)
  zephyr_library_named(zmk_sofle_rgb_layer)
  zephyr_library_sources(src/rgb_layer_indicator.c)
endif()
