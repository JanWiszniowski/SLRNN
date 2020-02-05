#include <string>
#include "IBasePlugin.h"
#include "ArcLinkPlugin.h"
#include "file_sc_xml.h"
#include "LoadDetections.h"

namespace CORE { namespace PLUGINS {

	class ExportANNDetection : public IBasePlugin {
        std::string outptPath;
        ArcLinkPlugin* preproc();
        CORE::TIME::QMLTime noiseTime;
        CORE::TIME::QMLTimeMicroseconds before;
        CORE::TIME::QMLTimeMicroseconds after;
        CORE::QUAKEML::WaveformStreamID stream;
	public:
        // -- Action --
        virtual void process(wxCommandEvent& ev);
        void processTimeShift(XMLNode& configNode);
        void processNoTimeShift(XMLNode& configNode);
        virtual void processDetTest(wxCommandEvent& ev);
//        void processDetTestTimeShift(XMLNode& configNode);
        void processDetTestNoTimeShift(XMLNode& configNode, XMLNode& rootNode);
        virtual void processDetTest2(wxCommandEvent& ev);
        void processDetTest2NoTimeShift(XMLNode& configNode, XMLNode& rootNode);
        void doDetection(wxCommandEvent& ev);
        void doDetectedSignals(wxCommandEvent& ev);
        void testDetection(wxCommandEvent& ev);
        bool evtDetected(CORE::QUAKEML::Event&, const TDetection&, int votesNumber = 2);
        void setOutput(wxCommandEvent& ev);
        void processNoisesList(wxCommandEvent& ev);
        void saveNoise(wxCommandEvent& ev);
        void saveUndetected(wxCommandEvent& ev);
        void getNoise(wxCommandEvent& ev);
        void listNoises(wxCommandEvent& ev);
        void doVotingList(wxCommandEvent& ev);
        void doViewList(wxCommandEvent& ev);
        std::string getPluginName() override { return "ExportANNDetection"; }
        int getPluginVersion() override { return 1; }
        unsigned int getPluginType() override { return PluginType::PT_BASE; }
        unsigned int getPluginInputType() override { return CORE::PLUGINS::PIT_INDEPENDENT_INPUT; }
        unsigned int getPluginOutputType() override { return CORE::PLUGINS::POT_WAVEFORM; }
        void defineMenu() override;
        ExportANNDetection();
        ~ExportANNDetection();
	};

}}
