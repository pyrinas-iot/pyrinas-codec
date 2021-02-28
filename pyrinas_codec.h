/*
 * Copyright (c) 2021 Circuit Dojo LLC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PYRINAS_CODEC_H
#define _PYRINAS_CODEC_H

#include <qcbor/qcbor.h>
#include <stddef.h>
#include <stdint.h>

typedef struct
{
    size_t size;
    uint8_t bytes[18];
} pyrinas_event_name_data_t;

typedef struct
{
    size_t size;
    uint8_t bytes[128];
} pyrinas_event_data_t;

typedef struct
{
    pyrinas_event_name_data_t name;
    pyrinas_event_data_t data;
    uint8_t peripheral_addr[6];
    uint8_t central_addr[6];
    int8_t peripheral_rssi;
    int8_t central_rssi;
} pyrinas_event_t;

typedef enum
{
    event_name_pos,
    event_data_pos,
    event_peripheral_addr_pos,
    event_central_addr_pos,
    event_peripheral_rssi_pos,
    event_central_rssi_pos
} pyrinas_event_pos_t;

#define pyrinas_event_t_size 184 + 64

QCBORError pyrinas_codec_encode(const pyrinas_event_t *p_data, uint8_t *p_buf, size_t len, size_t *p_size);
int pyrinas_codec_decode(pyrinas_event_t *p_data, const uint8_t *p_buf, size_t len);

#endif /* _PYRINAS_CODEC_H */