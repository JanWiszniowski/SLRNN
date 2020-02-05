#include "LoadDetections.h"
#include <fstream>

namespace CORE { namespace PLUGINS {

TLoadDetection::TLoadDetection() {

}

void TLoadDetection::load(const std::string& fname) {
    list.clear();
    std::ifstream file(fname);
    if (!file) return;
    std::string line;
    TDetection detection;
    while (std::getline(file, line)) {
        char lineType[4];
        char timeTxt[32];
        int votesNumber;
        sscanf(line.c_str(), "%3s %31s", lineType, timeTxt);
        if (!strcmp(lineType, "ON")) {
            if (sscanf(line.c_str(), "%3s %31s", lineType, timeTxt) == 2) {
                detection.beginTime = CORE::TIME::QMLTime(timeTxt, "%Y-%m-%dT%H:%M:%S.%usZ");
          }
            votesNumber = 0;
        }
        if (!strcmp(lineType, "OFF")) {
            if (sscanf(line.c_str(), "%3s %31s %d", lineType, timeTxt, &votesNumber) == 3) {
                detection.endTime = CORE::TIME::QMLTime(timeTxt, "%Y-%m-%dT%H:%M:%S.%usZ");
                detection.votesNumber = votesNumber;
                list.push_back(detection);
            }

        }
    }
}

}}
