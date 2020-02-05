#include "WaveFormsLine.h"
#include "sqr.h"
#include "file_sc_xml.h"

/// 
///============================================================================
/// Filter IIR liczacy na danych 3D
///
/// Wejsciem s¹ próbki 3D - tripleSampleType
/// Wynikiem s¹ próbki 3D double

class TDIIRFilter : public TDoubleFilter
{ // Filtr cyfrowy typu IIR
protected:
    std::string _name;
    std::vector< CORE::Seismic_Precessing::TVector3<double> > x;
    std::vector< CORE::Seismic_Precessing::TVector3<double> > y;
    std::vector<double> A;
    std::vector<double> B;
    //   float Adiv ;
    CORE::Seismic_Precessing::TBaseProcessing<CORE::DATA::tripleSampleType, CORE::Seismic_Precessing::TVector3<double> >* cloneprocessing() const override { return new TDIIRFilter(A, B, _name); }
public:
    TDIIRFilter() : _name("IIR"), x(1, CORE::Seismic_Precessing::TVector3<double>(0, 0, 0)), y(2, CORE::Seismic_Precessing::TVector3<double>(0, 0, 0)), A(1, 1.0), B(1, 1.0) {}
    TDIIRFilter(const std::vector<double>& iA, const std::vector<double>& iB, const std::string& ides = "IIR") : _name("IRR"), x(iB.size(), CORE::Seismic_Precessing::TVector3<double>(0, 0, 0)), y(iA.size(), CORE::Seismic_Precessing::TVector3<double>(0, 0, 0)), A(iA), B(iB) {}
    TDIIRFilter(const std::string& fname);
    virtual ~TDIIRFilter() {}
    CORE::Seismic_Precessing::TVector3<double> apply(const CORE::DATA::tripleSampleType&) override;
    void reset() override;
    std::string name() override { return _name; }
};

class TDIIRFilterTimeShift : public TDIIRFilter
{ // Filtr cyfrowy typu IIR
protected:
    std::vector< CORE::DATA::tripleSampleType > xShift;
    unsigned timeShift;
    unsigned index;
    CORE::Seismic_Precessing::TBaseProcessing<CORE::DATA::tripleSampleType, CORE::Seismic_Precessing::TVector3<double> >* cloneprocessing() const override { return new TDIIRFilterTimeShift(A, B, timeShift, _name); }
public:
    TDIIRFilterTimeShift() : TDIIRFilter(), xShift(0, CORE::DATA::tripleSampleType(0, 0, 0)), timeShift(0) { reset();  }
    TDIIRFilterTimeShift(const std::vector<double>& iA, const std::vector<double>& iB, int ts, const std::string& ides = "IIR shifted") : TDIIRFilter(iA, iB, ides), xShift(ts, CORE::DATA::tripleSampleType(0, 0, 0)), timeShift(ts) { reset(); }
    TDIIRFilterTimeShift(const std::string& fname, int ts) : TDIIRFilter(fname), xShift(ts, CORE::DATA::tripleSampleType(0, 0, 0)), timeShift(ts) {}
    virtual ~TDIIRFilterTimeShift() {}
    CORE::Seismic_Precessing::TVector3<double> apply(const CORE::DATA::tripleSampleType&) override;
    void reset() override;
};

/// 
///============================================================================
/// Liczy wartosci wejsciowe ANN w kazdej probce sygnalu sejsmicznego

class TCovariance : public TSampleBySampleProcessing
{
protected:
    double b1;
    double b2;
    double a1;
    double a2;
    double Mz;
    double Mn;
    double Me;
    //std::vector<double> out;
    int armingTime = 40;
    std::string filterName ;
    TSampleBySampleProcessing* cloneprocessing() const override { return new TCovariance(b1, a1, filterName); }
public:
    TCovariance() : TSampleBySampleProcessing(6), b1(0.99), b2(0.01), a1(0.999), a2(0.001), filterName("NF") { reset(); }
    TCovariance(double b, double a = 0.001, const std::string& fn="NF");
    virtual ~TCovariance() {}
    //std::vector<double> apply(const CORE::Seismic_Precessing::TVector3<double>&) override;
    void process(const CORE::Seismic_Precessing::TVector3<double>&) override;
    void reset() override;
    std::string name() override { return std::string("Covariance") + filterName; }
};

class TStaLta : public TSampleBySampleProcessing
{
protected:
    double staRatio1;
    double staRatio2;
    double ltaRatio1;
    double ltaRatio2;
    //std::vector<double> out;
    int armingTime = 40;
    std::string filterName;
    TSampleBySampleProcessing* cloneprocessing() const override { return new TStaLta(*this); }
public:
    TStaLta(double sta, double lta, const std::string& fn = "NF");
    TStaLta(const TStaLta& stn) : TSampleBySampleProcessing(4), staRatio1(stn.staRatio1), staRatio2(stn.staRatio2), ltaRatio1(stn.ltaRatio1), ltaRatio2(stn.ltaRatio2), filterName(stn.filterName) { reset(); }
    virtual ~TStaLta() {}
    //std::vector<double> apply(const CORE::Seismic_Precessing::TVector3<double>&) override;
    void process(const CORE::Seismic_Precessing::TVector3<double>&) override;
    void reset() override;
    std::string name() override { return std::string("STA/LTA ") + filterName; }
};

class TSampleBySampleRingBuffer : public TSampleBySampleProcessing
{
protected:
    unsigned idx;
    std::string filterName;
    TSampleBySampleProcessing* cloneprocessing() const override { return new TSampleBySampleRingBuffer(*this); }
public:
    TSampleBySampleRingBuffer(unsigned n, const std::string& fn = "NF") : TSampleBySampleProcessing(3*n), filterName(fn) { reset(); }
    TSampleBySampleRingBuffer(const TSampleBySampleRingBuffer& stn) : TSampleBySampleProcessing(stn.out.size()), filterName(stn.filterName) { reset(); }
    virtual ~TSampleBySampleRingBuffer() {}
    //std::vector<double> apply(const CORE::Seismic_Precessing::TVector3<double>&) override;
    void process(const CORE::Seismic_Precessing::TVector3<double>& v) override;
    void reset() override;
    std::string name() override { return std::string("RingBuffer ") + filterName; }
};

/// 
///============================================================================
/// Liczy wartosci wejsciowe ANN w kazdym kroku danych wejsciowych sieci

class TPolarisationLambda : public TStepByStepProcessing
{ // Polarisation
protected:
    std::vector<double> lambda;
    TStepByStepProcessing* cloneprocessing() const override { return new TPolarisationLambda(); }
    std::string filterName;
public:
    TPolarisationLambda(const std::string& fn = "NF") : lambda(2), filterName(fn) {}
    virtual ~TPolarisationLambda() {}
    std::vector<double> apply(const std::vector<double>& in) override;
    std::string name() override { return std::string("Polarisation Fisher ") + filterName; }
    void reset() override {}
};

class TPolarisationLambda3D : public TStepByStepProcessing
{ // Polarisation
protected:
    std::vector<double> lambda;
    TStepByStepProcessing* cloneprocessing() const override { return new TPolarisationLambda3D(); }
    std::string filterName;
public:
    TPolarisationLambda3D(const std::string& fn = "NF") : lambda(2), filterName(fn)  { }
    virtual ~TPolarisationLambda3D() {}
    std::vector<double> apply(const std::vector<double>& in) override;
    std::string name() override { return std::string("Polarisation3D lambda ") + filterName; }
    void reset() override {}
};

class TPolarisationLambda2D : public TStepByStepProcessing
{ // Polarisation
protected:
    std::vector<double> lambda;
    TStepByStepProcessing* cloneprocessing() const override { return new TPolarisationLambda2D(); }
    std::string filterName;
public:
    TPolarisationLambda2D(const std::string& fn = "NF") : lambda(1), filterName(fn) { }
    virtual ~TPolarisationLambda2D() {}
    std::vector<double> apply(const std::vector<double>& in) override;
    std::string name() override { return std::string("Polarisation2D lambda ") + filterName; }
    void reset() override {}
};

class TDegreeOfPolarization : public TStepByStepProcessing
{ // Polarisation
protected:
    std::vector<double> lambda;
    TStepByStepProcessing* cloneprocessing() const override { return new TDegreeOfPolarization(); }
    std::string filterName;
public:
    TDegreeOfPolarization(const std::string& fn = "NF") : lambda(1), filterName(fn) {}
    virtual ~TDegreeOfPolarization() {}
    std::vector<double> apply(const std::vector<double>& in) override;
    std::string name() override { return std::string("Degree of polarization ") + filterName; }
    void reset() override {}
};
class TRBDegreeOfPolarization : public TStepByStepProcessing
{ // Polarisation
protected:
    std::vector<double> lambda;
    TStepByStepProcessing* cloneprocessing() const override { return new TRBDegreeOfPolarization(); }
    std::string filterName;
public:
    TRBDegreeOfPolarization(const std::string& fn = "NF") : lambda(1,0.0), filterName(fn) {}
    virtual ~TRBDegreeOfPolarization() {}
    std::vector<double> apply(const std::vector<double>& in) override;
    std::string name() override { return std::string("Degree of polarization ") + filterName; }
    void reset() override {}
};

class STADivLTA : public TStepByStepProcessing
{ // Polarisation
protected:
	std::vector<double> out;
	TStepByStepProcessing* cloneprocessing() const override { return new STADivLTA(); }
	std::string filterName;
public:
	STADivLTA(const std::string& fn = "NF") : out(2), filterName(fn) {}
	virtual ~STADivLTA() {}
	std::vector<double> apply(const std::vector<double>& in) override;
	std::string name() override { return std::string("STA/LTA ") + filterName; }
	void reset() override {}
};

/// 
///============================================================================
/// Obiekty zawierajace detekcje

class TFilterBankDetection {
    std::vector<double> sample;
protected:
    TSequentialDetection& detection;
    CORE::TIME::QMLTimeMicroseconds _step = CORE::TIME::MODULO_SEC;
    TWaveFormsLine d3;
    std::vector< std::shared_ptr< TFilterProcessingList > > _filterbank;
    //std::list<MultipleCW_SP> _list;
    //MultipleCW_SP retsp;
public:
    TFilterBankDetection(TSequentialDetection& det) : detection(det) {}
    void push(CORE::DATA::SingleCW*, char);
    //const MultipleCW_SP& pop();
    //bool ready();
    //void reset() { detection.reset(); d3.reset(); filters.reset(); _list.clear(); }
    void reset() { detection.reset(); d3.reset(); for (auto&& f : _filterbank) f->reset(); }
    //virtual CORE::TIME::QMLTimeMicroseconds GetMaxTimeShift() const = 0;
    void setStep(CORE::TIME::QMLTimeMicroseconds s) { _step = s; }
};

class TFilterBankNoTimeShiftDetection : public TFilterBankDetection {
public:
    TFilterBankNoTimeShiftDetection(XMLNode& configNode, const std::string& filterPath, TSequentialDetection& det);
    //CORE::TIME::QMLTimeMicroseconds GetMaxTimeShift() const override { return 0; }
};

class TFilterBankTimeShiftDetection : public TFilterBankDetection {
    int maxTimeShift;
    CORE::TIME::QMLTimeMicroseconds microsecondsTimeShift;
public:
    TFilterBankTimeShiftDetection(XMLNode& configNode, const std::string& filterPath, TSequentialDetection& det, CORE::TIME::QMLTimeMicroseconds istep = 10000LL);
    //CORE::TIME::QMLTimeMicroseconds GetMaxTimeShift() const override { return step * maxTimeShift; }
    void setInputStep(CORE::TIME::QMLTimeMicroseconds istep) { microsecondsTimeShift = istep * maxTimeShift; detection.setTimeShift(microsecondsTimeShift); }
};