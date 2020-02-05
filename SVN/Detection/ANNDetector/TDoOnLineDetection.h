#ifndef TDOONLINEDETECTION_H
#define TDOONLINEDETECTION_H
#include "QMLTime.h"
#include "QuakeMLDef.h"
#include "WaveFormsLine.h"
#include "SLRNNFile.h"
#include "Logging.h"
//    struct TDetCurrentState {
//        int state;
//        CORE::TIME::QMLTime& time;
//    };
//    struct TDetStaState {
//        std::list<TDetCurrentState> states;
//        CORE::QUAKEML::Wawe sta;
//        int avaiability;
//    };
//
//    class TVoting {
//
//    };
//
    class TStaVotingBase {
//        std::vector<int>* stations;
//        int index;
    public:
        virtual void on(const CORE::TIME::QMLTime& t, int val = 0) = 0;
        virtual void off(const CORE::TIME::QMLTime& t, int val = 0) = 0;
        virtual void nothing(const CORE::TIME::QMLTime& t) {}
    };

    class TDoOffLineDetection : public TSequentialDetection {
        CORE::QUAKEML::WaveformStreamID sta;
        std::vector<double> _v;
        CORE::TIME::QMLTime _t;
        std::string annName;
        std::shared_ptr<TStaVotingBase> voting;
        std::ostream* dstream = NULL;
        int arming = 0;
        int deton = 0;
        int ontime = 0;
        int maxdet = 0;
        int index =1;
        //int index;
        // CORE::DATA::SharedDataContainer* sdc;
        CORE::LOGGING::Logger& logger;
        CORE::TIME::QMLTimeMicroseconds _step = CORE::TIME::MODULO_SEC;
        std::unique_ptr<CORE::Seismic_Precessing::TSLRNN <double, double> > detPtr;
    protected:
        void proc(const std::vector<double>& vector, const CORE::TIME::QMLTime& ttime);
    public:
//        TDoOffLineDetection(CORE::DATA::SharedDataContainer* sd, std::shared_ptr<TStaVotingBase>& v, CORE::TIME::QMLTimeMicroseconds step = 100000LL);
        TDoOffLineDetection(CORE::LOGGING::Logger& log, std::shared_ptr<TStaVotingBase> v, CORE::TIME::QMLTimeMicroseconds step = 100000LL)
         : voting(v), logger(log), _step(step) {}

        virtual ~TDoOffLineDetection();
        void reset() override;
        void reset(const std::string& aname) { annName = aname; reset(); }
        bool ready() override { return false; }
        void setSta(const CORE::QUAKEML::WaveformStreamID& w) { sta = w.getNetSta(); }
        bool get(std::vector<double>& v, CORE::TIME::QMLTime& t) override { v = _v; t = _t; return false; }
        CORE::TIME::QMLTime& time() override { return _t; }

    };

#endif // TDOONLINEDETECTION_H
