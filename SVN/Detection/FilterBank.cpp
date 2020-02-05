#include "stdafx.h"
#include "FilterBank.h"
#include <iostream>
#include <fstream>
//boost?? #include <boost/numeric/ublas/symmetric.hpp>
#include <armadillo>
#include <cmath>
//#include <boost/math/constants/constants.hpp>
#include "EigenValueSimetric3x3.h"

#define __in_Czz (in[0])
#define __in_Cnn (in[1])
#define __in_Cee (in[2])
#define __in_Czn (in[3])
#define __in_Cze (in[4])
#define __in_Cne (in[5])
#define __out_Czz out[0]
#define __out_Cnn out[1]
#define __out_Cee out[2]
#define __out_Czn out[3]
#define __out_Cze out[4]
#define __out_Cne out[5]
//---------------------------------------------------------------------------

static void setAverage(double b, double& b1, double& b2) {
    if (b == 0.0)  throw CORE::EXCEPTIONS::CoreException("Polarisation averaging value is 0");
    if (b < 0.0) b = -b;
    if (b >= 1.0) {
        b2 = 1.0 / b;
        b1 = 1.0 - b2;
    }
    else if (b < 0.5) {
        b2 = b;
        b1 = 1.0 - b2;
    }
    else {
        b1 = b;
        b2 = 1.0 - b1;
    }
}

//---------------------------------------------------------------------------

TDIIRFilter::TDIIRFilter(const std::string& fname) {
    int size;
    std::ifstream cfg(fname);
    if (!cfg) throw CORE::EXCEPTIONS::CoreException("Cannot open filter file '%s'", fname.c_str());
    std::getline(cfg, _name);
    cfg >> size;
    x.resize(size);
    B.resize(size);
    for (auto&& b : B) {
        cfg >> b;
    }
    cfg >> size;
    y.resize(size);
    A.resize(size);
    for (auto&& a : A) {
        cfg >> a;
    }
    reset();
}

void TDIIRFilter::reset()
{
    fill(x.begin(), x.end(), CORE::Seismic_Precessing::TVector3<double>(1.0e-20,1.0e-20,1.0e-20));
    fill(y.begin(), y.end(), CORE::Seismic_Precessing::TVector3<double>(1.0e-20,1.0e-20,1.0e-20));
}

CORE::Seismic_Precessing::TVector3<double> TDIIRFilter::apply(const CORE::DATA::tripleSampleType& in)
{
    for(int i = 0; i <3; i++) x[0][i] = in[i];
    CORE::Seismic_Precessing::TVector3<double> sum = x[0] * B[0];
    for (int k = B.size() - 1; k > 0; k--) {
        sum = sum + x[k] * B[k];
        x[k] = x[k - 1];
    }
    for (int k = A.size() - 1; k > 0; k--) {
        sum = sum - y[k] * A[k];
        y[k] = y[k - 1];
    }
    sum *= (1.0 / A[0]);
    y[1] = sum;
    return sum;
}

void TDIIRFilterTimeShift::reset()
{
    TDIIRFilter::reset();
    const CORE::DATA::singleSampleType mv = static_cast<CORE::DATA::singleSampleType > (1.0e-20);
    fill(xShift.begin(), xShift.end(), CORE::DATA::tripleSampleType(mv, mv, mv));
    index = 0;
}

CORE::Seismic_Precessing::TVector3<double> TDIIRFilterTimeShift::apply(const CORE::DATA::tripleSampleType& in) {
    if (timeShift) {
        CORE::Seismic_Precessing::TVector3<double> out = TDIIRFilter::apply(xShift[index]);
        xShift[index] = in;
        index++;
        index %= timeShift;
        return out;
    }
    else {
        return TDIIRFilter::apply(in);
    }
}

//---------------------------------------------------------------------------

std::vector<double> TPolarisationLambda3D::apply(const std::vector<double>& in) {
	double A[3][3];
	A[0][0] = in[0];
	A[1][1] = in[1];
	A[2][2] = in[2];
	A[0][1] = A[1][0] = in[3];
	A[0][2] = A[2][0] = in[4];
	A[1][2] = A[2][1] = in[5];
	std::vector<double> eigs = MyMatrixOperator::eigenvalues(A);
	std::sort(eigs.begin(), eigs.end());
	double sum = eigs[0] + eigs[1] + eigs[2];
	lambda[0] = eigs[2] / sum;
	lambda[1] = eigs[0] / sum;
	return lambda;

}

std::vector<double> TDegreeOfPolarization::apply(const std::vector<double>& in) {
    double A[3][3];
    A[0][0] = in[0];
    A[1][1] = in[1];
    A[2][2] = in[2];
    A[0][1] = A[1][0] = in[3];
    A[0][2] = A[2][0] = in[4];
    A[1][2] = A[2][1] = in[5];
    std::vector<double> eigs = MyMatrixOperator::eigenvalues(A);
    lambda[0] = (sqr(eigs[0] - eigs[1]) + sqr(eigs[0] - eigs[2]) + sqr(eigs[1] - eigs[2])) / sqr(eigs[0] + eigs[1] + eigs[2]) / 2.0;
    return lambda;
}
// Faster version
//std::vector<double> TDegreeOfPolarization::apply(const std::vector<double>& in) {
//    lambda[0] =  1.0 + 3.0*(__in_Czz*__in_Cnn + __in_Czz*__in_Cee + __in_Cnn*__in_Cee - __in_Czn*__in_Czn - __in_Cze*__in_Cze - __in_Cne*__in_Cne) / sqr(__in_Czz + __in_Cnn + __in_Cee);
//    return lambda;
//}

std::vector<double> TRBDegreeOfPolarization::apply(const std::vector<double>& in) {
    double srz = 0.0, srn = 0.0, sre = 0.0;
    unsigned N3 = in.size();
    double N = double(N3 / 3);
    for (unsigned idx = 0; idx < N3; idx += 3) {
        srz += in[idx];
        srn += in[idx+1];
        sre += in[idx+2];
    }
    srz /= N;
    srn /= N;
    sre /= N;
    double Czz = 0.0, Cnn = 0.0, Cee = 0.0, Czn = 0.0, Cze = 0.0, Cne = 0.0;
    for (unsigned idx = 0; idx < N3; idx += 3) {
        double z = in[idx] - srz;
        double n = in[idx+1] - srn;
        double e = in[idx+2] - sre;
        Czz += z*z;
        Cnn += n*n;
        Cee += e*e;
        Czn += z*n;
        Cze += z*e;
        Cne += n*e;
    }
    Czz /= N;
    Cnn /= N;
    Cee /= N;
    Czn /= N;
    Cze /= N;
    Cne /= N;
    double A[3][3];
	A[0][0] = Czz;
	A[1][1] = Cnn;
	A[2][2] = Cee;
	A[0][1] = A[1][0] = Czn;
	A[0][2] = A[2][0] = Cze;
	A[1][2] = A[2][1] = Cne;
	std::vector<double> eigs = MyMatrixOperator::eigenvalues(A);
	lambda[0] = (sqr(eigs[0] - eigs[1]) + sqr(eigs[0] - eigs[2]) + sqr(eigs[1] - eigs[2])) / sqr(eigs[0] + eigs[1] + eigs[2]) / 2.0;
	return lambda;
}

//---------------------------------------------------------------------------

void TCovariance::process(const CORE::Seismic_Precessing::TVector3<double>& in) {
    if (armingTime > 0) {
        Mz = 0.9 * Mz + 0.1 * in.z();
        Mn = 0.9 * Mn + 0.1 * in.n();
        Me = 0.9 * Me + 0.1 * in.e();
    }
    else {
        Mz = a1 * Mz + a2 * in.z();
        Mn = a1 * Mn + a2 * in.n();
        Me = a1 * Me + a2 * in.e();
    }
    double z = in.z() - Mz;
    double n = in.n() - Mn;
    double e = in.e() - Me;
    if (armingTime > 0) {
        out[0] = 0.9 * out[0] + 0.1 * z * z;
        out[1] = 0.9 * out[1] + 0.1 * n * n;
        out[2] = 0.9 * out[2] + 0.1 * e * e;
        out[3] = 0.9 * out[3] + 0.1 * z * n;
        out[4] = 0.9 * out[4] + 0.1 * z * e;
        out[5] = 0.9 * out[5] + 0.1 * n * e;
        armingTime--;
    }
    else {
        out[0] = b1 * out[0] + b2 * z * z;
        out[1] = b1 * out[1] + b2 * n * n;
        out[2] = b1 * out[2] + b2 * e * e;
        out[3] = b1 * out[3] + b2 * z * n;
        out[4] = b1 * out[4] + b2 * z * e;
        out[5] = b1 * out[5] + b2 * n * e;
    }
}

void TCovariance::reset() {
    Mz = Mn = Me = out[0] = out[1] = out[2] = out[3] = out[4] = out[5] = 1e-20;
    armingTime = 40;
}

TCovariance::TCovariance(double b, double a, const std::string& fn) : TSampleBySampleProcessing(6), filterName(fn) {
    setAverage(b, b1, b2);
    setAverage(a, a1, a2);
    reset();
}

TStaLta::TStaLta(double sta, double lta, const std::string& fn) : TSampleBySampleProcessing(4), filterName(fn) {
	setAverage(sta, staRatio1, staRatio2);
	setAverage(lta, ltaRatio1, ltaRatio2);
	reset();
}

void TStaLta::process(const CORE::Seismic_Precessing::TVector3<double>& in) {
	double v = sqr(in.z());
	double h = sqr(in.n()) + sqr(in.e());
	if (armingTime > 0) {
        out[0] = out[0] * 0.8 + v * 0.2;
        out[1] = out[1] * 0.8 + v * 0.2;
        out[2] = out[2] * 0.8 + h * 0.2;
        out[3] = out[3] * 0.8 + h * 0.2;
		armingTime--;
	}
	else {
        out[0] = out[0] * staRatio1 + v * staRatio2;
        out[1] = out[1] * ltaRatio1 + v * ltaRatio2;
        out[2] = out[2] * staRatio1 + h * staRatio2;
        out[3] = out[3] * ltaRatio1 + h * ltaRatio2;
	}
}

void TStaLta::reset() {
	out[0] = out[1] = out[2] = out[3] = 1.0e-20;
}

void TSampleBySampleRingBuffer::process(const CORE::Seismic_Precessing::TVector3<double>& v) {
    out[idx++] = v.z();
    out[idx++] = v.n();
    out[idx++] = v.e();
    if (idx >= out.size()) {
        idx = 0;
    }
}

void TSampleBySampleRingBuffer::reset() {
    out.assign(out.size(), 0.0);
    idx = 0;
}

std::vector<double> STADivLTA::apply(const std::vector<double>& in) {
	out[0] = in[0] / in[1];
	out[1] = in[2] / in[3];
	return out;
}

//---------------------------------------------------------------------------

std::vector<double> TPolarisationLambda::apply(const std::vector<double>& in) {
    lambda[0] = (__in_Cnn + __in_Cee + sqrt(sqr(__in_Cnn - __in_Cee) + 4.0 * sqr(__in_Cne))) / 2.0 / sqrt(__in_Cnn + __in_Cee);
    double aux1 = (__in_Cnn + __in_Czz + sqrt(sqr(__in_Cnn - __in_Czz) + 4.0 * sqr(__in_Czn))) / 2.0 / sqrt(__in_Cnn + __in_Czz);
    double aux2 = (__in_Cee + __in_Czz + sqrt(sqr(__in_Cee - __in_Czz) + 4.0 * sqr(__in_Cze))) / 2.0 / sqrt(__in_Cee + __in_Czz);
    lambda[1] = (aux1 + aux2) / 2.0;
    return lambda;
}

//---------------------------------------------------------------------------

TFilterBankNoTimeShiftDetection::TFilterBankNoTimeShiftDetection(XMLNode& configNode, const std::string& filterPath, TSequentialDetection& det) : TFilterBankDetection(det) {
    //std::string steptText = configNode.attribute("step");
    //_step = stoi(steptText);
    for (XMLNode filterNode = configNode.child("filter"); !filterNode.isNull(); filterNode = configNode.nextchild("filter")) {
        std::string fname = filterNode.attribute("file_name");
        std::shared_ptr<TFilterProcessingList> filter(new TFilterProcessingList());
        filter->doubleFilter.reset(new TDIIRFilter(filterPath + fname));
        for (XMLNode sampleBysample = filterNode.child("sample_by_sample_process"); !sampleBysample.isNull(); sampleBysample = filterNode.nextchild("sample_by_sample_process")) {
            std::string processText = sampleBysample.attribute("process");
            std::shared_ptr<TSampleBySampleProcessingList> sampleProcess(new TSampleBySampleProcessingList);
            if (processText == "covariance") {
				std::string staText = sampleBysample.attribute("offset_averaging");
				std::string ltaText = sampleBysample.attribute("covariance_averaging");
				double staVal = std::stod(staText);
				double ltaVal = std::stod(ltaText);
				sampleProcess->sampleBySampleProcessing.reset(new TCovariance(staVal, ltaVal, filter->doubleFilter->name()));
            }
            else if (processText == "STA/LTA") {
                std::string staText = sampleBysample.attribute("sta");
                std::string ltaText = sampleBysample.attribute("lta");
                double staVal = std::stod(staText);
                double ltaVal = std::stod(ltaText);
                sampleProcess->sampleBySampleProcessing.reset(new TStaLta(staVal, ltaVal, filter->doubleFilter->name()));
            }
            else if (processText == "ring_buffer") {
                std::string sizeText = sampleBysample.attribute("size");
                unsigned sizeVal = std::stoi(sizeText);
                sampleProcess->sampleBySampleProcessing.reset(new TSampleBySampleRingBuffer(sizeVal, filter->doubleFilter->name()));
            }
            else {
                throw CORE::EXCEPTIONS::CoreException("Wrong name '%' of the sample by sample process", processText.c_str());
            }
            for (XMLNode stepBystep = sampleBysample.child("step_by_step_process"); !stepBystep.isNull(); stepBystep = sampleBysample.nextchild("step_by_step_process")) {
                std::string processText = stepBystep.attribute("process");
                TStepByStepProcessing_SP stepProcess;
                if (processText == "polarisation_Fisher") {
                    if (sampleProcess->sampleBySampleProcessing->name().substr(0, 10) != "Covariance") throw CORE::EXCEPTIONS::CoreException("The process '%' is not preceded by the covariance process", processText.c_str());
                    stepProcess.reset(new TPolarisationLambda(filter->doubleFilter->name()));
                }
                else if (processText == "polarisation_lambda_3D") {
                    if (sampleProcess->sampleBySampleProcessing->name().substr(0, 10) != "Covariance") throw CORE::EXCEPTIONS::CoreException("The process '%' is not preceded by the covariance process", processText.c_str());
                    stepProcess.reset(new TPolarisationLambda3D(filter->doubleFilter->name()));
                }
                else if (processText == "degree of polarization") {
                    if (sampleProcess->sampleBySampleProcessing->name().substr(0, 10) == "Covariance") {
                        stepProcess.reset(new TDegreeOfPolarization(filter->doubleFilter->name()));
                    }
                    else if (sampleProcess->sampleBySampleProcessing->name().substr(0, 10) == "RingBuffer") {
                        stepProcess.reset(new TRBDegreeOfPolarization(filter->doubleFilter->name()));
                    }
                    else {
                        throw CORE::EXCEPTIONS::CoreException("The process '%' is not preceded by the covariance process", processText.c_str());
                    }
                }
				else if (processText == "STA/LTA") {
					if (sampleProcess->sampleBySampleProcessing->name().substr(0, 7) != "STA/LTA") throw CORE::EXCEPTIONS::CoreException("The process '%' is not preceded by the covariance process", processText.c_str());
					stepProcess.reset(new STADivLTA(filter->doubleFilter->name()));
				}
				else {
                    throw CORE::EXCEPTIONS::CoreException("Wrong name '%' of the sample by sample process", processText.c_str());
                }
                sampleProcess->stepByStepProcessingList.push_back(stepProcess);
            }
            if (sampleProcess->stepByStepProcessingList.size()) {
                filter->sampleBySampleProcessingList.push_back(sampleProcess);
            }
        }
        if (filter->sampleBySampleProcessingList.size()) {
            _filterbank.push_back(filter);
        }
    }
}

TFilterBankTimeShiftDetection::TFilterBankTimeShiftDetection(XMLNode& configNode, const std::string& filterPath, TSequentialDetection& det, CORE::TIME::QMLTimeMicroseconds istep) : TFilterBankDetection(det) {
    //std::string steptText = configNode.attribute("step");
    //_step = stoi(steptText);
    std::string timeshiftText = configNode.child("method").attribute("time_shift");
    maxTimeShift = stoi(timeshiftText);
    for (XMLNode filterNode = configNode.child("filter"); !filterNode.isNull(); filterNode = configNode.nextchild("filter")) {
        std::string fname = filterNode.attribute("file_name");
        timeshiftText = filterNode.attribute("time_shift");
        int timeshift = stoi(timeshiftText);
        if (maxTimeShift < timeshift) maxTimeShift = timeshift;
        std::shared_ptr<TFilterProcessingList> filter(new TFilterProcessingList());
        filter->doubleFilter.reset(new TDIIRFilterTimeShift(filterPath + fname, timeshift));
        for (XMLNode sampleBysample = filterNode.child("sample_by_sample_process"); !sampleBysample.isNull(); sampleBysample = filterNode.nextchild("sample_by_sample_process")) {
            std::string processText = sampleBysample.attribute("process");
            std::shared_ptr<TSampleBySampleProcessingList> sampleProcess(new TSampleBySampleProcessingList);
            if (processText == "covariance") {
				std::string aText = sampleBysample.attribute("offset_averaging");
				std::string bText = sampleBysample.attribute("covariance_averaging");
				double aVal = std::stod(aText);
				double bVal = std::stod(bText);
				sampleProcess->sampleBySampleProcessing.reset(new TCovariance(bVal, aVal, filter->doubleFilter->name()));
            }
            else if (processText == "STA/LTA") {
				std::string staText = sampleBysample.attribute("sta");
				std::string ltaText = sampleBysample.attribute("lta");
				double staVal = std::stod(staText);
				double ltaVal = std::stod(ltaText);
				sampleProcess->sampleBySampleProcessing.reset(new TStaLta(staVal, ltaVal, filter->doubleFilter->name()));
            }
            else {
                throw CORE::EXCEPTIONS::CoreException("Wrong name '%' of the sample by sample process", processText.c_str());
            }
            for (XMLNode stepBystep = sampleBysample.child("step_by_step_process"); !stepBystep.isNull(); stepBystep = sampleBysample.nextchild("step_by_step_process")) {
                std::string processText = stepBystep.attribute("process");
                TStepByStepProcessing_SP stepProcess;
                if (processText == "polarisation_Fisher") {
                    if (sampleProcess->sampleBySampleProcessing->name().substr(0, 10) != "Covariance") throw CORE::EXCEPTIONS::CoreException("The process '%' is not preceded by the covariance process", processText.c_str());
                    stepProcess.reset(new TPolarisationLambda(filter->doubleFilter->name()));
                }
                else if (processText == "polarisation_lambda_3D") {
                    if (sampleProcess->sampleBySampleProcessing->name().substr(0, 10) != "Covariance") throw CORE::EXCEPTIONS::CoreException("The process '%' is not preceded by the covariance process", processText.c_str());
                    stepProcess.reset(new TPolarisationLambda3D(filter->doubleFilter->name()));
                }
				else if (processText == "degree of polarization") {
					if (sampleProcess->sampleBySampleProcessing->name().substr(0, 10) != "Covariance") throw CORE::EXCEPTIONS::CoreException("The process '%' is not preceded by the covariance process", processText.c_str());
					stepProcess.reset(new TDegreeOfPolarization(filter->doubleFilter->name()));
				}
				else if (processText == "STA/LTA") {
					if (sampleProcess->sampleBySampleProcessing->name().substr(0, 7) != "STA/LTA") throw CORE::EXCEPTIONS::CoreException("The process '%' is not preceded by the covariance process", processText.c_str());
					stepProcess.reset(new STADivLTA(filter->doubleFilter->name()));
				}
				else {
                    throw CORE::EXCEPTIONS::CoreException("Wrong name '%' of the sample by sample process", processText.c_str());
                }
                sampleProcess->stepByStepProcessingList.push_back(stepProcess);
            }
            if (sampleProcess->stepByStepProcessingList.size()) {
                filter->sampleBySampleProcessingList.push_back(sampleProcess);
            }
        }
        if (filter->sampleBySampleProcessingList.size()) {
            _filterbank.push_back(filter);
        }
    }
    microsecondsTimeShift = istep * maxTimeShift;
    detection.setTimeShift(microsecondsTimeShift);
}

void TFilterBankDetection::push(CORE::DATA::SingleCW* ic, char c) {
    d3.push(ic, c);
    while (d3.ready()) {
        auto d3out = d3.pop();
        auto begin = d3out->timeFrom;
        auto step = d3out->samplingPeriodUSEC();
        for (auto&& fsample : d3out->samples) {
			if (begin.mod(_step) < step) {
                if (sample.empty()) {
                    for (auto&& f : _filterbank) {
                        f->proc(fsample);
                        for (auto dsample : f->out) {
                            sample.push_back(dsample);
                        }
                    }
                }
                else {
                    int idx = 0;
                    for (auto&& f : _filterbank) {
                        f->proc(fsample);
                        for (auto dsample : f->out) {
                            sample[idx++] = dsample;
                        }
                    }
                }
                detection.proc(sample, begin);
            }
			else {
				for (auto&& f : _filterbank) {
					f->push(fsample);
				}
			}
            begin += step;
        }
    }
}
