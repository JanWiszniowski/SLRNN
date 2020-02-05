#include <list>
#include "IBasePlugin.h"

namespace CORE { namespace PLUGINS {

struct TDetection {
    CORE::TIME::QMLTime beginTime;
    CORE::TIME::QMLTime endTime;
    int votesNumber = 0;
    std::string info;
};

class TLoadDetection {
public:
    std::list<TDetection> list;
    TLoadDetection();
    TLoadDetection(const std::string& fn) { load(fn); }
    void load(const std::string&);

};

}}