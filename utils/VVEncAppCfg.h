#pragma once
#include <string>
#include "vvencCfg.h"
#include "ParseArg.h"

using namespace std;


class VVEncAppCfg {
public:
    std::string  m_inputFileName;                                // source file name
    std::string  m_bitstreamFileName;                            // output bitstream file
    std::string  m_reconFileName;                                // output reconstruction file

    int          m_SourceWidth;                                  // source width in pixel
    int          m_SourceHeight;                                 // source height in pixel
    int          m_FrameRate;                                    // source frame-rates (Hz) Numerator

    VVEncAppCfg(){}
    virtual ~VVEncAppCfg(){}

    int Parse(int argc, char* argv[]);
    void CfgDefault();

};

int VVEncAppCfg::Parse(int argc, char* argv[]) {
    int ret = 0;

    Options opts;

    opts.addOptions()
    ("InputFile,i",                                     m_inputFileName,                                  "original YUV input file name or '-' for reading from stdin")
    ("BitstreamFile,b",                                 m_bitstreamFileName,                              "Bitstream output file name")
    ("ReconFile,o",                                     m_reconFileName,                                  "Reconstructed YUV output file name")
    ("SourceWidth,w",                                   m_SourceWidth,                                    "Source picture width")
    ("SourceHeight,h",                                  m_SourceHeight,                                   "Source picture height")
    ("FrameRate,-fr",                                   m_FrameRate,                                      "Temporal rate (framerate numerator) e.g. 25,30, 30000, 50,60, 60000")
    ;

    const std::list<const char*>& argv_unhandled = scanArgv( opts, argc, (const char**) argv);

    return ret;
}

void VVEncAppCfg::CfgDefault() {
    m_inputFileName = "";
    m_bitstreamFileName = "";
    m_reconFileName = "";

    m_SourceWidth         = 0;
    m_SourceHeight        = 0;
    m_FrameRate           = 0;
}
