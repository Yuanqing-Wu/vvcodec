#include "vvencCfg.h"

int vvenc_init_default(vvenc_config *c) {
    int iRet = 0;
    c->m_SourceWidth         = 0;
    c->m_SourceHeight        = 0;
    c->m_FrameRate           = 0;
    return iRet;
}