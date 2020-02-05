#include "TANNDetector.h"
#include "file_sc_xml.h"
#include "Exceptions.h"
#include "FilterBank.h"
#include "SOH.h"

class TFileStaVoting : public TStaVotingBase {
    std::ostream* file;
    std::string sta;
public:
    void on(const CORE::TIME::QMLTime& t, int) override { *file << "ON " << sta << t.toString() << std::endl; }
    void off(const CORE::TIME::QMLTime& t, int maxted) override { *file << "OFF " << sta << t.toString() << " /" << maxted << std::endl; }
    TFileStaVoting(std::ostream* f, const std::string s) : file(f), sta(s) {}
};

void TANNDetector::run(const TANNParameters& parameters) {
    try {
 //       initDetParameters(parameters, "DetectionParameters"));
        std::string configName = parameters.configPath + "/ANNDetectionConfig.xml";
        XMLFileNode rootNode(configName.c_str());
        XMLNode inputsNode = rootNode.child("inputs");
        std::string timeShift = inputsNode.child("method").attribute("time_shift");
        if (timeShift != "0") throw(CORE::EXCEPTIONS::CoreException("Time shift detection not implemented"));
        std::string stepText = inputsNode.attribute("step");
        CORE::TIME::QMLTimeMicroseconds step = stoll(inputsNode.attribute("step"));
        std::string filterPath = parameters.configPath + "/FilterBank/";
        XMLNode annNode = rootNode.child("ANN");
        if (annNode.attribute("type") != "SLRNN")  throw CORE::EXCEPTIONS::CoreException("Wrong ANN type.Must be SLRNN");
        std::string annGenericName = parameters.configPath + "/" + annNode.attribute("file");
        std::ofstream outstream;
        initOutput(parameters.outputName, outstream);
        initIntput(parameters.inputName);

//        CORE::PLUGINS::IProcessPlugin* resumpling_plugin = dynamic_cast<IProcessPlugin*>(pd[0]->pluginPtrs[0]);
//        if (resumpling_plugin == NULL) throw CORE::EXCEPTIONS::PluginException("Missing resampling process");
        std::shared_ptr<CORE::DATA::SeismicChannel> inputData;
        while(getData(inputData)) {
            auto chanItr = chanMap.find(inputData->channelName);
            if(chanItr == chanMap.end()) {
                TANNChanPar chanDetVar;
                std::string staName = inputData->channelName.getStaName();
                auto detItr = detMap.find(staName);
                if(detItr == detMap.end()) {
                    logger.c_log(CORE::LOGGING::LL_REPORT, "Init detection %s: ",staName.c_str());
                    std::shared_ptr<TStaVotingBase> staVoting(new TFileStaVoting(&outstream, staName));
                    std::shared_ptr<TDoOffLineDetection> detproc(new TDoOffLineDetection(logger, staVoting, step));
                    auto insret = detMap.insert(std::make_pair(staName,detproc));
                    detItr = insret.first;
                }
                auto fbItr = fbMap.find(staName);
                if(fbItr == fbMap.end()) {
                    XMLNode configNode = inputsNode.child("Default_Station", false);
                    for (XMLNode staNode = inputsNode.firstchild("Station"); !staNode.isNull(); staNode = inputsNode.nextchild("Station")) {
                        if (staNode.attribute("sta_name", false) == staName || staNode.attribute("sta_names", false).find(staName + ";") != std::string::npos) {
                            logger << CORE::LOGGING::LL_REPORT << "Individual input parameters " + staNode.attribute("name", false);
                            configNode = staNode;
                            break;
                        }
                    }
                    std::shared_ptr<TFilterBankNoTimeShiftDetection> filterbank(new TFilterBankNoTimeShiftDetection(configNode, filterPath, *(detItr->second)));
                    filterbank->setStep(step);
                    filterbank->reset();
                    auto insret = fbMap.insert(std::make_pair(staName,filterbank));
                    fbItr = insret.first;
                    std::string annName = annGenericName;
                    for (XMLNode annStaNode = annNode.firstchild("STA"); !annStaNode.isNull(); annStaNode = annNode.nextchild("STA")) {
                        if (annStaNode.attribute("name") == staName) {
                            annName = parameters.configPath + "/config/" + annStaNode.attribute("file");
                            break;
                        }
                    }
                    detItr->second->reset(annName);
                    detItr->second->setSta(inputData->channelName);
                }

                chanDetVar.resumpling = std::shared_ptr<CORE::Seismic_Precessing::TProcessNewChannelVector>(new CORE::Seismic_Precessing::TSOHNewChannelVector(int(10000.0 / inputData->channelParameters->sps() + 0.5) * 100));
                // chanDetVar.detproc = detItr.second;
                chanDetVar.filterbank = fbItr->second.get();
                chanDetVar.chanchar = inputData->channelName.channelCode[2];
                auto insret = chanMap.insert(std::make_pair(inputData->channelName,chanDetVar));
                chanItr = insret.first;
            }
            auto resumpling = chanItr->second.resumpling.get();
            auto filterbank = chanItr->second.filterbank;
            char chanchar = chanItr->second.chanchar;
            inputData->apply(*resumpling);
            auto resumpledChannel = resumpling->newChannel();
            if (resumpledChannel->waveform.isEmpty()) continue;
            for (auto& wf : resumpledChannel->waveform.getWaveforms()) {
                CORE::DATA::SingleCW* mch = dynamic_cast<CORE::DATA::SingleCW*>(wf.get());
                if(mch) filterbank->push(mch, chanchar);
            }
        }
        logger << CORE::LOGGING::LL_REPORT << std::string("End of input ") + parameters.inputName;
    }
    catch(...) {
    }
}

TANNDetector::~TANNDetector() {
    //dtor
}

TANNDetector::TANNDetector(const TANNDetector& other) : logger(other.logger) {
    //copy ctor
}

TANNDetector& TANNDetector::operator=(const TANNDetector& rhs) {
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

bool TANNDetector::getData(std::shared_ptr<CORE::DATA::SeismicChannel>& inputData) {
    int retcode = ms_readmsr(&msr, inputfile.get(), 0, NULL, NULL, 1, 1, -1);
    if (retcode != MS_NOERROR){
        return false;
    }
    inputData->channelName = CORE::QUAKEML::WaveformStreamID(msr->network, msr->station, msr->location, msr->channel);
    inputData->waveform.clear();
    if (msr->samplecnt == 0) return true;
    CORE::DATA::SingleCW* wfrm = new CORE::DATA::ContinousWaveform<CORE::DATA::singleSampleType>();
    wfrm->sType = CORE::DATA::ST_SINGLE;
    wfrm->samples.resize(msr->numsamples);
    wfrm->timeFrom = CORE::TIME::QMLTime(CORE::TIME::QMLTimeMicroseconds(msr->starttime));
    wfrm->givenSamplingPeriod = (1.0e6 / msr->samprate + 0.5);
    wfrm->timeTo = wfrm->timeFrom + wfrm->givenSamplingPeriod*msr->numsamples;
    inputData->waveform.addContinousWaveform(wfrm);
    switch (msr->sampletype){
        case 'i': {
            int32_t* datasamples = (int32_t*)(msr->datasamples);
            for (long i = 0; i < msr->numsamples; i++){
                wfrm->samples[i] = CORE::DATA::singleSampleType(datasamples[i]);
            }
            break;
        }
        case 'f': {
            float* datasamples = (float*)(msr->datasamples);
            for (long i = 0; i < msr->numsamples; i++) {
                wfrm->samples[i] = CORE::DATA::singleSampleType(datasamples[i]);
            }
            break;
        }
        case 'd': {
            double* datasamples = (double*)(msr->datasamples);
            for (long i = 0; i < msr->numsamples; i++){
                wfrm->samples[i] = CORE::DATA::singleSampleType(datasamples[i]);
            }
            break;
        }
        case 'a': {
            logger << CORE::LOGGING::LL_WARNING << "Program can not read ASCII MiniSEED";
        }
        default: {
            for (long i = 0; i < msr->numsamples; wfrm->samples[i++] = 0.0);
        }
    }
    return true;
}
