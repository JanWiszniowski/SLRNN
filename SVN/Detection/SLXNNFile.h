#pragma once

#include "autofile.h"
#include "SLXNN.h"

namespace CORE { namespace Seismic_Precessing {
//---------------------------------------------------------------------------
class TSLXNNFile : public TSLXNN < double, double > {
public:
    TSLXNNFile(std::string fileName);
};

//---------------------------------------------------------------------------
}} // namespace Seismic_Precessing
//---------------------------------------------------------------------------
