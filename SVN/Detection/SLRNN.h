//---------------------------------------------------------------------------

#ifndef sp_SLRNNH
#define sp_SLRNNH
//---------------------------------------------------------------------------

#include <math.h>
#include "Exceptions.h"
#include "ANN_CONST.h"
//---------------------------------------------------------------------------

#define SLRNNMaxRecurrentNeurals 16
#define SLRNNMaxInputs 256
#define SLRNNBETA ANNBETA
//---------------------------------------------------------------------------
//
//  V...
// |---|---|---|...|---|---|---|...|---|---|
// |   |   |   |   |   | 1 |   |   |   |   |
// |---|---|---|...|---|---|---|...|---|---|
//
//  \  | | | | | | /
//
//  O   O   O   O
// |-------------|
//  vSize <= SLRNNMaxReccurentInputs
// |-------------|
// |-------------------|
//  recurrentSize = vSize * dalaySize
// |-------------------|
//                     |---|
//                      bias = const = 1
//                     |---|
//                         |---------------|
//                          inputSize
//                         |---------------|
// |---------------------------------------|
//  inputNeuronSize = vSize * delaySize + 1 + inputSize <= SLRRNNMaxRecurrentNeurals
// |---------------------------------------|
//
// vSize      - number of neurons
// inputSize  - number of inputs of the network excluding bias
// delaySize  - number recurrent delays
// delays     - delays
//
//---------------------------------------------------------------------------

namespace CORE { namespace Seismic_Precessing {
//---------------------------------------------------------------------------

class TSLRNNException : public CORE::EXCEPTIONS::CoreException
{
public:
    TSLRNNException() : CORE::EXCEPTIONS::CoreException("SLRNN exception") {}
    TSLRNNException(const std::string& what) : CORE::EXCEPTIONS::CoreException("SLRNN: %s", what.c_str()) {}
} ;
//---------------------------------------------------------------------------

template <class T, class T_in> class TSLRNN
{
protected:
    int _noDelaysPos;
    int _noDelays;
    int _vSize; //* Size of recurrent neurals
    int _inputSize; //* Size of inputs of the network (not neurons)
    int _recurrentSize;
    int _inputNeuronSize; //* Size of inputs of neurons

    int* _delaysPos = NULL;
    T** _Delays = NULL;
    T _V[SLRNNMaxInputs] ; //* Inputs of neural neurons
    T _weight[SLRNNMaxRecurrentNeurals][SLRNNMaxInputs]; //* Weights of neural network

    void initValues();
    void clear();
    virtual T _fun(T iv)  {return tanh(iv*SLRNNBETA) ; }
public:
    TSLRNN(T** iw = NULL, int vSize = 1, int inputSize = 1, int delaySize = 0, int* delays = NULL);
    virtual ~TSLRNN() { clear(); }
    void reset();
    void process(const T_in* iptr);
    int VSize() const { return _vSize ; }
    int input_size() const { return _inputSize ; }
    T output(int i) const { return _Delays[0][i]; }

} ;
//---------------------------------------------------------------------------
}} // namespace Seismic_Precessing
//---------------------------------------------------------------------------
#endif
