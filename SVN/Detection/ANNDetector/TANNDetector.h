#ifndef TANNDETECTOR_H
#define TANNDETECTOR_H
#include <memory>
#include <map>
#include <string>
#include "WaveForm.h"
#include "DigNewChannelProcessing.h"
#include "libmseed.h"
#include "TDoOnLineDetection.h"

class TFilterBankNoTimeShiftDetection;

struct TANNChanPar {
    // std::shared_ptr<TDoOnLineDetection> detproc;
    TFilterBankNoTimeShiftDetection* filterbank;
    char chanchar;
    std::shared_ptr<CORE::Seismic_Precessing::TProcessNewChannelVector> resumpling;
};

struct TANNParameters {
    std::string configPath;
    std::string outputName;
    std::string inputName;

};
class TANNDetector
{
public:
    MSRecord *msr = NULL;
    std::unique_ptr<char[]> inputfile;
protected:
    CORE::LOGGING::Logger& logger;
    std::map<CORE::QUAKEML::WaveformStreamID, TANNChanPar> chanMap;
    std::map<std::string, std::shared_ptr<TFilterBankNoTimeShiftDetection>> fbMap;
    std::map<std::string, std::shared_ptr<TDoOffLineDetection>> detMap;
    bool getData(std::shared_ptr<CORE::DATA::SeismicChannel>& inputData);
    void initOutput(const std::string& oname, std::ofstream& outstream) { outstream.open(oname); }
    void initIntput(const std::string& iname) {inputfile.reset(new char[iname.size()+1]); strcpy(inputfile.get(), iname.c_str()); }

public:
    TANNDetector(CORE::LOGGING::Logger& log) : logger(log) {}
    virtual ~TANNDetector();
    TANNDetector(const TANNDetector& other);
    TANNDetector& operator=(const TANNDetector& other);
    void run(const TANNParameters& parameters);
};

#endif // TANNDETECTOR_H
