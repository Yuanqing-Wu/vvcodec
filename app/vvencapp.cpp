#include <iostream>
#include "VVEncAppCfg.h"

bool parseCfg(int argc, char* argv[], VVEncAppCfg& rcVVEncAppCfg) {
    bool ret = true;

    if(argc){
        // remove application name
        argc--;
        argv++;
    }

    int parserRes = rcVVEncAppCfg.Parse(argc, argv);

    return ret;
}

int main (int argc, char* argv[]) {   
    VVEncAppCfg vvencappCfg = VVEncAppCfg();
    vvencappCfg.CfgDefault();

    // parse configuration
    if (!parseCfg(argc, argv, vvencappCfg)) {
        return 1;
    }
    return 0;
}