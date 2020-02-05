#include "TDoOnLineDetection.h"

//staState.push_back()
//
//for(auto& state : stationsStates) {
//    if(state.avaiability) {
//        if(state.states.back().time + timeToOff < currTime) {
//            state.avaiability = false;
//            state.states.clear();
//        }
//    }
//}
//if(thisState.state.empty())

void TDoOffLineDetection::proc(const std::vector<double>& vector, const CORE::TIME::QMLTime& ttime) {
    //CORE::QUAKEML::Event* evt = sdc->quakeML->getCurrentEventPtr();
    //CORE::QUAKEML::Origin* org = sdc->quakeML->getCurrentOriginPtr();
    //CORE::TIME::QMLTimeMicroseconds sec1(CORE::TIME::MODULO_SEC);
    _v = vector;
    _t = ttime - _timeShift;
    detPtr->process(vector.data());
    if (arming > 300) {
        if (detPtr->output(0) > 0) {
            if (deton == 1) {
                voting->on(_t);
                logger.c_log(CORE::LOGGING::LL_INFO, "Detection %d: %s on at %s", index, sta.getStaName().c_str(), _t.toString().c_str());
                ontime = 20 * 1000000 / _step;
                return;
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
                    voting->off(_t, maxdet);
                    logger.c_log(CORE::LOGGING::LL_INFO, "Detection %d: %s off at %s /%d, ", index++, sta.getStaName().c_str(), _t.toString().c_str(), maxdet);
                    deton = 0;
                    maxdet = 0;
                    return;
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
    voting->nothing(_t);
}

TDoOffLineDetection::~TDoOffLineDetection() {

}

void TDoOffLineDetection::reset() {
//    if (file) fflush(file);
    index = 1;
    arming = 0;
    deton = 0;
    ontime = 0;
    maxdet = 0;
    if (annName.empty()) {
        detPtr.reset();
    }
    else {
        logger.c_log(CORE::LOGGING::LL_REPORT, "SLRNN '%s'", annName.c_str());
        detPtr.reset(new CORE::Seismic_Precessing::TSLRNNFile(annName));
    }
}
