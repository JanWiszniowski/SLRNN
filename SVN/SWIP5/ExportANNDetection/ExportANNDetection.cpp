#include "stdafx.h"
#include <fstream>
#include <wx/wx.h>
#include "Exceptions.h"
#include "ExportANNDetection.h"
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include "IFilterPlugin.h"
#include "WaveFormsLine.h"
#include "FilterBank.h"
#include "WaveformStreamIDAux.h"
#include <stdio.h>
#include "SLRNNFile.h"
#include "Time2EntryDialog.h"
#include "InitParameters.h"
#include <wx/choicdlg.h>

#if defined(_MSC_VER)
extern "C" __declspec(dllexport) void* getClass();
#elif defined(__GNUC__)
extern "C" void* getClass();
#endif
namespace CORE { namespace PLUGINS {

    struct ExANNDet {
        boost::optional<CORE::QUAKEML::TimeQuantity> P, S, evTime;
        std::string evtId;
    };
    struct PureSingleCW {
        char c;
        std::shared_ptr<CORE::DATA::IContinousWaveform> d;
        PureSingleCW(std::shared_ptr<CORE::DATA::IContinousWaveform> id, char ic) : c(ic), d(id) {}
    };
    bool compSingleCW(const PureSingleCW& i1, const PureSingleCW& i2) { return i1.d->timeFrom < i2.d->timeFrom; }

    class TWriteToFile: public TSequentialDetection {
        std::vector<double> _v;
        CORE::TIME::QMLTime _t;
        FILE* file;
        std::string fileName;
        CORE::DATA::SharedDataContainer* sdc;
        std::string _sta;
        std::vector<ExANNDet>* phaseVect;
        CORE::TIME::QMLTimeMicroseconds _step = CORE::TIME::MODULO_SEC;
    protected:
        void proc(const std::vector<double>& vector, const CORE::TIME::QMLTime& ttime) override {
            //CORE::QUAKEML::Event* evt = sdc->quakeML->getCurrentEventPtr();
            //CORE::QUAKEML::Origin* org = sdc->quakeML->getCurrentOriginPtr();
            CORE::TIME::QMLTimeMicroseconds sec1(CORE::TIME::MODULO_SEC);
            _v = vector;
            _t = ttime - _timeShift;
            if (file) {
                fprintf(file, "%.3lf", _t.toDouble());
                for (auto&& val : vector) {
                    fprintf(file, " %f", val);
                }
				const double LowEta = 1.0e-12;
				const double MidleEta = 1.0;
                const double HighEta = 10.0;
                const double ExtraHiEta = 50.0;
                const double LowOutput = 0.0;
                const double HighOutput = 1.0;
                double outD = LowOutput;
                double outP = LowOutput;
                double outS = LowOutput;
                double etaD = MidleEta;
                double etaP = MidleEta;
                double etaS = MidleEta;
                double eventT = 0.0;
				if (phaseVect != nullptr) for (auto&& phs : *phaseVect) {
                    if (phs.S && phs.P) {
                        // P phase
                        if (phs.P->value < _t && _t < phs.S->value + sec1 * 2) {
                            outP = HighOutput;
                            etaP = HighEta;
                        }
                        if (phs.P->value - _step < _t && _t < phs.P->value + _step * 2) {
                            etaP = LowEta;
                        }
                        if (phs.S->value + _step * 4 < _t && _t < phs.S->value + sec1 * 15) {
                            etaP = LowEta;
                        }
                        // S phase
                        //if (phs.S->value < _t && _t < phs.S->value  + sec1 * 3) {
                        //    outS = HighOutput;
                        //    etaS = HighEta;
                        //}
                        if (phs.S->value < _t && _t < phs.S->value + _step * 7) {
                            outS = HighOutput;
                            etaS = HighEta;
                        }
                        if (phs.P->value < _t && _t < phs.S->value + _step * 7) {
                            etaS = HighEta;
                        }

                        if (phs.S->value - _step < _t && _t < phs.S->value + _step * 2) {
                            etaS = LowEta;
                        }
                        if (phs.S->value + _step * 6 < _t && _t < phs.S->value + sec1 * 15) {
                            etaS = LowEta;
                        }
                        // Detection
                        if (phs.S->value + _step < _t && _t < phs.S->value + sec1 * 5) {
                            outD = HighOutput;
                            etaD = ExtraHiEta;
                        }
                        //if ((phs.P->value + sec1 * 2 < _t || phs.S->value - _step < _t) && _t < phs.S->value + _step * 3) {
                        //    etaD = LowEta;
                        //}
                        if (phs.S->value - _step < _t && _t < phs.S->value + _step * 4) {
                            etaD = LowEta;
                        }
                        if (phs.S->value + _step * 6 < _t && _t < phs.S->value + sec1 * 15) {
                            etaD = LowEta;
                        }
						if (phs.S && phs.S->value <= _t && phs.S->value + _step > _t) {
							eventT += 100;
						}
						if (phs.P && phs.P->value <= _t && phs.P->value + _step > _t) {
							eventT += 10;
						}
					}
                    else if (phs.S) {
                        // P phase
                        if (phs.S->value - sec1 * 8 < _t && _t < phs.S->value + sec1 * 5) {
                            etaP = LowEta;
                        }
                        // S phase
                        if (phs.S->value < _t && _t < phs.S->value + sec1 * 5) {
                            outS = HighOutput;
                            etaS = HighEta;
                        }
                        if (phs.S->value - _step < _t && _t < phs.S->value + _step * 2) {
                            etaS = LowEta;
                        }
                        if (phs.S->value + _step * 3 < _t && _t < phs.S->value + sec1 * 15) {
                            etaS = LowEta;
                        }
                        // Detection
						if (phs.S->value + _step  < _t && _t < phs.S->value + sec1 * 5) {
                            outD = HighOutput;
                            etaD = ExtraHiEta;
                        }
                        if (phs.S->value - sec1 < _t && _t < phs.S->value + _step * 4) {
                            etaD = LowEta;
                        }
                        if (phs.S->value + _step * 7 < _t && _t < phs.S->value + sec1 * 15) {
                            etaD = LowEta;
                        }
						if (phs.S && phs.S->value <= _t && phs.S->value + _step > _t) {
							eventT += 100;
						}
					}
                    else if (phs.P) {
                        // P phase
                        if (phs.P->value < _t && _t < phs.P->value + sec1 * 10) {
                            outP = HighOutput;
                            etaP = HighEta;
                        }
                        if (phs.P->value - _step < _t && _t < phs.P->value + _step * 2) {
                            etaP = LowEta;
                        }
                        if (phs.P->value + sec1 * 7 < _t && _t < phs.P->value + sec1 * 20) {
                            etaP = LowEta;
                        }
                        // S phase
                        if (phs.P->value + _step < _t && _t < phs.P->value + sec1 * 20) {
							etaS = LowEta;
                        }
                        // Detection
                        if (phs.P->value + sec1 * 5 < _t && _t < phs.P->value + sec1 * 8) {
                            outD = HighOutput;
                            etaD = ExtraHiEta;
                        }
                        if (phs.P->value + _step < _t && _t < phs.P->value + sec1 * 6) {
                            etaD = LowEta;
                        }
                        if (phs.P->value + sec1 * 6 + 2 * _step < _t && _t < phs.P->value + sec1 * 20) {
                            etaD = LowEta;
                        }
						if (phs.P && phs.P->value <= _t && phs.P->value + _step > _t) {
							eventT += 10;
						}
					}
                    if (phs.evTime && phs.evTime->value <= _t && phs.evTime->value + _step > _t) {
                        eventT += 1;
                    }
                }
                fprintf(file, " %f %f %f %f %f %f %f\n", outD, outP, outS, etaD, etaP, etaS, eventT);
            }
        }
    public:
        TWriteToFile(CORE::DATA::SharedDataContainer* sd, CORE::TIME::QMLTimeMicroseconds step = 100000LL) { sdc = sd;  file = NULL; _step = step; }
        ~TWriteToFile() { reset(); }
        void reset() override {
            if (file) fclose(file);
            if (fileName.empty()) {
                file = nullptr;
            }
            else {
                file = fopen(fileName.c_str(), "w");
            }
        }
        void reset(const std::string& fname, std::vector<ExANNDet>* pv) { fileName = fname; phaseVect = pv; reset(); }
        bool ready() override { return false; }
        bool get(std::vector<double>& v, CORE::TIME::QMLTime& t) override { v = _v; t = _t; return false; }
        CORE::TIME::QMLTime& time() override { return _t; }
        void setSta(const CORE::QUAKEML::WaveformStreamID& w) { _sta = w.stationCode; }

    };

    struct TDetectionList {
        bool on;
        CORE::QUAKEML::WaveformStreamID sta;
        CORE::TIME::QMLTime time;
        int nover;
        TDetectionList(bool o, const CORE::QUAKEML::WaveformStreamID& s, const CORE::TIME::QMLTime& t, int no = 0) : on(o), sta(s), time(t), nover(no) {}
        bool operator <(const TDetectionList& d) { return time < d.time; }
    };

    class TDoOffLineDetection : public TSequentialDetection {
        CORE::QUAKEML::WaveformStreamID sta;
        std::vector<double> _v;
        CORE::TIME::QMLTime _t;
        std::string annName;
        FILE* file=NULL;
        std::string fileName;
        int arming = 0;
        int deton;
        int ontime = 0;
        int index;
        CORE::DATA::SharedDataContainer* sdc;
        CORE::TIME::QMLTimeMicroseconds _step = CORE::TIME::MODULO_SEC;
        std::unique_ptr<CORE::Seismic_Precessing::TSLRNN <double, double> > detPtr;
        int maxdet = 0;
    protected:
        void proc(const std::vector<double>& vector, const CORE::TIME::QMLTime& ttime) override {
            //CORE::QUAKEML::Event* evt = sdc->quakeML->getCurrentEventPtr();
            //CORE::QUAKEML::Origin* org = sdc->quakeML->getCurrentOriginPtr();
            CORE::TIME::QMLTimeMicroseconds sec1(CORE::TIME::MODULO_SEC);
            _v = vector;
            _t = ttime - _timeShift;
            detPtr->process(vector.data());
            if (arming > 300) {
                if (detPtr->output(0) > 0) {
                    if (deton == 1) {
                        list.push_back(TDetectionList(true,sta,_t));
                        if (file) {
                            fprintf(file, "ON %s %s\n", _t.toString().c_str(), sta.getStaName().c_str());
                        }
                        sdc->logger.c_log(CORE::LOGGING::LL_REPORT, "Detection %d: %s on at %s", index++, sta.getStaName().c_str(), _t.toString().c_str());
                        ontime = 20 * 1000000 / _step;
                    }
                    deton++;
                    if (maxdet < deton) maxdet = deton;
                }
                else {
                    if (deton > 1) {
                        if (ontime > 0) {
                            ontime--;
                        }
                        else {
                            list.push_back(TDetectionList(false, sta, _t, maxdet));
                            if (file) {
                                fprintf(file, "OFF %s %s/%d\n", _t.toString().c_str(), sta.getStaName().c_str(), maxdet);
                            }
                            sdc->logger.c_log(CORE::LOGGING::LL_REPORT, " off %d: %s on at %s", maxdet, sta.getStaName().c_str(), _t.toString().c_str());
                            deton = 0;
                            maxdet = 0;
                        }
                    }
                    else {
                        deton = 0;
                    }
                }
            }
            else {
                arming++;
            }
        }
    public:
        std::list<TDetectionList> list;
        TDoOffLineDetection(CORE::DATA::SharedDataContainer* sd, const std::string& fn, CORE::TIME::QMLTimeMicroseconds step = 100000LL) {
            file = fopen(fn.c_str(), "w");
            sdc = sd;
            _step = step;
        }
        ~TDoOffLineDetection() {
            if (deton > 0) {
                fprintf(file, "OFF %s %s/%d\n", _t.toString().c_str(), sta.getStaName().c_str(), maxdet);
            }
            fclose(file);
        }
        void reset() override {
//            if (file) fflush(file);
            index = 1;
            arming = 0;
            deton = 0;
            ontime = 0;
            maxdet = 0;
            if (annName.empty()) {
                detPtr.reset();
            }
            else {
                sdc->logger.c_log(CORE::LOGGING::LL_REPORT, "SLRNN '%s'", annName.c_str());
                detPtr.reset(new CORE::Seismic_Precessing::TSLRNNFile(annName));
            }
        }
        void reset(const std::string& aname) { annName = aname; reset(); }
        bool ready() override { return false; }
        void setSta(const CORE::QUAKEML::WaveformStreamID& w) {
            if (deton > 1) {
                fprintf(file, "OFF %s %s/%d\n", _t.toString().c_str(), sta.getStaName().c_str(), maxdet);
                deton = 0;
                maxdet = 0;
            }
            sta = w.getNetSta();
        }
        bool get(std::vector<double>& v, CORE::TIME::QMLTime& t) override { v = _v; t = _t; return false; }
        CORE::TIME::QMLTime& time() override { return _t; }

    };

    class TDetTest : public TSequentialDetection {
        std::vector<double> _v;
        CORE::TIME::QMLTime _t;
        std::string annName;
        int arming = 0;
        int deton;
        int ontime = 0;
        CORE::DATA::SharedDataContainer* sdc;
        CORE::QUAKEML::Pick pick;
        CORE::TIME::QMLTimeMicroseconds _step = CORE::TIME::MODULO_SEC;
        std::unique_ptr<CORE::Seismic_Precessing::TSLRNN <double, double> > detPtr;
    protected:
        void proc(const std::vector<double>& vector, const CORE::TIME::QMLTime& ttime) override {
            //CORE::QUAKEML::Event* evt = sdc->quakeML->getCurrentEventPtr();
            //CORE::QUAKEML::Origin* org = sdc->quakeML->getCurrentOriginPtr();
            CORE::TIME::QMLTimeMicroseconds sec1(CORE::TIME::MODULO_SEC);
            _v = vector;
            _t = ttime - _timeShift;
            detPtr->process(vector.data());
            int index = 1;
            //if (arming > 120) {
            //    if (detPtr->output(0) > 0) {
            //        if (deton == 1) {
            //            pick.phaseHint->code = "dB";
            //            pick.time.value = _t;
            //            sdc->proposedPicks.back().picks->push_back(pick);
            //            sdc->logger.c_log(CORE::LOGGING::LL_REPORT, "Det%3d: %s on at %s", index++, pick.waveformID.getChannelName().c_str(), _t.toString().c_str());
            //        }
            //        deton++;
            //    } else {
            //        if (deton > 1) {
            //            pick.phaseHint->code = "dE " + std::to_string(deton);
            //            pick.time.value = _t;
            //            sdc->proposedPicks.back().picks->push_back(pick);
            //        }
            //        deton = 0;
            //    }
            //}
            //else {
            //    arming++;
            //}
            if (arming > 120) {
                if (detPtr->output(0) > 0) {
                    if (deton == 1) {
                        pick.phaseHint->code = "dB";
                        pick.time.value = _t;
                        sdc->proposedPicks.back().picks->push_back(pick);
                        sdc->logger.c_log(CORE::LOGGING::LL_REPORT, "Det%3d: %s on at %s", index++, pick.waveformID.getChannelName().c_str(), _t.toString().c_str());
                        ontime = 20 * 1000000 / _step;
                    }
                    deton++;
                }
                else {
                    if (deton > 1) {
                        if (ontime > 0) {
                            ontime--;
                        }
                        else {
                            pick.phaseHint->code = "dE " + std::to_string(deton);
                            pick.time.value = _t;
                            sdc->proposedPicks.back().picks->push_back(pick);
                            deton = 0;
                        }
                    }
                    else {
                        deton = 0;
                    }
                }
            }
            else {
                arming++;
            }
        }
    public:
        TDetTest(CORE::DATA::SharedDataContainer* sd, CORE::TIME::QMLTimeMicroseconds step = 100000LL) {
            sdc = sd;
            _step = step;
            pick.phaseHint = Phase();
            pick.waveformID.channelCode[2] = 'Z';
        }
        ~TDetTest() {}
        void reset() override {
            arming = 0;
            deton = 0;
            if (annName.empty()) {
                detPtr.reset();
            }
            else {
                sdc->logger.c_log(CORE::LOGGING::LL_REPORT, "SLRNN '%s'", annName.c_str());
                detPtr.reset(new CORE::Seismic_Precessing::TSLRNNFile(annName));
            }
        }
        void reset(const std::string& aname) { annName = aname; reset(); }
        bool ready() override { return false; }
        void setSta(const CORE::QUAKEML::WaveformStreamID& w) { pick.waveformID = w; }
        bool get(std::vector<double>& v, CORE::TIME::QMLTime& t) override { v = _v; t = _t; return false; }
        CORE::TIME::QMLTime& time() override { return _t; }

    };

    class TWriteToFileDetTest : public TSequentialDetection {
        std::vector<double> _v;
        CORE::TIME::QMLTime _t;
        FILE* file;
        std::string fileName;
        std::string annName;
        CORE::DATA::SharedDataContainer* sdc;
        std::string _sta;
        CORE::TIME::QMLTimeMicroseconds _step = CORE::TIME::MODULO_SEC;
        std::unique_ptr<CORE::Seismic_Precessing::TSLRNN <double, double> > detPtr;
    protected:
        void proc(const std::vector<double>& vector, const CORE::TIME::QMLTime& ttime) override {
            //CORE::QUAKEML::Event* evt = sdc->quakeML->getCurrentEventPtr();
            //CORE::QUAKEML::Origin* org = sdc->quakeML->getCurrentOriginPtr();
            CORE::TIME::QMLTimeMicroseconds sec1(CORE::TIME::MODULO_SEC);
            _v = vector;
            _t = ttime - _timeShift;
            if (file) {
                fprintf(file, "%.3lf", _t.toDouble());
                //for (auto&& val : vector) {
                //    fprintf(file, " %f", val);
                //}
                detPtr->process(vector.data());
                for (int i = 0; i < detPtr->VSize(); i++) {
                    fprintf(file, " %f", detPtr->output(i));
                }
                fprintf(file, "\n", detPtr->output(0), detPtr->output(2), detPtr->output(2));
            }
        }
    public:
        TWriteToFileDetTest(CORE::DATA::SharedDataContainer* sd, CORE::TIME::QMLTimeMicroseconds step = 100000LL) { sdc = sd;  file = NULL; _step = step; }
        ~TWriteToFileDetTest() { if (file) fclose(file); }
        void reset() override {
            if (file) fclose(file);
            if (fileName.empty()) {
                file = nullptr;
                detPtr.reset();
            }
            else {
                file = fopen(fileName.c_str(), "w");
                detPtr.reset(new CORE::Seismic_Precessing::TSLRNNFile(annName));
            }
        }
        void reset(const std::string& fname, const std::string& aname) { fileName = fname; annName = aname; reset(); }
        bool ready() override { return false; }
        bool get(std::vector<double>& v, CORE::TIME::QMLTime& t) override { v = _v; t = _t; return false; }
        CORE::TIME::QMLTime& time() override { return _t; }
        void setSta(const CORE::QUAKEML::WaveformStreamID& w) { _sta = w.stationCode; }

    };

    void ExportANNDetection::process(wxCommandEvent& ev) {
        std::string configName = outptPath + "/config/ANNDetectionConfig.xml";
        XMLFileNode configNode(configName.c_str());
        XMLNode inputsNode = configNode.child("ANN").child("inputs");
        std::string timeShift = inputsNode.child("method").attribute("time_shift");
        if (timeShift == "0") {
            processNoTimeShift(inputsNode);
        }
        else {
            processTimeShift(inputsNode);
        }
        //        throw CORE::EXCEPTIONS::CoreException("Cannot open configuration file '%s/config/ANNDetectionConfig.xml'", outptPath.c_str());
    }

    void ExportANNDetection::processDetTest(wxCommandEvent& ev) {
        std::string configName = outptPath + "/config/ANNDetectionConfig.xml";
        XMLFileNode configNode(configName.c_str());
        XMLNode inputsNode = configNode.child("ANN").child("inputs");
        std::string timeShift = inputsNode.child("method").attribute("time_shift");
        if (timeShift == "0") {
            processDetTestNoTimeShift(inputsNode, configNode);
        }
        else {
            //processDetTestTimeShift(inputsNode);
            return;
        }
    }

    void ExportANNDetection::processDetTest2(wxCommandEvent& ev) {
        std::string configName = outptPath + "/config/ANNDetectionConfig.xml";
        XMLFileNode configNode(configName.c_str());
        XMLNode inputsNode = configNode.child("ANN").child("inputs");
        std::string timeShift = inputsNode.child("method").attribute("time_shift");
        sdc->proposedPicks.push_back(CORE::QUAKEML::ProposePick("Detections"));
        try {
            XMLNode pickColour = configNode.child("colour");
            sdc->proposedPicks.back().penColour = sdc->proposedPicks.back().textColour = wxColour(
                static_cast<unsigned char>(std::stoi(pickColour.attribute("red"))),
                static_cast<unsigned char>(std::stoi(pickColour.attribute("green"))),
                static_cast<unsigned char>(std::stoi(pickColour.attribute("blue")))
            );
        }
        catch (...) {}
        if (timeShift == "0") {
            processDetTest2NoTimeShift(inputsNode, configNode);
        }
        else {
            //processDetTestTimeShift(inputsNode);
            return;
        }
    }

    void ExportANNDetection::processNoTimeShift(XMLNode& inputsNode) {
        std::string stepText = inputsNode.attribute("step");
        CORE::TIME::QMLTimeMicroseconds step = stoll(stepText);
        std::string path = outptPath + "/config/FilterBank/";
        TWriteToFile detproc(sdc, step);
        CORE::PLUGINS::IProcessPlugin* resumpling_plugin = dynamic_cast<IProcessPlugin*>(pd[0]->pluginPtrs[0]);
        if (resumpling_plugin == NULL) throw CORE::EXCEPTIONS::PluginException("Missing resampling process");
        CORE::TIME::QMLTime bt;
        CORE::TIME::QMLTime et;
        std::map<CORE::QUAKEML::WaveformStreamID, std::vector<ExANNDet> > stations;
        std::set<std::string> evtIds;
        for (auto&& st : sdc->seismicChannels) {
            auto mapRef = stations.insert(std::make_pair(st->channelName.getSta(), std::vector<ExANNDet>()));
            if (!mapRef.second) continue;
            if (!bt.exists() || bt < st->waveform.timeFrom()) {
                bt = st->waveform.timeFrom();
            }
            if (!et.exists() || bt < st->waveform.timeTo()) {
                et = st->waveform.timeTo();
            }
            CORE::DATA::SDC_lock_read ql(sdc, "ANN", 120);
            for (auto&& evt : sdc->quakeML->eventParameters->event_) {
                ExANNDet phs;
                auto oriPtr = CORE::QUAKEML::getOriginPtr(evt);
                if (oriPtr != nullptr) phs.evTime = oriPtr->time;
                phs.evtId = evt.publicID.resourceID;
                for (auto ph : evt.pick) {
                    if (ph.phaseHint && ph.waveformID.stationCode == st->channelName.stationCode && ph.time.value > bt && ph.time.value < et) {
                        if (ph.phaseHint->code == "P" || ph.phaseHint->code == "Pg") {
                            phs.P = ph.time;
                        }
                        if (ph.phaseHint->code == "S" || ph.phaseHint->code == "Sg") {
                            phs.S = ph.time;
                        }
                    }
                }
                if (phs.P || phs.S) {
                    mapRef.first->second.push_back(phs);
                    evtIds.insert(evt.publicID.resourceID);
                }

            }
        }
        for (auto&& sta : stations) {
            std::string fname = "";
            int nochan = 0;
            XMLNode configNode = inputsNode.child("Default_Station", false);
            for (XMLNode staNode = inputsNode.firstchild("Station"); !staNode.isNull(); staNode = inputsNode.nextchild("Station")) {
                if (staNode.attribute("sta_name", false) == sta.first.getStaName() || staNode.attribute("sta_names", false).find(sta.first.getStaName() + ";") != std::string::npos) {
                    sdc->logger << CORE::LOGGING::LL_REPORT << "Individual input parameters " + staNode.attribute("name", false);
                    configNode = staNode;
                    break;
                }
            }
            TFilterBankNoTimeShiftDetection filterbank(configNode, path, detproc);
            filterbank.setStep(step);
            filterbank.reset();
            bool allEvents = true;
            for (auto&& evtId : evtIds) {
                bool foundEvt = false;
                for (auto&& st : sta.second) {
                    if (st.evtId == evtId) {
                        foundEvt = true;
                        break;
                    }
                }
                if (!foundEvt) {
                    allEvents = false;
                    break;
                }
            }
            if (allEvents) {
                detproc.reset(outptPath + sta.first.getStaName() + bt.toString("%Y%m%d%H%M%S.dat"), &sta.second);
            }
            else {
                detproc.reset(outptPath + sta.first.getStaName() + bt.toString("%Y%m%d%H%M%S.tst"), &sta.second);
            }
            detproc.setSta(sta.first);
            std::shared_ptr<CORE::DATA::SeismicChannel> inchan[3];
            for (auto&& st : sdc->seismicChannels) {
                // Finding 3 channels
                if (sta.first == st->channelName.getSta()) {
                    switch (st->channelName.channelCode[2]) {
                    case 'Z':
                        inchan[0] = st;
                        nochan++;
                        break;
                    case 'N':
                        inchan[1] = st;
                        nochan++;
                        break;
                    case 'E':
                        inchan[2] = st;
                        nochan++;
                        break;
                    }
                }
            }
            if (nochan != 3 || !inchan[0] || !inchan[1] || !inchan[2]) {
                continue;
            }
            // Test if channels continues

            // Converts to regular samples and 3D samples
            for (int i = 0; i < 3; i++) {
                auto resumpling = resumpling_plugin->getProcess(*inchan[i]);
                inchan[i]->apply(*resumpling);
                if (resumpling->newChannel()->waveform.isEmpty()) continue;
                CORE::DATA::SingleCW* mch = dynamic_cast<CORE::DATA::SingleCW*>(resumpling->newChannel()->waveform.getWaveforms().front().get());
                filterbank.push(mch, inchan[i]->channelName.channelCode[2]);
            }
            for (int i = 0; i < 3; i++) {
                for (auto&& wf : inchan[i]->waveform.getWaveforms()) {
                    CORE::TIME::QMLTime stime = wf->timeFrom;
                    CORE::TIME::QMLTimeMicroseconds sstep = (wf->timeTo - wf->timeFrom) / wf->size();
                    CORE::DATA::SingleCW* mch = dynamic_cast<CORE::DATA::SingleCW*>(wf.get());
                    if (mch == nullptr) continue;
                    std::ofstream cf(outptPath + sta.first.getStaName() + bt.toString("%Y%m%d%H%M%S.") + inchan[i]->channelName.channelCode[2]);
                    if (!cf.is_open()) continue;
                    cf.precision(6);
                    for (auto samp : mch->samples) {
                        cf << std::fixed << stime.toDouble() << ' ' << std::scientific << samp << '\n';
                        stime += sstep;
                    }
                }

            }
            detproc.reset("", nullptr);
        }
    }

    void ExportANNDetection::processTimeShift(XMLNode& inputsNode) {
        std::string stepText = inputsNode.attribute("step");
        CORE::TIME::QMLTimeMicroseconds step = stoll(stepText);
        std::string path = outptPath + "/config/FilterBank/";
        TWriteToFile detproc(sdc, step);
        CORE::PLUGINS::IProcessPlugin* resumpling_plugin = dynamic_cast<IProcessPlugin*>(pd[0]->pluginPtrs[0]);
        if (resumpling_plugin == NULL) throw CORE::EXCEPTIONS::PluginException("Missing resampling process");
        CORE::TIME::QMLTime bt;
        CORE::TIME::QMLTime et;
        std::map<CORE::QUAKEML::WaveformStreamID, std::vector<ExANNDet> > stations;
        std::set<std::string> evtIds;
        for (auto&& st : sdc->seismicChannels) {
            auto mapRef = stations.insert(std::make_pair(st->channelName.getSta(), std::vector<ExANNDet>()));
            if (!bt.exists() || bt < st->waveform.timeFrom()) {
                bt = st->waveform.timeFrom();
            }
            if (!et.exists() || bt < st->waveform.timeTo()) {
                et = st->waveform.timeTo();
            }
			CORE::DATA::SDC_lock_read ql(sdc, "ANN", 120);
			for (auto&& evt : sdc->quakeML->eventParameters->event_) {
                ExANNDet phs;
                phs.evtId = evt.publicID.resourceID;
                for (auto ph : evt.pick) {
                    if (ph.phaseHint && ph.waveformID.stationCode == st->channelName.stationCode && ph.time.value > bt && ph.time.value < et) {
                        if (ph.phaseHint->code == "P" || ph.phaseHint->code == "Pg") {
                            phs.P = ph.time;
                        }
                        if (ph.phaseHint->code == "S" || ph.phaseHint->code == "Sg") {
                            phs.S = ph.time;
                        }
                    }
                }
                if (phs.P || phs.S) {
                    mapRef.first->second.push_back(phs);
                    evtIds.insert(evt.publicID.resourceID);
                }

            }
        }
        for (auto&& sta : stations) {
            std::string fname = "";
            int nochan = 0;
            XMLNode configNode = inputsNode.child("Default_Station", false);
            for (XMLNode staNode = inputsNode.firstchild("Station"); !staNode.isNull(); staNode = inputsNode.nextchild("Station")) {
                if (staNode.attribute("sta_name", false) == sta.first.getStaName() || staNode.attribute("sta_names", false).find(sta.first.getStaName() + ";") != std::string::npos) {
                    sdc->logger << CORE::LOGGING::LL_REPORT << "Individual input parameters " + staNode.attribute("name", false);
                    configNode = staNode;
                    break;
                }
            }
            TFilterBankTimeShiftDetection filterbank(configNode, path, detproc);
            filterbank.setStep(step);
            filterbank.reset();
            bool allEvents = true;
            for (auto&& evtId : evtIds) {
                bool foundEvt = false;
                for (auto&& st : sta.second) {
                    if (st.evtId == evtId) {
                        foundEvt = true;
                        break;
                    }
                }
                if (!foundEvt) {
                    allEvents = false;
                    break;
                }
            }
            if (allEvents) {
                detproc.reset(outptPath + sta.first.getStaName() + bt.toString("%Y%m%d%H%M%S.dat"), &sta.second);
            }
            else {
                detproc.reset(outptPath + sta.first.getStaName() + bt.toString("%Y%m%d%H%M%S.tst"), &sta.second);
            }
            detproc.setSta(sta.first);
            std::shared_ptr<CORE::DATA::SeismicChannel> inchan[3];
            for (auto&& st : sdc->seismicChannels) {
                // Finding 3 channels
                if (sta.first == st->channelName.getSta()) {
                    switch (st->channelName.channelCode[2]) {
                    case 'Z':
                        inchan[0] = st;
                        nochan++;
                        break;
                    case 'N':
                        inchan[1] = st;
                        nochan++;
                        break;
                    case 'E':
                        inchan[2] = st;
                        nochan++;
                        break;
                    }
                }
                if (nochan != 3 || !inchan[0] || !inchan[1] || !inchan[2]) {
                    continue;
                }
                // Test if channels continues
                filterbank.setInputStep(int(10000.0 / inchan[0]->channelParameters->sps() + 0.5) * 100);
                // Converts to regular samples and 3D samples
                for (int i = 0; i < 3; i++) {
                    auto resumpling = resumpling_plugin->getProcess(*inchan[i]);
                    inchan[i]->apply(*resumpling);
                    if (resumpling->newChannel()->waveform.isEmpty()) continue;
                    CORE::DATA::SingleCW* mch = dynamic_cast<CORE::DATA::SingleCW*>(resumpling->newChannel()->waveform.getWaveforms().front().get());
                    filterbank.push(mch, inchan[i]->channelName.channelCode[2]);
                }
                for (int i = 0; i < 3; i++) {
                    for (auto&& wf : inchan[i]->waveform.getWaveforms()) {
                        CORE::TIME::QMLTime stime = wf->timeFrom;
                        CORE::TIME::QMLTimeMicroseconds sstep = (wf->timeTo - wf->timeFrom) / wf->size();
                        CORE::DATA::SingleCW* mch = dynamic_cast<CORE::DATA::SingleCW*>(wf.get());
                        if (mch == nullptr) continue;
                        std::ofstream cf(outptPath + sta.first.getStaName() + bt.toString("%Y%m%d%H%M%S.") + inchan[i]->channelName.channelCode[2]);
                        if (!cf.is_open()) continue;
                        cf.precision(6);
                        for (auto samp : mch->samples) {
                            cf << std::fixed << stime.toDouble() << ' ' << std::scientific << samp << '\n';
                            stime += sstep;
                        }
                    }

                }
                detproc.reset("", nullptr);
            }
        }
    }

    void ExportANNDetection::processDetTestNoTimeShift(XMLNode& inputsNode, XMLNode& rootNode) {
        std::string stepText = inputsNode.attribute("step");
        CORE::TIME::QMLTimeMicroseconds step = stoll(stepText);
        std::string path = outptPath + "/config/FilterBank/";
        if (rootNode.child("ANN").attribute("type") != "SLRNN")  throw CORE::EXCEPTIONS::PluginException("Wrong ANN type");
        std::string annName = outptPath + "config/" + rootNode.child("ANN").attribute("file");
        TWriteToFileDetTest detproc(sdc, step);
        CORE::PLUGINS::IProcessPlugin* resumpling_plugin = dynamic_cast<IProcessPlugin*>(pd[0]->pluginPtrs[0]);
        if (resumpling_plugin == NULL) throw CORE::EXCEPTIONS::PluginException("Missing resampling process");
        CORE::TIME::QMLTime bt;
        CORE::TIME::QMLTime et;
        std::map<CORE::QUAKEML::WaveformStreamID, std::vector<ExANNDet> > stations;
        for (auto&& st : sdc->seismicChannels) {
            auto mapRef = stations.insert(std::make_pair(st->channelName.getSta(), std::vector<ExANNDet>()));
            if (!mapRef.second) continue;
            if (!bt.exists() || bt < st->waveform.timeFrom()) {
                bt = st->waveform.timeFrom();
            }
            if (!et.exists() || bt < st->waveform.timeTo()) {
                et = st->waveform.timeTo();
            }
        }
        for (auto&& sta : stations) {
            std::string fname = "";
            int nochan = 0;
            XMLNode configNode = inputsNode.child("Default_Station", false);
            for (XMLNode staNode = inputsNode.firstchild("Station"); !staNode.isNull(); staNode = inputsNode.nextchild("Station")) {
                if (staNode.attribute("sta_name", false) == sta.first.getStaName() || staNode.attribute("sta_names", false).find(sta.first.getStaName() + ";") != std::string::npos) {
                    sdc->logger << CORE::LOGGING::LL_REPORT << "Individual iput parameters " + staNode.attribute("name", false);
                    configNode = staNode;
                    break;
                }
            }
            TFilterBankNoTimeShiftDetection filterbank(configNode, path, detproc);
            filterbank.setStep(step);
            filterbank.reset();
            detproc.reset(outptPath + sta.first.getStaName() + bt.toString("%Y%m%d%H%M%S.wyn"), annName);
            detproc.setSta(sta.first);
            std::shared_ptr<CORE::DATA::SeismicChannel> inchan[3];
            for (auto&& st : sdc->seismicChannels) {
                // Finding 3 channels
                if (sta.first == st->channelName.getSta()) {
                    switch (st->channelName.channelCode[2]) {
                    case 'Z':
                        inchan[0] = st;
                        nochan++;
                        break;
                    case 'N':
                        inchan[1] = st;
                        nochan++;
                        break;
                    case 'E':
                        inchan[2] = st;
                        nochan++;
                        break;
                    }
                }
            }
            if (nochan != 3 || !inchan[0] || !inchan[1] || !inchan[2]) {
                continue;
            }
            // Test if channels continues

            // Converts to regular samples and 3D samples
            for (int i = 0; i < 3; i++) {
                auto resumpling = resumpling_plugin->getProcess(*inchan[i]);
                inchan[i]->apply(*resumpling);
                if (resumpling->newChannel()->waveform.isEmpty()) continue;
                CORE::DATA::SingleCW* mch = dynamic_cast<CORE::DATA::SingleCW*>(resumpling->newChannel()->waveform.getWaveforms().front().get());
                filterbank.push(mch, inchan[i]->channelName.channelCode[2]);
            }
            for (int i = 0; i < 3; i++) {
                for (auto&& wf : inchan[i]->waveform.getWaveforms()) {
                    CORE::TIME::QMLTime stime = wf->timeFrom;
                    CORE::TIME::QMLTimeMicroseconds sstep = (wf->timeTo - wf->timeFrom) / wf->size();
                    CORE::DATA::SingleCW* mch = dynamic_cast<CORE::DATA::SingleCW*>(wf.get());
                    if (mch == nullptr) continue;
                    std::ofstream cf(outptPath + sta.first.getStaName() + bt.toString("%Y%m%d%H%M%S.") + inchan[i]->channelName.channelCode[2]);
                    if (!cf.is_open()) continue;
                    cf.precision(6);
                    for (auto samp : mch->samples) {
                        cf << std::fixed << stime.toDouble() << ' ' << std::scientific << samp << '\n';
                        stime += sstep;
                    }
                }

            }
            detproc.reset("", "");
        }
    }

    void ExportANNDetection::processDetTest2NoTimeShift(XMLNode& inputsNode, XMLNode& rootNode) {
//        sdc->proposedPicks.clear();
        std::string stepText = inputsNode.attribute("step");
        CORE::TIME::QMLTimeMicroseconds step = stoll(stepText);
        std::string path = outptPath + "/config/FilterBank/";
        if (rootNode.child("ANN").attribute("type") != "SLRNN")  throw CORE::EXCEPTIONS::PluginException("Wrong ANN type");
        XMLNode annNode = rootNode.child("ANN");
        std::string annGenericName = outptPath + "config/" + annNode.attribute("file");
        TDetTest detproc(sdc, step);
        CORE::PLUGINS::IProcessPlugin* resumpling_plugin = dynamic_cast<IProcessPlugin*>(pd[0]->pluginPtrs[0]);
        if (resumpling_plugin == NULL) throw CORE::EXCEPTIONS::PluginException("Missing resampling process");
        CORE::TIME::QMLTime bt;
        CORE::TIME::QMLTime et;
        std::map<CORE::QUAKEML::WaveformStreamID, std::vector<ExANNDet> > stations;
        for (auto&& st : sdc->seismicChannels) {
            auto mapRef = stations.insert(std::make_pair(st->channelName.getSta(), std::vector<ExANNDet>()));
            if (!mapRef.second) continue;
            if (!bt.exists() || bt < st->waveform.timeFrom()) {
                bt = st->waveform.timeFrom();
            }
            if (!et.exists() || bt < st->waveform.timeTo()) {
                et = st->waveform.timeTo();
            }
        }
        for (auto&& sta : stations) {
            sdc->logger.c_log(CORE::LOGGING::LL_REPORT, "Detection %s: ",sta.first.getStaName().c_str());
            int nochan = 0;
            XMLNode configNode = inputsNode.child("Default_Station", false);
            for (XMLNode staNode = inputsNode.firstchild("Station"); !staNode.isNull(); staNode = inputsNode.nextchild("Station")) {
                if (staNode.attribute("sta_name", false) == sta.first.getStaName() || staNode.attribute("sta_names", false).find(sta.first.getStaName() + ";") != std::string::npos) {
                    sdc->logger << CORE::LOGGING::LL_REPORT << "Individual input parameters " + staNode.attribute("name", false);
                    configNode = staNode;
                    break;
                }
            }
            TFilterBankNoTimeShiftDetection filterbank(configNode, path, detproc);
            filterbank.setStep(step);
            filterbank.reset();
            std::string annName = annGenericName;
            for (XMLNode annStaNode = annNode.firstchild("STA"); !annStaNode.isNull(); annStaNode = annNode.nextchild("STA")) {
                if (annStaNode.attribute("name") == sta.first.getStaName()) {
                    annName = outptPath + "config/" + annStaNode.attribute("file");
                    break;
                }
            }
            detproc.reset(annName);
            detproc.setSta(sta.first);
            std::shared_ptr<CORE::DATA::SeismicChannel> inchan[3];
            for (auto&& st : sdc->seismicChannels) {
                // Finding 3 channels
                if (sta.first == st->channelName.getSta()) {
                    switch (st->channelName.channelCode[2]) {
                    case 'Z':
                        inchan[0] = st;
                        nochan++;
                        break;
                    case 'N':
                        inchan[1] = st;
                        nochan++;
                        break;
                    case 'E':
                        inchan[2] = st;
                        nochan++;
                        break;
                    }
                }
            }
            if (nochan != 3 || !inchan[0] || !inchan[1] || !inchan[2]) {
                continue;
            }
            // Test if channels continues

            // Converts to regular samples and 3D samples
            std::list<PureSingleCW> continuesPeriodList;
            for (int i = 0; i < 3; i++) {
                auto resumpling = resumpling_plugin->getProcess(*inchan[i]);
                inchan[i]->apply(*resumpling);
                if (resumpling->newChannel()->waveform.isEmpty()) continue;
                for (auto& wf : resumpling->newChannel()->waveform.getWaveforms()) {
                    continuesPeriodList.push_back(PureSingleCW(wf, inchan[i]->channelName.channelCode[2]));
//                    if (mch) filterbank.push(mch, inchan[i]->channelName.channelCode[2]);
                }
            }
            continuesPeriodList.sort(compSingleCW);
            for (auto& m : continuesPeriodList) {
                CORE::DATA::SingleCW* mch = dynamic_cast<CORE::DATA::SingleCW*>(m.d.get());
                if (mch) filterbank.push(mch, m.c);
            }
            detproc.reset("");
        }
        sdc->updateWaveformKeepEvent();
    }

    void ExportANNDetection::setOutput(wxCommandEvent& ev) {
        wxDirDialog dirDialog(sdc->mainWindow, wxT("Set output path for exporting ANN detection inputs"), outptPath, wxDD_NEW_DIR_BUTTON);
        if (dirDialog.ShowModal() == wxID_OK) {
            outptPath = dirDialog.GetPath().ToStdString() + "\\";
        }
    }

    ArcLinkPlugin* ExportANNDetection::preproc() {
        if (pd.size() < 2) {
            sdc->logger << CORE::LOGGING::LL_ERROR << "Missing arclink plugin";
            return nullptr;
        }
        ArcLinkPlugin* arclink = dynamic_cast<ArcLinkPlugin*>(pd[1]->pluginPtrs[0]);
        if (arclink == NULL) {
            sdc->logger << CORE::LOGGING::LL_ERROR << "Wrong arclink plugin";
            return nullptr;
        }
        return arclink;
    }

    void ExportANNDetection::listNoises(wxCommandEvent& ev) {
        try {
            map<CORE::QUAKEML::WaveformStreamID, int> stas;
            ArcLinkPlugin* arclink = preproc();
            if (arclink == NULL) throw CORE::EXCEPTIONS::CoreException("Wrong arclink plugin");
            char comp[3] = { 'Z', 'N', 'E' };
            std::ifstream ifile("noises.txt");
            if (!ifile.good()) throw CORE::EXCEPTIONS::CoreException("Can not read noises.txt");
            std::string buf;
            CORE::TIME::QMLTimeMicroseconds before;
            CORE::TIME::QMLTimeMicroseconds after;
            if (!std::getline(ifile, buf))  throw CORE::EXCEPTIONS::CoreException("Can not read time before");
            before = stoll(buf) * CORE::TIME::MODULO_SEC;
            if (!std::getline(ifile, buf))  throw CORE::EXCEPTIONS::CoreException("Can not read time after");
            after = stoll(buf) * CORE::TIME::MODULO_SEC;
            int val = 0;
            while (std::getline(ifile, buf)) {
                int space = buf.find(' ');
                CORE::QUAKEML::WaveformStreamID stream = CORE::QUAKEML::wfFromString(buf.substr(0, space));
                stream.channelCode[2] = '*';
                auto ref = stas.insert(std::make_pair(stream, val));
                ref.first->second++;
            }
            for (auto&& st : stas) {
                sdc->logger << CORE::LOGGING::LL_REPORT << st.first.getStaName() + std::string(": ") + std::to_string(st.second);
            }
        }
        catch (const std::exception& e) {
            sdc->logger << CORE::LOGGING::LL_ERROR << e.what();
        }
    }

    void ExportANNDetection::getNoise(wxCommandEvent& ev) {
        const char comp[3] = { 'Z', 'N', 'E' };
        ArcLinkSettings als(sdc);
        auto waveforms = als.waveforms;
        auto waveform_duration = als.waveform_duration;
        try {
            ArcLinkPlugin* arclink = preproc();
            if (arclink == NULL) throw CORE::EXCEPTIONS::CoreException("Wrong arclink plugin");
            als.waveforms.clear();
            for (char c : comp) {
                stream.channelCode[2] = c;
                als.waveforms.push_back(stream);
            }
            als.waveform_begin = noiseTime - before;
            als.waveform_end = noiseTime + after;
            als.waveform_duration = (als.waveform_end.toDouble() - als.waveform_begin.toDouble()) / 60.0;
            arclink->arclinkRequest(als, false);
        }
        catch (const std::exception& e) {
            sdc->logger << e.what();
        }
        als.waveforms = waveforms;
        als.waveform_duration = waveform_duration;
    }
    
    void ExportANNDetection::doDetection(wxCommandEvent& ev) {
        std::vector<CORE::QUAKEML::WaveformStreamID> streams;
        streams.push_back(CORE::QUAKEML::WaveformStreamID("Netwok (required)", "Station (required)", "Location (not required)", "Stream (required 3 letetters, last leter can by any)"));
        streams.push_back(CORE::QUAKEML::WaveformStreamID("..", "....", "", "..."));
        if (!InitParameters(streams, "DetectionStreams", sdc)) return;

        std::string configName = outptPath + "/config/ANNDetectionConfig.xml";
        XMLFileNode rootNode(configName.c_str());
        XMLNode inputsNode = rootNode.child("ANN").child("inputs");
        std::string timeShift = inputsNode.child("method").attribute("time_shift");
        if (timeShift != "0") {
            sdc->logger.c_log(CORE::LOGGING::LL_REPORT, "Time shift detection not implemented");
            return;
        }

        CORE::GUI::Time2EntryDialog dialog(sdc->mainWindow, wxID_ANY, "Days of detetion", "Define priod for detection", CORE::GUI::ACCURACY_DAYS);
        if (dialog.ShowModal() != wxID_OK) return;
        CORE::TIME::QMLTime detFrom = dialog.GetTimeFrom();
        CORE::TIME::QMLTime detTo = dialog.GetTimeTo();

        wxFileDialog saveFileDialog(sdc->mainWindow, _("Detection file"), outptPath, "", "*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (saveFileDialog.ShowModal() == wxID_CANCEL) return;

        static CORE::TIME::QMLTimeMicroseconds lday = 86400000000LL;
        static CORE::TIME::QMLTimeMicroseconds lpre = 300000000LL;
        static CORE::TIME::QMLTimeMicroseconds lpost = 87000000000LL;
        
        int noDays = int((detTo - detFrom) / lday + 1);
        int idx = 0;
        ArcLinkSettings als(sdc);
        const char comp[3] = { 'Z', 'N', 'E' };
        auto waveforms = als.waveforms;
        auto waveform_duration = als.waveform_duration;
        bool on = false;
        int votingMax = 0;
        try {
            std::string staDetFileName = saveFileDialog.GetPath().ToStdString();
            AUTOFILE detectionFile(staDetFileName.c_str(), "w");
            boost::replace_all(staDetFileName, ".txt", ".det");
            AUTOFILE detectionVoteFile(staDetFileName.c_str(), "w");
            boost::replace_all(staDetFileName, ".det", ".sdet");
            ArcLinkPlugin* arclink = preproc();
            std::string stepText = inputsNode.attribute("step");
            CORE::TIME::QMLTimeMicroseconds step = stoll(stepText);
            std::string path = outptPath + "/config/FilterBank/";
            if (rootNode.child("ANN").attribute("type") != "SLRNN")  throw CORE::EXCEPTIONS::PluginException("Wrong ANN type");
            XMLNode annNode = rootNode.child("ANN");
            std::string annGenericName = outptPath + "config/" + annNode.attribute("file");
            TDoOffLineDetection detproc(sdc, staDetFileName, step);
            CORE::PLUGINS::IProcessPlugin* resumpling_plugin = dynamic_cast<IProcessPlugin*>(pd[0]->pluginPtrs[0]);
            if (resumpling_plugin == NULL) throw CORE::EXCEPTIONS::PluginException("Missing resampling process");


            if (arclink == NULL) throw CORE::EXCEPTIONS::CoreException("Wrong arclink plugin");

            sdc->proposedPicks.push_back(CORE::QUAKEML::ProposePick("StaDetections"));
            sdc->proposedPicks.back().penColour = *wxLIGHT_GREY;
            auto staDetection = sdc->proposedPicks.back().picks;
            sdc->proposedPicks.push_back(CORE::QUAKEML::ProposePick("EventDetections"));
            sdc->proposedPicks.back().penColour = *wxRED;
            auto eventDetection = sdc->proposedPicks.back().picks;
            CORE::QUAKEML::Pick staDetPick;
            staDetPick.phaseHint = Phase();
            CORE::QUAKEML::Pick eventDetPick;
            eventDetPick.phaseHint = Phase();
            for (CORE::TIME::QMLTime readFrom = detFrom; readFrom < detTo; readFrom += lday) {
                staDetection->clear();
                eventDetection->clear();
                als.waveform_begin = readFrom - lpre;
                als.waveform_end = readFrom + lpost;
                eventDetection->clear();
                for (auto& stream1 : streams) {
                    std::string annName = annGenericName;
                    for (XMLNode annStaNode = annNode.firstchild("STA"); !annStaNode.isNull(); annStaNode = annNode.nextchild("STA")) {
                        if (annStaNode.attribute("name") == stream1.getStaName()) {
                            annName = outptPath + "config/" + annStaNode.attribute("file");
                            break;
                        }
                    }
                    sdc->logger.c_log(CORE::LOGGING::LL_REPORT, "Process detection stream '%s' from %s to %s", stream1.getChannelName().c_str(), als.waveform_begin.toString().c_str(), als.waveform_end.toString().c_str());
                    als.waveforms.clear();
                    for (char c : comp) {
                        stream1.channelCode[2] = c;
                        als.waveforms.push_back(stream1);
                    }
                    try {
                        als.waveform_duration = (als.waveform_end.toDouble() - als.waveform_begin.toDouble()) / 60.0;
                        arclink->arclinkRequest(als, false);
                        staDetection->clear();
                        sdc->updateWaveformKeepEvent();
                        Sleep(200);

                        int nochan = 0;
                        detproc.setSta(stream1.getSta());
                        std::shared_ptr<CORE::DATA::SeismicChannel> inchan[3];
                        for (auto&& st : sdc->seismicChannels) {
                            if (stream1.getSta() == st->channelName.getSta()) {
                                switch (st->channelName.channelCode[2]) {
                                case 'Z':
                                    inchan[0] = st;
                                    nochan++;
                                    break;
                                case 'N':
                                    inchan[1] = st;
                                    nochan++;
                                    break;
                                case 'E':
                                    inchan[2] = st;
                                    nochan++;
                                    break;
                                }
                            }
                        }
                        if (nochan != 3 || !inchan[0] || !inchan[1] || !inchan[2]) {
                            throw CORE::EXCEPTIONS::CoreException("Missing 3 channels of %s", stream1.getStaName().c_str());
                        }
                        // Test if channels continues

                        // Converts to regular samples and 3D samples
                        XMLNode configNode = inputsNode.child("Default_Station", false);
                        for (XMLNode staNode = inputsNode.firstchild("Station"); !staNode.isNull(); staNode = inputsNode.nextchild("Station")) {
                            if (staNode.attribute("sta_name", false) == stream1.getStaName() || staNode.attribute("sta_names", false).find(stream1.getStaName() + ";") != std::string::npos) {
                                sdc->logger << CORE::LOGGING::LL_REPORT << "Individual input parameters " + staNode.attribute("name", false);
                                configNode = staNode;
                                break;
                            }
                        }
                        TFilterBankNoTimeShiftDetection filterbank(configNode, path, detproc);
                        filterbank.setStep(step);
                        filterbank.reset();
                        detproc.reset(annName);
                        std::list<PureSingleCW> continuesPeriodList;
                        for (int i = 0; i < 3; i++) {
                            auto resumpling = resumpling_plugin->getProcess(*inchan[i]);
                            inchan[i]->apply(*resumpling);
                            if (resumpling->newChannel()->waveform.isEmpty()) continue;
                            for (auto& wf : resumpling->newChannel()->waveform.getWaveforms()) {
                                continuesPeriodList.push_back(PureSingleCW(wf, inchan[i]->channelName.channelCode[2]));
                            }
                        }
                        continuesPeriodList.sort(compSingleCW);
                        for (auto& m : continuesPeriodList) {
                            CORE::DATA::SingleCW* mch = dynamic_cast<CORE::DATA::SingleCW*>(m.d.get());
                            if (mch) filterbank.push(mch, m.c);
                        }

                        for (auto& det : detproc.list) {
                            if (det.sta == stream1.getSta()) {
                                staDetPick.phaseHint->code = det.on ? "d+" : "d-";
                                staDetPick.waveformID = det.sta;
                                staDetPick.time = det.time;
                                staDetection->push_back(staDetPick);
                            }
                        }
                        sdc->updateWaveformKeepEvent();
                        Sleep(2000);
                    }
                    catch (const std::exception& e) {
                        sdc->logger << CORE::LOGGING::LL_REPORT << e.what();
                    }
                }
                progress = idx * 100 / noDays;
            }
            detproc.list.sort();
            std::map<CORE::QUAKEML::WaveformStreamID, bool> staMap;
            for (auto& det : detproc.list) {
                auto ref = staMap.find(det.sta);
                if (ref == staMap.end()) {
                    staMap.insert(std::make_pair(det.sta, det.on));
                }
                else {
                    ref->second = det.on;
                }
                fprintf(detectionFile, "# %s:", det.time.toString().c_str());
                int voting = 0;
                for (auto& sp : staMap) {
                    if (sp.second) voting++;
                    fprintf(detectionFile, "%d(%s)", int(sp.second),sp.first.getStaName().c_str());
                }

                fprintf(detectionFile, "\n");
                if (voting >= 2) {
                    if (!on) {
                        fprintf(detectionFile, "ON %s\n", det.time.toString().c_str());
                        fprintf(detectionVoteFile, "ON %s\n", det.time.toString().c_str());
                        on = true;
                        //eventDetPick.phaseHint->code = "ON";
                        //eventDetPick.waveformID = det.sta;
                        //eventDetPick.time = det.time;
                        //eventDetection->push_back(eventDetPick);
                    }
                    if (votingMax < voting) votingMax = voting;
                }
                else {
                    if (on) {
                        fprintf(detectionFile, "OFF %s %d\n", det.time.toString().c_str(), votingMax);
                        fprintf(detectionVoteFile, "OFF %s %d\n", det.time.toString().c_str(), votingMax);
                        votingMax = 0;
                        on = false;
                        //eventDetPick.phaseHint->code = "OFF";
                        //eventDetPick.waveformID = det.sta;
                        //eventDetPick.time = det.time;
                        //eventDetection->push_back(eventDetPick);
                    }
                }
            }
            sdc->updateWaveformKeepEvent();
//            fflush(detectionFile);
            Sleep(2000);
        }
        catch (const std::exception& e) {
            sdc->logger << CORE::LOGGING::LL_ERROR << e.what();
        }
        als.waveforms = waveforms;
        als.waveform_duration = waveform_duration;
    }
    
    void ExportANNDetection::doViewList(wxCommandEvent& ev) {
        wxFileDialog openFileDialog(sdc->mainWindow, _("Detection file"), "", "", "*.sdet", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (openFileDialog.ShowModal() == wxID_CANCEL) return;
        std::string staDetFileName = openFileDialog.GetPath().ToStdString();
        std::ifstream file(staDetFileName);
        if (!file) return;
        std::string line;
        CORE::QUAKEML::WaveformStreamID station;
        CORE::TIME::QMLTime time;
        sdc->proposedPicks.push_back(CORE::QUAKEML::ProposePick("StaDetections"));
        sdc->proposedPicks.back().penColour = *wxBLUE;
        auto staDetection = sdc->proposedPicks.back().picks;
        CORE::QUAKEML::Pick vpick;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            if (line.substr(0, 2) == "ON") {
                vpick.waveformID = wfFromString(line.substr(31));
                vpick.time.value = CORE::TIME::QMLTime(line.substr(3, 27), "%Y-%m-%dT%H:%M:%S%f");
                vpick.phaseHint = CORE::QUAKEML::Phase("dB");
                staDetection->push_back(vpick);

            }
            else if (line.substr(0, 3) == "OFF") {
                vpick.waveformID = wfFromString(line.substr(32, line.find("/") - 32));
                vpick.time.value = CORE::TIME::QMLTime(line.substr(4, 28), "%Y-%m-%dT%H:%M:%S%f");
                vpick.phaseHint = CORE::QUAKEML::Phase("eB");
                staDetection->push_back(vpick);
            }
        }
    }
    
    void ExportANNDetection::doVotingList(wxCommandEvent& ev) {
        wxFileDialog openFileDialog(sdc->mainWindow, _("Detection file"), "", "", "*.sdet", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (openFileDialog.ShowModal() == wxID_CANCEL) return;
        std::string staDetFileName = openFileDialog.GetPath().ToStdString();
        std::ifstream file(staDetFileName);
        if (!file) return;
        boost::replace_all(staDetFileName, ".sdet", ".txt");
        AUTOFILE detectionFile(staDetFileName.c_str(), "w");
        if (!detectionFile.opened()) return;
        boost::replace_all(staDetFileName, ".txt", ".det");
        AUTOFILE detectionVoteFile(staDetFileName.c_str(), "w");
        if (!detectionVoteFile.opened()) return;
        std::list<TDetectionList> list;
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            if (line.substr(0, 2) == "ON") {
                list.push_back(TDetectionList(true, wfFromString(line.substr(31)), CORE::TIME::QMLTime(line.substr(3, 27), "%Y-%m-%dT%H:%M:%S%f")));

            }
            else if (line.substr(0, 3) == "OFF") {
                list.push_back(TDetectionList(false, wfFromString(line.substr(32, line.find("/")-32)), CORE::TIME::QMLTime(line.substr(4, 28), "%Y-%m-%dT%H:%M:%S%f")));
            }
        }
        list.sort();
        bool on = false;
        int votingMax = 0;
        std::map<CORE::QUAKEML::WaveformStreamID, bool> staMap;
        for (auto& det : list) {
            auto ref = staMap.find(det.sta);
            if (ref == staMap.end()) {
                staMap.insert(std::make_pair(det.sta, det.on));
            }
            else {
                ref->second = det.on;
            }
            fprintf(detectionFile, "# %s:", det.time.toString().c_str());
            int voting = 0;
            for (auto& sp : staMap) {
                if (sp.second) voting++;
                fprintf(detectionFile, "%d(%s)", int(sp.second), sp.first.getStaName().c_str());
            }

            fprintf(detectionFile, "\n");
            //if (voting >= 2) {
            if (voting) {
                if (!on) {
                    fprintf(detectionFile, "ON %s\n", det.time.toString().c_str());
                    fprintf(detectionVoteFile, "ON %s\n", det.time.toString().c_str());
                    on = true;
                }
                if (votingMax < voting) votingMax = voting;
            }
            else {
                if (on) {
                    fprintf(detectionFile, "OFF %s %d\n", det.time.toString().c_str(), votingMax);
                    fprintf(detectionVoteFile, "OFF %s %d\n", det.time.toString().c_str(), votingMax);
                    votingMax = 0;
                    on = false;
                }
            }
        }
    }

    void ExportANNDetection::processNoisesList(wxCommandEvent& ev) {
        try {
            wxFileDialog openFileDialog(sdc->mainWindow, _("Open noises list"), "", "", "Noises text files (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
            if (openFileDialog.ShowModal() == wxID_CANCEL) return;
            std::ifstream ifile(openFileDialog.GetPath().ToStdString());
            if (!ifile.good()) throw CORE::EXCEPTIONS::CoreException("Can not read noises.txt");
            std::string buf;
            if (!std::getline(ifile, buf))  throw CORE::EXCEPTIONS::CoreException("Can not read time before");
            before = stoll(buf) * CORE::TIME::MODULO_SEC;
            if (!std::getline(ifile, buf))  throw CORE::EXCEPTIONS::CoreException("Can not read time after");
            after = stoll(buf) * CORE::TIME::MODULO_SEC;
            //std::unique_ptr<PluginThread>plugin(new PluginThread(NULL, boost::bind(&ExportANNDetection::getNoise, this, ev), sdc));
            //if (plugin->Create() != wxTHREAD_NO_ERROR) throw CORE::EXCEPTIONS::CoreException("Can not create thread");
            int line = 0;
            while (std::getline(ifile, buf)) {
                int space = buf.find(' ');
                noiseTime = CORE::TIME::QMLTime(buf.substr(space + 1), "%Y-%m-%dT%H:%M:%S.%usZ");
                stream = CORE::QUAKEML::wfFromString(buf.substr(0, space));
                sdc->logger.c_log(CORE::LOGGING::LL_REPORT, "%4d: '%s'", ++line, buf);
                getNoise(ev);
                //sdc->pluginRunning = true;
                //plugin->Run();
                //do {
                //    sdc->logger.c_log(CORE::LOGGING::LL_DEBUG, "Waiting for noise");
                Sleep(200);
                //} while (sdc->pluginRunning);
                process(ev);
            }
        }
        catch (const std::exception& e) {
            sdc->logger << e.what();
        }
    }

    void ExportANNDetection::saveNoise(wxCommandEvent& ev) {
        std::ofstream ofile("noises.txt", std::ofstream::out | std::ofstream::app);
        ofile << ev.GetString() << std::endl;
    }

    void ExportANNDetection::saveUndetected(wxCommandEvent& ev) {
        std::ofstream ofile("undetected.txt", std::ofstream::out | std::ofstream::app);
        ofile << ev.GetString() << std::endl;
    }

    bool ExportANNDetection::evtDetected(CORE::QUAKEML::Event& evt, const TDetection& detection, int votesNumber) {
        const CORE::TIME::QMLTimeMicroseconds s20 = 20000000LL;
        const CORE::TIME::QMLTimeMicroseconds s30 = 30000000LL;
        auto oriPtr = CORE::QUAKEML::getOriginPtr(evt);
        if (!evt.pick.empty()) {
            for (auto& pick : evt.pick) {
                if (detection.endTime < pick.time.value) continue;
                if (detection.beginTime > pick.time.value + s20) continue;
                if (detection.votesNumber >= votesNumber) return true;
            }
        }
        else if (oriPtr != nullptr) {
            if (detection.endTime > oriPtr->time.value && detection.beginTime > oriPtr->time.value + s30) {
                return true;
            }
        }
        else {
            sdc->logger.c_log(CORE::LOGGING::LL_WARNING, "Event '%s' has neither origin nor picks", evt.publicID.resourceID.c_str());
        }
        return false;
    }
    struct TDoDetectedSignals {
        template<class Archive> void serialize(Archive & ar, const unsigned int version) {
            ar & BOOST_SERIALIZATION_NVP(before);
            ar & BOOST_SERIALIZATION_NVP(after);
            ar & BOOST_SERIALIZATION_NVP(path_buf);
        }
        CORE::TIME::QMLTimeMicroseconds before = 60000000LL;
        CORE::TIME::QMLTimeMicroseconds after=60000000LL;
        std::string path_buf = "/buf/";
    };
    void ExportANNDetection::doDetectedSignals(wxCommandEvent& ev) {
        ArcLinkPlugin* arclink = preproc();
        if (arclink == NULL) throw CORE::EXCEPTIONS::CoreException("Wrong arclink plugin");
        TDoDetectedSignals par;
        const unsigned MaxVoted = 5;
        if (!InitParameters(par, "DoDetectedSignals", sdc)) return;
        progress = 0;
        wxFileDialog openFileDialog(sdc->mainWindow, _("Detection file"), outptPath, "", "*.det", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (openFileDialog.ShowModal() == wxID_CANCEL) return;
        TLoadDetection detected(openFileDialog.GetPath().ToStdString());
        if (detected.list.empty()) {
            sdc->logger << CORE::LOGGING::LL_WARNING << "No detections";
            return;
        }
        std::vector<unsigned> votedDetections(MaxVoted + 1 - 2, 0);
        for (auto& detection : detected.list) {
            for (unsigned vn = 2; vn < MaxVoted ; vn++) {
                unsigned arrIndex = vn - 2;
                if (detection.votesNumber >= vn) {
                    votedDetections[arrIndex]++;
                }
            }
        }
        wxArrayString choices;
        for (unsigned vn = 2; vn < MaxVoted; vn++) {
            unsigned arrIndex = vn - 2;
            wxString msg;
            msg.Printf("Min %d stations: %d events", vn, votedDetections[arrIndex]);
            choices.Add(msg);
        }
        wxSingleChoiceDialog detDialog(sdc->mainWindow, "Choose nuber of stations", "Signals voting",choices);
        detDialog.SetSelection(1);
        if (detDialog.ShowModal() != wxID_OK) return;
        unsigned votesNumber = detDialog.GetSelection() + 2;
        unsigned index = 1;
        unsigned noEvents = votedDetections[detDialog.GetSelection()];
        for (auto& detection : detected.list) {
            if (detection.votesNumber >= votesNumber) {
                sdc->logger.c_log(CORE::LOGGING::LL_REPORT, "Load event %d/%d, %s-%s/%d", index, noEvents, detection.beginTime.toString("%Y-%m-%d %H:%M:%S").c_str(), detection.endTime.toString("%Y-%m-%d %H:%M:%S").c_str(), detection.votesNumber);
                std::unique_ptr<ArcLinkSettings> als(new ArcLinkSettings(sdc));
                std::string path_buf = als->path_buf;
                auto waveform_duration = als->waveform_duration;
                try {
                    als->waveform_begin = detection.beginTime - par.before;
                    als->waveform_end = detection.endTime + par.after;
                    als->path_buf = par.path_buf;
                    als->waveform_duration = (als->waveform_end.toDouble() - als->waveform_begin.toDouble()) / 60.0;

                    als.reset();
                    //arclink->arclinkRequest(als, false);
                    arclink->processData(false, false, ev);
                }
                catch (...) {
                    sdc->logger.c_log(CORE::LOGGING::LL_WARNING, "Can not get event %d/%d, %s-%s/%d", index, noEvents, detection.beginTime.toString("%Y-%m-%d %H:%M:%S").c_str(), detection.endTime.toString("%Y-%m-%d %H:%M:%S").c_str(), detection.votesNumber);

                }
                als.reset(new ArcLinkSettings(sdc));
                als->path_buf = path_buf;
                als->waveform_duration = waveform_duration;
                progress = 100 * index++ / noEvents;
            }
        }
    }
    void ExportANNDetection::testDetection(wxCommandEvent& ev) {
        progress = 0;
        wxFileDialog openFileDialog(sdc->mainWindow, _("Detection file"), outptPath, "", "*.det", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (openFileDialog.ShowModal() == wxID_CANCEL) return;
        progress = -1;
        TLoadDetection detected(openFileDialog.GetPath().ToStdString());
        if (detected.list.empty()) {
            sdc->logger << CORE::LOGGING::LL_WARNING << "No detections";
            return;
        }
        int progresIdx = 0;
        progress = 0;
        std::list<TDetection> falseDetections;
        std::list<CORE::QUAKEML::Event> undetectedEvents;
        //for (unsigned vn = 2; vn < 5; vn++) {
        for (unsigned vn = 1; vn < 4; vn++) {
            sdc->logger.c_log(CORE::LOGGING::LL_REPORT, "Voting number %d events", vn);
            progresIdx = 0;
            falseDetections.clear();
            int no_ndet = 0;
            int undetekted_with_origin = 0;
            int undetekted_without_origin = 0;
            for (auto& detection : detected.list) {
                bool found = false;
                if (detection.votesNumber < vn) continue;
                no_ndet++;
                for (auto& evt : sdc->quakeML->eventParameters->event_) {
                    if (evtDetected(evt, detection, vn)) {
                        found = true;
                        break;
                    }
                }
                if (!found) falseDetections.push_back(detection);
                progress = progresIdx++ * 50 / detected.list.size();
            }
            progresIdx = 0;
            undetectedEvents.clear();
            for (auto& evt : sdc->quakeML->eventParameters->event_) {
                bool found = false;
                for (auto& detection : detected.list) {
                    if (evtDetected(evt, detection, vn)) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    undetectedEvents.push_back(evt);
                    if (evt.origin.empty()) {
                        undetekted_without_origin++;
                    }
                    else {
                        undetekted_with_origin++;
                    }
                }
                progress = progresIdx++ * 50 / sdc->quakeML->eventParameters->event_.size() + 50;
            }
            sdc->logger.c_log(CORE::LOGGING::LL_REPORT, "Undetected %d(wo=%d, no=%d)/%d events", undetectedEvents.size(), undetekted_with_origin, undetekted_without_origin, sdc->quakeML->eventParameters->event_.size());
            for (auto& evt : undetectedEvents) {
                if (evt.origin.empty()) {
                    std::set<std::string> stas;
                    for (auto& pick : evt.pick) {
                        stas.insert(pick.waveformID.getStaName());
                    }
                    std::string line;
                    for (auto& sta : stas) {
                        line += "; ";
                        line += sta;
                    }
                    sdc->logger.c_log(CORE::LOGGING::LL_INFO, "   %d %s; %s", stas.size(), evt.publicID.resourceID.c_str(),line.c_str());
                } else {
                    sdc->logger.c_log(CORE::LOGGING::LL_INFO, "  Or %s", evt.publicID.resourceID.c_str());
                }
            }
            sdc->logger.c_log(CORE::LOGGING::LL_REPORT, "False %d/%d(%d) detection", falseDetections.size(), no_ndet, detected.list.size());
            for (auto& det : falseDetections) {
                sdc->logger.c_log(CORE::LOGGING::LL_DEBUG, "   %s - %s, %d", det.beginTime.toString().c_str(), det.beginTime.toString().c_str(), det.votesNumber);
            }
        }
        sdc->logger.c_log(CORE::LOGGING::LL_INFO, "All events:");
        std::vector<int> noStas(10, 0);
        int noO = 0;
        for (auto& evt : sdc->quakeML->eventParameters->event_) {
            if (evt.origin.empty()) {
                std::set<std::string> stas;
                for (auto& pick : evt.pick) {
                    stas.insert(pick.waveformID.getStaName());
                }
                std::string line;
                for (auto& sta : stas) {
                    line += "; ";
                    line += sta;
                }
                sdc->logger.c_log(CORE::LOGGING::LL_INFO, "  %d %s; %s", stas.size(), evt.publicID.resourceID.c_str(), line.c_str());
                noStas[stas.size() > 9 ? 9 : stas.size()]++;
            }
            else {
                sdc->logger.c_log(CORE::LOGGING::LL_INFO, "  o %s", evt.publicID.resourceID.c_str());
                noO++;
            }
        }
        for (int i = 0; i < 10; i++) {
            sdc->logger.c_log(CORE::LOGGING::LL_INFO, "%d: %3d / %d", i, noStas[i], sdc->quakeML->eventParameters->event_.size());
        }
        sdc->logger.c_log(CORE::LOGGING::LL_INFO, "Origin: %3d / %d", noO, sdc->quakeML->eventParameters->event_.size());

    }

    void ExportANNDetection::defineMenu() {
        try {
            std::ifstream ifs = sdc->ifConfigStream("ExportANNDetection.cfg");
            ifs >> outptPath;
            if (!wxDirExists(outptPath)) throw CORE::EXCEPTIONS::CoreException("Missing configuration directory\n'%s'\nDo setup ANN detection", outptPath.c_str());
            if (!wxFileExists(outptPath + "/config/ANNDetectionConfig.xml")) throw CORE::EXCEPTIONS::CoreException("Missing ANNDetectionConfig.xml file\nin '%s'", outptPath.c_str());
        }
        catch (CORE::EXCEPTIONS::CoreException& e) {
            sdc->logger << CORE::LOGGING::LL_WARNING << e.what();
        }
        PluginMenuWrapper* pmwN = PluginMenuWrapper::withoutGUI(sdc, "Save noise", boost::bind(&ExportANNDetection::saveNoise, this, _1), boost::bind(&ExportANNDetection::getProgress, this));
        PluginMenuWrapper* pmwD = PluginMenuWrapper::withoutGUI(sdc, "Save undetected", boost::bind(&ExportANNDetection::saveUndetected, this, _1), boost::bind(&ExportANNDetection::getProgress, this));
        CORE::GUI::wxMenuItemDef* miN = new CORE::GUI::wxMenuItemDef(sdc->menuID++, pmwN->getName());
        CORE::GUI::wxMenuItemDef* miD = new CORE::GUI::wxMenuItemDef(sdc->menuID++, pmwD->getName());
        sdc->channnelMenuItems->menus.push_back(CORE::DATA::TPopupMenus(miN, (void*)pmwN));
        sdc->channnelMenuItems->menus.push_back(CORE::DATA::TPopupMenus(miD, (void*)pmwD));
        sdc->zoomChannelMenuItems->menus.push_back(CORE::DATA::TPopupMenus(miN, (void*)pmwN));
        sdc->zoomChannelMenuItems->menus.push_back(CORE::DATA::TPopupMenus(miD, (void*)pmwD));
        mp.push_back(PluginMenuWrapper::withoutGUI(sdc, "Export ANN detection", boost::bind(&ExportANNDetection::process, this, _1), boost::bind(&ExportANNDetection::getProgress, this), std::string("ANN")));
        mp.push_back(PluginMenuWrapper::withoutGUI(sdc, "Compute ANN detection", boost::bind(&ExportANNDetection::processDetTest2, this, _1), boost::bind(&ExportANNDetection::getProgress, this), std::string("ANN")));
        mp.push_back(PluginMenuWrapper::withoutGUI(sdc, "Compute ANN detection to file", boost::bind(&ExportANNDetection::processDetTest, this, _1), boost::bind(&ExportANNDetection::getProgress, this), std::string("ANN")));
        mp.push_back(PluginMenuWrapper::withoutGUI(sdc, "Setup ANN detection", boost::bind(&ExportANNDetection::setOutput, this, _1), boost::bind(&ExportANNDetection::getProgress, this), std::string("&Setup")));
        mp.push_back(PluginMenuWrapper::withoutGUI(sdc, "Make noises", boost::bind(&ExportANNDetection::processNoisesList, this, _1), boost::bind(&ExportANNDetection::getProgress, this), std::string("ANN")));
        mp.push_back(PluginMenuWrapper::withoutGUI(sdc, "List noises", boost::bind(&ExportANNDetection::listNoises, this, _1), boost::bind(&ExportANNDetection::getProgress, this), std::string("ANN")));
        mp.push_back(PluginMenuWrapper::withoutGUI(sdc, "Do detection", boost::bind(&ExportANNDetection::doDetection, this, _1), boost::bind(&ExportANNDetection::getProgress, this), std::string("ANN")));
        mp.push_back(PluginMenuWrapper::withoutGUI(sdc, "Test detection", boost::bind(&ExportANNDetection::testDetection, this, _1), boost::bind(&ExportANNDetection::getProgress, this), std::string("ANN")));
        mp.push_back(PluginMenuWrapper::withoutGUI(sdc, "Voting detection", boost::bind(&ExportANNDetection::doVotingList, this, _1), boost::bind(&ExportANNDetection::getProgress, this), std::string("ANN")));
        mp.push_back(PluginMenuWrapper::withoutGUI(sdc, "View detections", boost::bind(&ExportANNDetection::doViewList, this, _1), boost::bind(&ExportANNDetection::getProgress, this), std::string("ANN")));
        mp.push_back(PluginMenuWrapper::withoutGUI(sdc, "Do detected signals", boost::bind(&ExportANNDetection::doDetectedSignals, this, _1), boost::bind(&ExportANNDetection::getProgress, this), std::string("ANN")));
    }

    ExportANNDetection::ExportANNDetection() {
        pd.push_back(new PluginDependency("SOH extrapolation resumpling"));
        pd.push_back(new PluginDependency("ArcLink Plugin"));
    }
    
    ExportANNDetection::~ExportANNDetection() {
        std::ofstream ofs(sdc->configPath() + "ExportANNDetection.cfg");
        ofs << outptPath;
    }

} }

#if defined(_MSC_VER)
__declspec(dllexport) void* getClass() {
#elif defined(__GNUC__)
void* getClass() {
#endif
	return static_cast< void* > (new CORE::PLUGINS::ExportANNDetection);
}
