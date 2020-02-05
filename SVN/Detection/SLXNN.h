//---------------------------------------------------------------------------

#ifndef sp_SLXNNH
#define sp_SLXNNH
//---------------------------------------------------------------------------

#include <math.h>
#include "Exceptions.h"
#include "ANN_CONST.h"
//---------------------------------------------------------------------------

#define SLXNNMaxRecurrentNeurals 16
#define SLXNNMaxInputs 256
#define SLXNNBETA ANNBETA
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
//  vSize <= SLXNNMaxReccurentInputs
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

class TSLXNNException : public CORE::EXCEPTIONS::CoreException
{
public:
    TSLXNNException() : CORE::EXCEPTIONS::CoreException("SLXNN exception") {}
    TSLXNNException(const std::string& what) : CORE::EXCEPTIONS::CoreException("SLXNN: %s", what.c_str()) {}
} ;
//---------------------------------------------------------------------------

template <class T, class T_in> class TSLXNN
{
protected:
    int _noDelays; //* Recurrent order
    int _vSize; //* Size of recurrent neurons
    int _inputSize; //* Size of inputs of the network (not neurons)
    int _recurrentSize; //* Size of recurrent inputs of neurons
    int _inputNeuronSize; //* Size of inputs of neurons

    T** _Delays = NULL;
    T _V[SLXNNMaxInputs] ; //* Inputs of neural neurons
    T _weight[SLXNNMaxRecurrentNeurals][SLXNNMaxInputs]; //* Weights of neural network

    void initValues();
    void clear();
    virtual T _fun(T iv)  {return tanh(iv*SLXNNBETA) ; }
public:
    TSLXNN(T** iw = NULL, int vSize = 1, int inputSize = 1, int delaySize = 0, int* delays = NULL);
    virtual ~TSLXNN() { clear(); }
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
