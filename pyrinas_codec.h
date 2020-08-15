#ifndef _PYRINAS_CODEC_H
#define _PYRINAS_CODEC_H

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
    uint8_t faddr[6];
    uint8_t taddr[6];
} pyrinas_event_t;

#define pyrinas_event_t_size 184 + 64

int pyrinas_codec_encode(const pyrinas_event_t *p_data, uint8_t *p_buf, size_t len, size_t *p_size);
int pyrinas_codec_decode(pyrinas_event_t *p_data, const uint8_t *p_buf, size_t len);

#endif /* _PYRINAS_CODEC_H */