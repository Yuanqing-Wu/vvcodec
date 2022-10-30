#pragma once

typedef struct vvenc_config {
    // core config params
    int                 m_SourceWidth;                                                     // source width in pixel
    int                 m_SourceHeight;                                                    // source height in pixel (when interlaced = field height)
    int                 m_FrameRate;                                                       // source frame-rates (Hz) Numerator
} vvenc_config;

int vvenc_init_default(vvenc_config *c);