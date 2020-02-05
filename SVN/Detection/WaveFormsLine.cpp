#include "stdafx.h"
#include "WaveFormsLine.h"
//---------------------------------------------------------------------------

TripleCW_SP TWaveFormsLine::pop() {
    if (!ready()) return TripleCW_SP(nullptr);
	unsigned size = static_cast<unsigned>((_end - _begin) /_step);
    TripleCW_SP ret(new TripleCW);
    ret->sType = CORE::DATA::SampleTypeName::ST_TRIPLE;
    ret->samples.resize(size);
    for (int lineidx = 0; lineidx < 3; lineidx++) {
        unsigned outidx = 0;
        auto& ch = _line[lineidx];
        for (auto wf = ch.begin(); wf != ch.end() && outidx < size;) {
            auto wfref = (*wf);
            ret->givenSamplingPeriod = wfref->givenSamplingPeriod;
            unsigned insize = wfref->samples.size();
            if (wfref->timeTo <= _begin) {
                wf = ch.erase(wf);
			} else {
				unsigned inidx = 0;
                if (wfref->timeFrom < _begin) {
                    inidx = static_cast<unsigned>((_begin - wfref->timeFrom) / _step);
                }
                if (wfref->timeFrom > _begin) {
                    outidx = static_cast<unsigned>((wfref->timeFrom - _begin) / _step);
                }
                if (wfref->timeTo <= _end) {
					while(inidx < insize) {
                        ret->samples[outidx++][lineidx] = wfref->samples[inidx++];
					}
                    wf = ch.erase(wf);
				} else {
                    insize -= static_cast<unsigned>((wfref->timeTo - _end) / _step);
					while(inidx < insize) {
                        ret->samples[outidx++][lineidx] = wfref->samples[inidx++];
					}
				}
			}
		}
	}

    ret->timeFrom = _begin;
    ret->timeTo = _end;
    _begin = _end;
	return ret;
}

void TWaveFormsLine::push_chan(CORE::DATA::SingleCW* waveform, std::list<CORE::DATA::SingleCW_SP>& line) {
    if (!waveform) return;
    // Continuity test
    CORE::TIME::QMLTime end_time;
    if (line.empty()) {
        end_time = _end;
    }
    else {
        end_time = line.back()->timeTo;
    }
    if (waveform->timeFrom == end_time) { // Neither gap nor overlay
        line.push_back(CORE::DATA::SingleCW_SP(new CORE::DATA::SingleCW(*waveform)));
    }
    else {
        if (waveform->timeFrom > end_time) { // Gap action

            if (!_begin.exists() || _begin < waveform->timeFrom) {
                _begin = waveform->timeFrom;
            }
            for (auto&& ch : _line) {
                if (ch.size()) {
                    if (ch.back()->timeTo < _begin) {
                        ch.clear();
                    }
                    else {
                        for (auto chiter = ch.begin(); chiter != ch.end() && (*chiter)->timeTo < _begin; chiter = ch.erase(chiter));
                    }
                }
            }
            line.push_back(CORE::DATA::SingleCW_SP(new CORE::DATA::SingleCW(*waveform)));
        }
        else {
            // Overlay action
            if (waveform->timeTo > end_time) {
                unsigned beginidx = static_cast<unsigned>((end_time - waveform->timeFrom) / _step);
                unsigned endidx = waveform->samples.size();
                if (beginidx < endidx) {
                    CORE::DATA::SingleCW* wfptr = new CORE::DATA::SingleCW(*waveform);
                    for (unsigned i = beginidx; i < endidx; i++) {
                        wfptr->samples[i - beginidx] = wfptr->samples[i];
                    }
                    wfptr->samples.resize(endidx - beginidx);
                    wfptr->timeFrom = end_time;
                    line.push_back(CORE::DATA::SingleCW_SP(wfptr));
                }
            }
            // else ignore block;
        }
	}
}

void TWaveFormsLine::push(CORE::DATA::SingleCW* waveform, char component) {
    if (!waveform) return;
    if (!waveform->size()) return;
    if (_step == 0.0) {
        _step = static_cast<unsigned>((waveform->timeTo - waveform->timeFrom) / waveform->size());
    }
    switch (component) {
		case 'Z':
			push_chan(waveform,_line[0]);
		break;
		case 'N':
            push_chan(waveform, _line[1]);
		break;
		case 'E':
            push_chan(waveform, _line[2]);
		break;
	}
    if(waveform->timeTo > _end) {
		CORE::TIME::QMLTime temp_end = waveform->timeTo;
		for(auto&& ch : _line) {
			if(ch.empty()) {
				temp_end = _end;
				break;
			}
			if(ch.back()->timeTo < temp_end) {
				temp_end = ch.back()->timeTo;
			}
		}
		_end = temp_end;
	}
}


void TSampleBySampleProcessingList::push(const CORE::Seismic_Precessing::TVector3<double>& in) {
    sampleBySampleProcessing->process(in);
}
void TSampleBySampleProcessingList::proc(const CORE::Seismic_Precessing::TVector3<double >& in) {
    sampleBySampleProcessing->process(in);
    if (out.empty()) {
        for (auto&& p : stepByStepProcessingList) {
            auto sout = p->apply(sampleBySampleProcessing->out);
            for (auto&& v : sout) {
                out.push_back(v);
            }
        }

    }
    else {
        int idx = 0;
        for (auto&& p : stepByStepProcessingList) {
            auto sout = p->apply(sampleBySampleProcessing->out);
            for (auto&& v : sout) {
                out[idx++] = v;
            }
        }
    }
}

void TFilterProcessingList::push(const CORE::DATA::tripleSampleType& in) {
	auto fout = doubleFilter->apply(in);
	for (auto&& p : sampleBySampleProcessingList) {
		p->push(fout);
	}
}

void TFilterProcessingList::proc(const CORE::DATA::tripleSampleType& in) {
	auto fout = doubleFilter->apply(in);
    if (out.empty()) {
        for (auto&& p : sampleBySampleProcessingList) {
            p->proc(fout);
            for (auto&& p : p->out) {
                out.push_back(p);
            }
        }

    }
    else {
        int idx = 0;
        for (auto&& p : sampleBySampleProcessingList) {
            p->proc(fout);
            for (auto&& p : p->out) {
                out[idx++] = p;
            }
        }
    }
}

//---------------------------------------------------------------------------
//
//void TDemultiplex::push(TripleCW_SP& sp) {
//    if (!sp) return;
//    _line.push_back(sp);
//}
//
//MultipleCW_SP TDemultiplex::pop() {
//    if (_nout == 0 || _line.empty()) return MultipleCW_SP(nullptr);
//    auto ff = _line.front();
//    if (!ff->timeFrom.isEqual(_end, _step)) {
//        for (auto&& filter : _filterbank) {
//            filter->reset();
//        }
//    }
//    MultipleCW_SP ret(new MultipleCW());
//    ret->timeFrom = ff->timeFrom;
//    ret->timeTo = ff->timeTo;
//    ret->givenSamplingPeriod = ff->givenSamplingPeriod;
//    ret->sType = CORE::DATA::SampleTypeName::ST_MULTIPLE;
//    unsigned size = ff->size();
//    ret->samples.resize(size);
//    for (unsigned idx = 0; idx < size; idx++) {
//        auto&& in = ff->samples[idx];
//        auto&& out = ret->samples[idx];
//        out.resize(_nout);
//        int nout = 0;
//        for (unsigned fidx = 0; fidx < _nouts.size(); fidx++) {
//            auto&& filter = _filterbank[fidx];
//            auto fret = filter->apply(in);
//            for (unsigned i = 0; i < _nouts[fidx]; i++) {
//                out[nout++] = fret[i];
//            }
//        }
//    }
//    _end = ff->timeTo;
//    _line.pop_front();
//    return ret;
//}
//
//void TDemultiplex::addProcess(TSimple3DProcessing& filter, unsigned nout) {
//    if (_line.size()) {
//        throw CORE::EXCEPTIONS::CoreException("Can not add filter. Rien ne va plus");
//    }
//    _filterbank.push_back(std::shared_ptr< TSimple3DProcessing >(filter.clone()));
//    _nouts.push_back(nout);
//    _nout += nout;
//}

//---------------------------------------------------------------------------
void MovingWindowDetection::reset() {
    _window.clear();
}

void MovingWindowDetection::proc(const std::vector<double>& sample, const CORE::TIME::QMLTime& time) {
    _window.push_back(sample);
    if (_window.size() == _length) {
        preproc();

        int ix = 0;
        for (auto&& vect : _window) {
            procvect(vect, ix++);
        }
        postproc();
        _window.pop_front();
    }
}

//---------------------------------------------------------------------------
//
//void TSequentialDetection::push(MultipleCW_SP& in) {
//    if (!in) return;
//    if (!in->timeFrom.isEqual(_end, _step)) {
//        reset();
//    }
//    auto begin = in->timeFrom;
//    auto step = in->samplingPeriodUSEC();
//    for (auto&& sample : in->samples) {
//        if (begin.mod(_step) < step) {
//            proc(sample, begin);
//        }
//        begin += step;
//    }
//}

//---------------------------------------------------------------------------
