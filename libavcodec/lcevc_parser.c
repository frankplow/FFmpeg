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

#include "parser.h"

typedef struct LCEVCParserContext {
} LCEVCParserContext;

static int lcevc_parser_parse(AVCodecParserContext *s, AVCodecContext *avctx,
                            const uint8_t **poutbuf, int *poutbuf_size,
                            const uint8_t *buf, int buf_size)
{
    int next;
    LCEVCParserContext *ctx = s->priv_data;

    next = buf_size;

    return next;
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
