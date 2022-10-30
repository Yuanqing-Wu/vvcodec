#include <iostream>
#include "VVEncAppCfg.h"

bool parseCfg(int argc, char* argv[], apputils::VVEncAppCfg& rcVVEncAppCfg, vvenc_config &vvenccfg) {
    bool ret = true;

    if(argc){
        // remove application name
        argc--;
        argv++;
    }

    int parserRes = rcVVEncAppCfg.parse(argc, argv, &vvenccfg);

    return ret;
}

int main (int argc, char* argv[]) {   
    apputils::VVEncAppCfg vvencappCfg = apputils::VVEncAppCfg();

    vvenc_config vvenccfg;
    vvenc_init_default(&vvenccfg);

    // parse configuration
    if (!parseCfg(argc, argv, vvencappCfg, vvenccfg)) {
        return 1;
    }
    return 0;
}