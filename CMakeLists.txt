
#
# Copyright (c) 2020 Circuit Dojo LLC
#
# SPDX-License-Identifier: Apache-2.0
#

zephyr_include_directories(.)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/pyrinas_codec.c)