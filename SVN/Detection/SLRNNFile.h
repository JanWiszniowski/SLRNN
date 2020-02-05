#pragma once

#include "autofile.h"
#include "SLRNN.h"

namespace CORE { namespace Seismic_Precessing {
//---------------------------------------------------------------------------
class TSLRNNFile : public TSLRNN < double, double > {
public:
    TSLRNNFile(std::string fileName);
};

//---------------------------------------------------------------------------
}} // namespace Seismic_Precessing
//---------------------------------------------------------------------------
