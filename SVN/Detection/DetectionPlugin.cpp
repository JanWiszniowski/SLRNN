// Win32Project1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <wx/wx.h>
#include "Exceptions.h"
#include "PluginInterface/IBasePlugin.h"
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>


namespace CORE {
    namespace PLUGINS {
        
        class DetectionStations

        
        class DetectionPlugin : public IBasePlugin {
        public:
            // -- Action --
            void processANNDetection(wxCommandEvent& ev) {

                // Lock access if you want read data from quakeML
                //CORE::QUAKEML::QML_lock_write ql1(*sdc->quakeML);

                // Lock access if you want write data to quakeML
                //CORE::QUAKEML::QML_lock_read ql2(*sdc->quakeML);

                sdc->logger << "<<<<ANN Detection!";
                map<
                for (auto&& chan : sdc->seismicChannels) {
                    auto sta = chan->channelName.getStaName()
                }
            }
            void processNoiseDetection(wxCommandEvent& ev) {
                sdc->logger << std::string("<<<<TEST Noise Detection");
            }

            // -- IBasePlugin funtions that have to be override --
            std::string getPluginName() override { return "DetectionPlugin"; }
            int getPluginVersion() override { return 1; }
            unsigned int getPluginType() override { return PluginType::PT_BASE; }
            unsigned int getPluginInputType() override { return CORE::PLUGINS::PIT_INDEPENDENT_INPUT; }
            unsigned int getPluginOutputType() override { return CORE::PLUGINS::POT_WAVEFORM; }

            // -- IBasePlugin funtions that don't usualy have to be override (commented out) --
            // bool isTypeOf(enum PluginType type) override { return type & PluginType::PT_BASE; }
            // std::vector<PluginDependency*> getPluginDependencies() override { return pd; }
            // virtual int getProgress() override { return this->progress; }

            // -- IBasePlugin funtions that rather shouldn't be override (commented out) --
            // std::vector<PluginMenuWrapper *> getMenuPositions() { if (mp.empty()) defineMenu(); return mp; }
            // void setSharedDataContainer(CORE::DATA::SharedDataContainer* ptr) { this->sdc = ptr; }
            // int getProgress() { return this->progress; } // -1 gdy nie zwraca postêpu

            // -- IBasePlugin funtion that should be override --
            //    because constructor doesn't set the pointer to the SharedDataContainer,
            //    which is requared for PluginMenuWrappers,
            //    the PluginMenuWrappers have to be defined in getMenuPositions or setSharedDataContainer, e.g.
            void defineMenu() override {
                mp.push_back(PluginMenuWrapper::withoutGUI(sdc, "Create ANN detection ", boost::bind(&DetectionPlugin::processANNDetection, this, _1), boost::bind(&DetectionPlugin::getProgress, this)));
                mp.push_back(PluginMenuWrapper::withoutGUI(sdc, "Create noise detection", boost::bind(&DetectionPlugin::processNoiseDetection, this, _1), boost::bind(&DetectionPlugin::getProgress, this)));
            }
            // -- 
            DetectionPlugin() {
            }

            ~DetectionPlugin() {
            }
        };

    }
}

#if defined(_MSC_VER)
__declspec(dllexport) void* getClass() {
#elif defined(__GNUC__)
void* getClass() {
#endif
    return static_cast< void* > (new CORE::PLUGINS::DetectionPlugin);
}
