//---------------------------------------------------------------------------
#include "stdafx.h"
#include "SLXNN.h"

//---------------------------------------------------------------------------
namespace CORE { namespace Seismic_Precessing {
//---------------------------------------------------------------------------

template <class T, class T_in> TSLXNN<T, T_in>::TSLXNN(T** iw, int vSize, int inputSize, int delaySize)
    : _vSize(vSize), _inputSize(inputSize), _noDelays(delaySize)
{
    if (iw == NULL) throw TSLXNNException("Missing weights");
    initValues();
    for(int i = 0 ; i < SLXNNMaxInputs ; i++) {
        for(int j = 0 ; j < SLXNNMaxRecurrentNeurals ; j++) {
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
template <class T, class T_in> void TSLXNN<T, T_in>::initValues() {
    _recurrentSize = _vSize * _noDelays;
    _inputNeuronSize = _recurrentSize + 1 + _inputSize;
    if ((_vSize > SLXNNMaxRecurrentNeurals) || (_inputNeuronSize > SLXNNMaxInputs)) throw TSLXNNException("Weights size limit");
    _Delays = new T*[_noDelays];
    for (int i = 0; i < _noDelays; i++) {
        _Delays[i] = new T[_vSize];
    }

}
//---------------------------------------------------------------------------
template <class T, class T_in> void TSLXNN<T, T_in>::clear() {
    if (_Delays == NULL) return;
    for (int i = 0; i < _noDelays; i++) {
        delete[] _Delays[i];
    }
    delete[] _Delays;

}

//---------------------------------------------------------------------------
template <class T, class T_in> void TSLXNN<T,T_in>::reset()
{
    _V[_recurrentSize] = 1.0;
   for (int i = 0; i < _noDelays; i++) {
       for (int j = 0; j < _vSize; j++) {
           _Delays[i][j] = 0.0;
       }
   }
}
//---------------------------------------------------------------------------

template <class T, class T_in> void TSLXNN<T,T_in>::process(const T_in* iptr)
{
    T* lastDelay = _Delays[_noDelays - 1];
    for (int i = _noDelays-1; i > 0; i--) {
        _Delays[i] = _Delays[i - 1];
    }
    _Delays[0] = lastDelay;
    int idx = 0;
    for (int di = 0; di < _noDelays; di++) {
        T* dPtr = _Delays[di];
        for (int i = 0; i < _vSize; i++) {
            _V[idx++] = dPtr[i];
        }
    }
    for (int i = _recurrentSize + 1; i < _inputNeuronSize; _V[i++] = *iptr++);
   // #pragma omp parallel for default(none)
   // firstprivate(template <class T> TSLXNN<T>::V) shared(weight, out)
   for(int i = 0 ; i < _vSize ; i++) {
      T sum = 0.0 ;
      for (int j = 0; j < _inputNeuronSize; j++) {
         sum += _weight[i][j] * _V[j] ;
      }
      lastDelay[i] = _fun(sum);
   }
}
//---------------------------------------------------------------------------

// template class TSLXNN<float>;
template class TSLXNN<double,float>;
template class TSLXNN<double,double>;
//---------------------------------------------------------------------------

}} // namespace Seismic_Precessing
//---------------------------------------------------------------------------
