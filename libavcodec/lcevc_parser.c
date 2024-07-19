/*
 * LCEVC parser
 *
 * Copyright (C) 2024 Frank Plowman <post@frankplowman.com>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "libavutil/mem.h"
#include "parser.h"
#include "startcode.h"

typedef struct LCEVCParserContext {
} LCEVCParserContext;

static int is_lcevc_nalu(const uint8_t *nalu_start, const uint8_t *nalu_end)
{
    uint16_t nalu_header;
    uint8_t nalu_type;
    const int nalu_len = nalu_end - nalu_start;
    if (nalu_len < 2)
        return 0;
    nalu_header = (nalu_start[0] << 8) + nalu_start[1];

    // Check forbidden_zero_bit and forbidden_one_bit
    if ((nalu_header & 0xc000) != 0x4000)
        return 0;

    // Check reserved bits
    if ((nalu_header & 0x01ff) != 0x01ff)
        return 0;

    // Check NALU type is one of those defined for LCEVC
    nalu_type = (nalu_header & 0x3e00) >> 9;
    if (!(nalu_type >= 28 && nalu_type <= 30))
        return 0;

    return 1;
}

static int lcevc_parser_parse(AVCodecParserContext *s, AVCodecContext *avctx,
                            const uint8_t **poutbuf, int *poutbuf_size,
                            const uint8_t *buf, int buf_size)
{
    LCEVCParserContext *ctx = s->priv_data;
    const uint8_t *buf_end = buf + buf_size;
    const uint8_t *nalu_start;
    const uint8_t *next_nalu_start = buf;
    uint32_t state = -1;
    uint8_t *base_buf;
    size_t base_buf_len;

    base_buf = av_malloc(buf_size);
    base_buf_len = 0;

    while (next_nalu_start < buf_end) {
        const uint8_t *nalu_end;
        int nalu_len;
        nalu_start = next_nalu_start;
        next_nalu_start = avpriv_find_start_code(nalu_start, buf_end, &state);
        nalu_end = next_nalu_start - 2;
        while (*nalu_end == 0x00)
            nalu_end--;
        nalu_len = nalu_end - nalu_start + 1;

        if (is_lcevc_nalu(nalu_start, nalu_end)) {
            // @TODO: Check for scaling etc.
        } else {
            base_buf[base_buf_len]     = 0x00;
            base_buf[base_buf_len + 1] = 0x00;
            base_buf[base_buf_len + 2] = 0x00;
            base_buf[base_buf_len + 3] = 0x01;
            base_buf_len += 4;
            memcpy(base_buf + base_buf_len, nalu_start, nalu_len);
            base_buf_len += nalu_len;
        }
    }

    // @TODO: Free excess base encoded data buffer.
    // @TODO: Probe base encoded data buffer and parse.
    // @TODO: Combine results of base layer and enhancement layer.

    av_freep(base_buf);

    return next_nalu_start - buf;
}

static av_cold int lcevc_parser_init(AVCodecParserContext *s)
{
    LCEVCParserContext *ctx = s->priv_data;
    int ret = 0;

    return ret;
}

static av_cold void lcevc_parser_close(AVCodecParserContext *s)
{
    LCEVCParserContext *ctx = s->priv_data;
}

const AVCodecParser ff_lcevc_parser = {
    .codec_ids      = { AV_CODEC_ID_LCEVC },
    .priv_data_size = sizeof(LCEVCParserContext),
    .parser_init    = lcevc_parser_init,
    .parser_close   = lcevc_parser_close,
    .parser_parse   = lcevc_parser_parse,
};
