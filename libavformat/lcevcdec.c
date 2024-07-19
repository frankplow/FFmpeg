/*
 * RAW LCEVC video demuxer
 * Copyright (c) 2024 Frank Plowman <post@frankplowman.com>
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

#include <stddef.h>

#include "rawdec.h"

static int lcevc_probe(const AVProbeData *p)
{
    uint32_t code = -1;

    for (unsigned i = 0; i < p->buf_size - 1; i++) {
        // Check for NAL unit start codes.
        code = (code << 8) + p->buf[i];
        if ((code & 0xffffff00) == 0x100) {
            const uint16_t nal_unit_header = (p->buf[i] << 8) + p->buf[i + 1];
            uint8_t nal_unit_type;

            // Check forbidden_zero_bit and forbidden_one_bit
            if ((nal_unit_header & 0xc000) != 0x4000)
                continue;

            // Check reserved bits
            if ((nal_unit_header & 0x01ff) != 0x01ff)
                continue;

            nal_unit_type = (nal_unit_header & 0x3e00) >> 9;
            if (nal_unit_type >= 28 && nal_unit_type <= 30)
                // +2 ensures LCEVC takes precedence over H.264, HEVC and VVC, whose
                // probe functions each return AVPROBE_SCORE_EXTENSION + 1.
                return AVPROBE_SCORE_EXTENSION + 2;
        }
    }

    // @TODO: This will almost certainly need to be more strict.  For now, though,
    //        this appears to be sufficient to distinguish LCEVC bitstreams from
    //        plain AVC/HEVC/VVC.

    return 0;
}

FF_DEF_RAWVIDEO_DEMUXER(lcevc, "raw LCEVC video", lcevc_probe, "lcevc", AV_CODEC_ID_LCEVC)
