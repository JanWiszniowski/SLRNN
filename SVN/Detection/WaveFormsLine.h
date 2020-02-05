#ifndef __WAVEFORMSLINE_H
#define __WAVEFORMSLINE_H
#include "WaveForm.h"
#include "DigProcessing.h"

///
///-----------------------------------------------------------------------------
///

typedef CORE::DATA::ContinousWaveform<CORE::DATA::tripleSampleType> TripleCW;
typedef std::shared_ptr<TripleCW> TripleCW_SP;

typedef CORE::DATA::ContinousWaveform<CORE::DATA::multipleSampleType> MultipleCW;
typedef std::shared_ptr<MultipleCW> MultipleCW_SP;

typedef CORE::Seismic_Precessing::TBaseProcessing<CORE::DATA::tripleSampleType, CORE::Seismic_Precessing::TVector3<double> > TDoubleFilter;
typedef std::shared_ptr<TDoubleFilter> TDoubleFilter_SP;

class TSampleBySampleProcessing
{
protected:
    virtual TSampleBySampleProcessing* cloneprocessing() const = 0;
public:
    std::vector<double> out;
    virtual void process(const CORE::Seismic_Precessing::TVector3<double>&) = 0;
    virtual std::string name() { return ""; }
    virtual void reset() = 0;
    TSampleBySampleProcessing(unsigned n) : out(n,0.0) {}
    virtual ~TSampleBySampleProcessing() {}
};
typedef std::shared_ptr<TSampleBySampleProcessing> TSampleBySampleProcessing_SP;


typedef CORE::Seismic_Precessing::TBaseProcessing< std::vector<double>, std::vector<double> > TStepByStepProcessing;
typedef std::shared_ptr<TStepByStepProcessing> TStepByStepProcessing_SP;

struct TSampleBySampleProcessingList {
    std::vector<double> out;
    TSampleBySampleProcessing_SP sampleBySampleProcessing;
    std::vector<TStepByStepProcessing_SP> stepByStepProcessingList;
    void reset() { sampleBySampleProcessing->reset(); for (auto&& f : stepByStepProcessingList) f->reset(); }
    void push(const CORE::Seismic_Precessing::TVector3<double >&);
    void proc(const CORE::Seismic_Precessing::TVector3<double >&);
};

struct TFilterProcessingList {
    std::vector<double> out;
    TDoubleFilter_SP doubleFilter;
    std::vector< std::shared_ptr<TSampleBySampleProcessingList> > sampleBySampleProcessingList;
    void reset() { doubleFilter->reset(); for (auto&& f : sampleBySampleProcessingList) f->reset(); }
    void push(const CORE::DATA::tripleSampleType&);
    void proc(const CORE::DATA::tripleSampleType&);
};

///
///---------------------------------------------------------------------------
/// Synchronizuje trzy sk³adowe sygna³u
/// Pracuje blokowo. Na wejsciu podaje siê ciag³e bloki danych kana³ów

class TWaveFormsLine {
protected:
	CORE::TIME::QMLTime _begin;
    CORE::TIME::QMLTime _end;
	int _step = 0;
    std::list<CORE::DATA::SingleCW_SP> _line[3];
    void push_chan(CORE::DATA::SingleCW*, std::list<CORE::DATA::SingleCW_SP>&);
public:
    void push(CORE::DATA::SingleCW*, char);
    TripleCW_SP pop();
    bool ready() { return (_begin.exists() && _end > _begin); }
    void reset() { for (auto&& l : _line) l.clear(); _begin = _end = CORE::TIME::NaNQMLTime; _step = 0; }
};

///
///---------------------------------------------------------------------------
/// Demutlipleksuje - rozdziela dane wejsciowe blokowe 3D na wynik na wieloelementowy
/// gdzie wyjœcia odpowiadaj¹ odpowiednim sygna³om wejsciowym detekcji ANN
/// Pracuje blokowo. Na wejsciu podaje siê ciag³e bloki danych kana³ów
/// Bloki wyjœciowe maj¹ zmienione próbkowanie
//
//class TDemultiplex {
//protected:
//    MultipleCW_SP _out;
//    /// Procesy sygna³ów
//    std::vector< std::shared_ptr< TFilterProcessingList > > _filterbank;
//    /// Rozmiary wektrów wyjœciowych procesów
//    std::vector<unsigned> _nouts;
//    unsigned _nout;
//    CORE::TIME::QMLTime _end;
//    std::list<TripleCW_SP> _line;
//    int _step;
//    CORE::TIME::QMLTimeMicroseconds _step;
//
//    bool _running = false;
//    void push_chan(TripleCW_SP&, std::list<CORE::DATA::SingleCW_SP>&);
//public:
//    void push(TripleCW_SP&);
//    MultipleCW_SP pop();
//    bool ready() { return _line.size() > 0; }
//    void addProcess(std::shared_ptr< TFilterProcessingList >&, unsigned nout);
//    void reset() { _line.clear(); for (auto&& fp : _filterbank) fp->reset(); _end = CORE::TIME::NaNQMLTime; }
//};

///
///---------------------------------------------------------------------------
/// Klasa bazowa dla detekcji zarówno rekurencyjnej jak i z ruchomym oknem

class TSequentialDetection {
protected:
    CORE::TIME::QMLTimeMicroseconds _timeShift = 0;
    CORE::TIME::QMLTime _end;
public:
    //void push(MultipleCW_SP&);
    virtual bool ready() = 0;
    virtual bool get(std::vector<double>&, CORE::TIME::QMLTime&) = 0;
    virtual void proc(const std::vector<double>&, const CORE::TIME::QMLTime&) = 0;
    virtual CORE::TIME::QMLTime& time() = 0;
    virtual void reset() = 0;
    void setTimeShift(CORE::TIME::QMLTimeMicroseconds ts) { _timeShift = ts; }
};

///
///---------------------------------------------------------------------------
/// Klasa bazowa dla detekcji z ruchomym oknem

class MovingWindowDetection : public TSequentialDetection {
protected:
    unsigned _length;
    CORE::TIME::QMLTime _end;
    std::list < std::vector<double> > _window;
    void proc(const std::vector<double>&, const CORE::TIME::QMLTime&) override;
    virtual void preproc() = 0;
    virtual void procvect(std::vector<double>&, int idx) = 0;
    virtual void postproc() = 0;
    virtual void reset() override ;
public:
};

///
///---------------------------------------------------------------------------

#endif
