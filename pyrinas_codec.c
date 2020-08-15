#include "pyrinas_codec.h"

#if defined(CONFIG_TINYCBOR)
#include <tinycbor/cbor.h>
#include <tinycbor/cbor_buf_writer.h>
#include <tinycbor/cbor_buf_reader.h>
#else
#include <cbor.h>
#endif

#if defined(NRF_LOG_ENABLED)
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#endif

int pyrinas_codec_encode(const pyrinas_event_t *p_data, uint8_t *p_buf, size_t buf_len, size_t *p_size)
{
    CborEncoder cbor, cbor_map;
    CborError err;
    int i = 0;

#if defined(CONFIG_TINYCBOR)
    struct cbor_buf_writer writer;
    cbor_buf_writer_init(&writer, p_buf, buf_len);
    cbor_encoder_init(&cbor, &writer.enc, 0);
#else
    cbor_encoder_init(&cbor, p_buf, buf_len, 0);
#endif

    err = cbor_encoder_create_map(&cbor, &cbor_map, CborIndefiniteLength);
    if (err)
        return -1;

    cbor_encode_uint(&cbor_map, i++);
    err = cbor_encode_byte_string(&cbor_map, p_data->name.bytes, p_data->name.size);
    if (err)
        return -1;

    cbor_encode_uint(&cbor_map, i++);
    err = cbor_encode_byte_string(&cbor_map, p_data->data.bytes, p_data->data.size);
    if (err)
        return -1;

    cbor_encode_uint(&cbor_map, i++);
    err = cbor_encode_byte_string(&cbor_map, p_data->faddr, sizeof(p_data->faddr));
    if (err)
        return -1;

    cbor_encode_uint(&cbor_map, i++);
    err = cbor_encode_byte_string(&cbor_map, p_data->taddr, sizeof(p_data->taddr));
    if (err)
        return -1;

    err = cbor_encoder_close_container(&cbor, &cbor_map);
    if (err)
        return -1;

/* Buff size */
#if defined(CONFIG_TINYCBOR)
    *p_size = (size_t)(writer.ptr - p_buf);
#else
    *p_size = cbor_encoder_get_buffer_size(&cbor, p_buf);
#endif

#if defined(NRF_LOG_ENABLED)
    NRF_LOG_HEXDUMP_DEBUG(p_buf, *p_size);
#endif

    return 0;
}

int pyrinas_codec_decode(pyrinas_event_t *p_data, const uint8_t *p_buf, size_t len)
{

    CborParser parser;
    CborValue value, map_value;
    CborError err;

#if defined(CONFIG_TINYCBOR)
    struct cbor_buf_reader reader;

    /* initalize the reader */
    cbor_buf_reader_init(&reader, p_buf, len);
    err = cbor_parser_init(&reader.r, 0, &parser, &value);
#else
    err = cbor_parser_init(p_buf, len, 0, &parser, &value);
#endif
    if (err)
        return -1;

#if defined(NRF_LOG_ENABLED)
    NRF_LOG_HEXDUMP_DEBUG(p_buf, len);
#endif

    /* Return if we're not dealing with a map*/
    if (!cbor_value_is_map(&value))
    {
#if defined(NRF_LOG_ENABLED)
        NRF_LOG_ERROR("Unexpected CBOR data structure.\n");
#endif
        return -1;
    }

    /* Enter map */
    cbor_value_enter_container(&value, &map_value);

    /* Get name */
    size_t size = sizeof(p_data->name.bytes);
    cbor_value_advance_fixed(&map_value);
    err = cbor_value_copy_byte_string(&map_value, p_data->name.bytes, &size, &map_value);
    if (err)
        return -1;
    p_data->name.size = size;

    // Get data
    size = sizeof(p_data->data.bytes);
    cbor_value_advance_fixed(&map_value);
    err = cbor_value_copy_byte_string(&map_value, p_data->data.bytes, &size, &map_value);
    if (err)
        return -1;
    p_data->data.size = size;

    size = sizeof(p_data->faddr);
    cbor_value_advance_fixed(&map_value);
    err = cbor_value_copy_byte_string(&map_value, p_data->faddr, &size, &map_value);
    if (err)
        return -1;

    size = sizeof(p_data->taddr);
    cbor_value_advance_fixed(&map_value);
    err = cbor_value_copy_byte_string(&map_value, p_data->taddr, &size, &map_value);
    if (err)
        return -1;

    return 0;
}