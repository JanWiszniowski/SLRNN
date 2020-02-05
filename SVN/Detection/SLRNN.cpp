//---------------------------------------------------------------------------
#include "stdafx.h"
#include "SLRNN.h"

//---------------------------------------------------------------------------
namespace CORE { namespace Seismic_Precessing {
//---------------------------------------------------------------------------

template <class T, class T_in> TSLRNN<T, T_in>::TSLRNN(T** iw, int vSize, int inputSize, int delaySize, int* delays)
    : _noDelaysPos(delaySize), _vSize(vSize), _inputSize(inputSize), _recurrentSize(delaySize*vSize), _inputNeuronSize(vSize*delaySize + 1 + inputSize)
{
    if (iw == NULL) return;
    if (_noDelaysPos == 0 || delays == NULL) {
        _delaysPos = new int[1];
        _noDelaysPos = 1;
        _delaysPos[0] = 1;
    }
    else {
        _delaysPos = new int[_noDelaysPos];
        for (int i = 0; i < _noDelaysPos; i++) {
            _delaysPos[i] = delays[i];
        }

    }
    initValues();
    for(int i = 0 ; i < SLRNNMaxInputs ; i++) {
        for(int j = 0 ; j < SLRNNMaxRecurrentNeurals ; j++) {
            if ((i < _inputNeuronSize) && (j < _vSize)) {
                _weight[j][i] = iw[j][i] ;
            } else {
                _weight[j][i] = 0 ;
            }
        }
    }
    reset() ;
}

//---------------------------------------------------------------------------
template <class T, class T_in> void TSLRNN<T, T_in>::initValues() {
    _recurrentSize = _vSize*_noDelaysPos;
    _inputNeuronSize = _vSize*_noDelaysPos + 1 + _inputSize;
    if ((_vSize > SLRNNMaxRecurrentNeurals) || (_inputNeuronSize > SLRNNMaxInputs)) throw TSLRNNException("Weights size limit");
    _noDelays = _delaysPos[_noDelaysPos - 1] + 1;
    _Delays = new T*[_noDelays];
    for (int i = 0; i < _noDelays; i++) {
        _Delays[i] = new T[_vSize];
    }

}
//---------------------------------------------------------------------------
template <class T, class T_in> void TSLRNN<T, T_in>::clear() {
    delete[] _delaysPos;
    if (_Delays == NULL) return;
    for (int i = 0; i < _noDelays; i++) {
        delete[] _Delays[i];
    }
    delete[] _Delays;

}

//---------------------------------------------------------------------------
template <class T, class T_in> void TSLRNN<T,T_in>::reset()
{
    _V[_recurrentSize] = 1.0;
   for (int i = 0; i < _noDelays; i++) {
       for (int j = 0; j < _vSize; j++) {
           _Delays[i][j] = 0.0;
       }
   }
}
//---------------------------------------------------------------------------

template <class T, class T_in> void TSLRNN<T,T_in>::process(const T_in* iptr)
{
    T* lastDelay = _Delays[_noDelays - 1];
    for (int i = _noDelays-1; i > 0; i--) {
        _Delays[i] = _Delays[i - 1];
    }
    _Delays[0] = lastDelay;
    int idx = 0;
    for (int di = 0; di < _noDelaysPos; di++) {
        T* dPtr = _Delays[_delaysPos[di]];
        for (int i = 0; i < _vSize; i++) {
            _V[idx++] = dPtr[i];
        }
    }
    for (int i = _recurrentSize + 1; i < _inputNeuronSize; _V[i++] = *iptr++);
   // #pragma omp parallel for default(none)
   // firstprivate(template <class T> TSLRNN<T>::V) shared(weight, out)
   for(int i = 0 ; i < _vSize ; i++) {
      T sum = 0.0 ;
      for (int j = 0; j < _inputNeuronSize; j++) {
         sum += _weight[i][j] * _V[j] ;
      }
      lastDelay[i] = _fun(sum);
   }
}
//---------------------------------------------------------------------------

// template class TSLRNN<float>;
template class TSLRNN<double,float>;
template class TSLRNN<double,double>;
//---------------------------------------------------------------------------

}} // namespace Seismic_Precessing
//---------------------------------------------------------------------------
