#include "stdafx.h"
#include "SLRNNFile.h"
#include <fstream> 
#include <vector>
namespace CORE { namespace Seismic_Precessing {
//---------------------------------------------------------------------------
    template<class T> int getline(std::vector<T>& numbers, std::ifstream& file) {
        std::string line;
        numbers.clear();
        T number;
        do {
            if (!file.good()) return 0;
            getline(file, line);
        } while (line.empty() || line[0] == '#');
        std::istringstream iss(line);
        while (iss >> number) {
            numbers.push_back(number);
        }
        return numbers.size();
    }

    TSLRNNFile::TSLRNNFile(std::string fileName) {
        try {
            std::vector<int> iv;
            std::vector<double> dv;
            std::ifstream file(fileName);
            if (getline(iv, file) == 0) throw TSLRNNException("can not read no inputs");
            _inputSize = iv[0];
            if (getline(iv, file) == 0) throw TSLRNNException("can not read no neurons");
            _vSize = iv[0];
            if (getline(iv, file) == 0) throw TSLRNNException("can not read no delays");
            _noDelaysPos = iv.size();
            _delaysPos = new int[_noDelaysPos];
            for (int i = 0; i < _noDelaysPos; i++) {
                _delaysPos[i] = iv[i];
            }
            initValues();
            int index = 0;
            for (int i = 0; i < _noDelaysPos*_vSize; i++) {
                if (getline(dv, file) != _vSize) throw TSLRNNException("can not read recurrent weights");
                for (int j = 0; j < _vSize; j++) {
                    _weight[j][index] = dv[j];
                }
                index++;
            }
            if (getline(dv, file) != _vSize) throw TSLRNNException("can not read bias weights");
            for (int j = 0; j < _vSize; j++) {
                _weight[j][index] = dv[j];
            }
            index++;
            for (int i = 0; i < _inputSize; i++) {
                if (getline(dv, file) != _vSize) throw TSLRNNException("can not read input weights");
                for (int j = 0; j < _vSize; j++) {
                    _weight[j][index] = dv[j];
                }
                index++;
            }
            reset();
        }
        catch (const TSLRNNException& e) {
            clear();
            throw e;
        }
    }

//---------------------------------------------------------------------------
}} // namespace Seismic_Precessing
//---------------------------------------------------------------------------
