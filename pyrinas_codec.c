#include "pyrinas_codec.h"
#include "qcbor.h"

#if defined(__ZEPHYR__)
#include <logging/log.h>
LOG_MODULE_REGISTER(pyrinas_codec);
#endif

#if defined(__PYRINAS_OS__)
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define LOG_INF(...) NRF_LOG_INFO(__VA_ARGS__)
#define LOG_ERR(...) NRF_LOG_ERROR(__VA_ARGS__)
#endif

#define OVERFLOW_ERROR "Potential overflow condition for: %s!"

QCBORError pyrinas_codec_encode(const pyrinas_event_t *p_data, uint8_t *p_buf, size_t buf_len, size_t *p_size)
{
    // Setup of the goods
    UsefulBuf buf = {
        .ptr = p_buf,
        .len = buf_len};
    QCBOREncodeContext ec;
    QCBOREncode_Init(&ec, buf);

    /* Create over-arching map */
    QCBOREncode_OpenMap(&ec);

    UsefulBufC data = {
        .ptr = p_data->name.bytes,
        .len = p_data->name.size};
    QCBOREncode_AddBytesToMapN(&ec, event_name_pos, data);

    data.ptr = p_data->data.bytes;
    data.len = p_data->data.size;
    QCBOREncode_AddBytesToMapN(&ec, event_data_pos, data);

    data.ptr = p_data->faddr;
    data.len = sizeof(p_data->faddr);
    QCBOREncode_AddBytesToMapN(&ec, event_faddr_pos, data);

    data.ptr = p_data->taddr;
    data.len = sizeof(p_data->taddr);
    QCBOREncode_AddBytesToMapN(&ec, event_taddr_pos, data);

    QCBOREncode_CloseMap(&ec);

    return QCBOREncode_FinishGetSize(&ec, p_size);
}

int pyrinas_codec_decode(pyrinas_event_t *p_data, const uint8_t *p_buf, size_t len)
{

    // Setup of the goods
    UsefulBufC buf = {
        .ptr = p_buf,
        .len = len};
    QCBORDecodeContext dc;
    QCBORItem item;
    QCBORDecode_Init(&dc, buf, QCBOR_DECODE_MODE_NORMAL);

    QCBORDecode_GetNext(&dc, &item);
    if (item.uDataType != QCBOR_TYPE_MAP)
    {
        LOG_ERR("Expected CBOR map structure.");
        return -1;
    }

    /* Need to set this in stone before iteration*/
    const uint8_t count = item.val.uCount;

    for (int i = 0; i < count; i++)
    {

        QCBORDecode_GetNext(&dc, &item);

        switch (item.label.int64)
        {
        case event_name_pos:
            if (item.val.string.len <= sizeof(p_data->name.bytes))
            {
                // Copy over contents
                memcpy(p_data->name.bytes, item.val.string.ptr, item.val.string.len);
                p_data->name.size = item.val.string.len;
            }
            else
            {
                LOG_ERR(OVERFLOW_ERROR, "name");
            }
            break;
        case event_data_pos:
            if (item.val.string.len <= sizeof(p_data->data.bytes))
            {
                // Copy over contents
                memcpy(p_data->data.bytes, item.val.string.ptr, item.val.string.len);
                p_data->data.size = item.val.string.len;
            }
            else
            {
                LOG_ERR(OVERFLOW_ERROR, "data");
            }
            break;
        case event_faddr_pos:
            if (item.val.string.len == sizeof(p_data->faddr))
            {
                // Copy over contents
                memcpy(p_data->faddr, item.val.string.ptr, item.val.string.len);
            }
            else
            {
                LOG_ERR(OVERFLOW_ERROR, "faddr");
            }
            break;
        case event_taddr_pos:
            if (item.val.string.len == sizeof(p_data->taddr))
            {
                // Copy over contents
                memcpy(p_data->taddr, item.val.string.ptr, item.val.string.len);
            }
            else
            {
                LOG_ERR(OVERFLOW_ERROR, "taddr");
            }
            break;
        }
    }

    return 0;
}