#pragma once
#include <string>
#include "vvencCfg.h"
#include "ParseArg.h"

using namespace std;
namespace po = apputils::program_options;

namespace apputils {

class VVEncAppCfg {
public:
    std::string  m_inputFileName;                                ///< source file name
    std::string  m_bitstreamFileName;                            ///< output bitstream file
    std::string  m_reconFileName;                                ///< output reconstruction file

    VVEncAppCfg(){}
    virtual ~VVEncAppCfg(){}

    int parse(int argc, char* argv[], vvenc_config* c);

};

int VVEncAppCfg::parse(int argc, char* argv[], vvenc_config* c) {
    int ret = 0;

    po::Options opts;

    opts.addOptions()
    ("InputFile,i",                                     m_inputFileName,                                     "original YUV input file name or '-' for reading from stdin")
    ("BitstreamFile,b",                                 m_bitstreamFileName,                                 "Bitstream output file name")
    ("ReconFile,o",                                     m_reconFileName,                                     "Reconstructed YUV output file name")
    ("SourceWidth,w",                                   c->m_SourceWidth,                                    "Source picture width")
    ("SourceHeight,h",                                  c->m_SourceHeight,                                   "Source picture height")
    ("FrameRate,-fr",                                   c->m_FrameRate,                                      "Temporal rate (framerate numerator) e.g. 25,30, 30000, 50,60, 60000")
    ;

    po::ErrorReporter err;
    const std::list<const char*>& argv_unhandled = po::scanArgv( opts, argc, (const char**) argv, err );

    return ret;
}

}