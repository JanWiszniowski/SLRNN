#define ANNDETECTOR_VERSION "SWIP5 command line v5.0.0"

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
#include "Exceptions.h"
#include <wx/filedlg.h>
#include <wx/cmdline.h>
#include <boost/filesystem.hpp>
#include "Logging.h"
#include "TANNDetector.h"


class TConsoleBroadcaster : public CORE::LOGGING::TBroadcaster {
public:
    void PushLog(enum CORE::LOGGING::LoggingLevels level, const std::string &date, const std::string& message) override;
    int viewLevel = 2;
    char colors[6][16];
    TConsoleBroadcaster();
};

class MyApp : public wxAppConsole
{
public:
    //wxLocale *locale;
    TConsoleBroadcaster broadcaster;
    CORE::LOGGING::Logger logger;
    TANNParameters parameters;
    bool OnInit() override;
    int OnRun() override;
    int OnExit() override;
    void OnInitCmdLine(wxCmdLineParser& parser) override;
    bool OnCmdLineParsed(wxCmdLineParser& parser) override;
    MyApp();
};


int MyApp::OnExit() {
    return 0;
}

MyApp::MyApp() : wxAppConsole() {
    #if defined(__GNUC__)
    //XInitThreads();
    #endif
}

bool MyApp::OnInit() {
    logger.bindWithBroadcaster(&broadcaster);
    try {
        logger.setLogfilePath(boost::filesystem::path("Logs/swipc.log").native());
        logger.setLoggingToFile(true);
        logger.setFileIgnoreLevels(true);
        logger.setLowestLoggingLevel(CORE::LOGGING::LL_DEBUG);
        if (!wxAppConsole::OnInit()) return false;
        if (parameters.configPath.empty()) throw CORE::EXCEPTIONS::CoreException("Missing config path");
        if (parameters.inputName.empty()) throw CORE::EXCEPTIONS::CoreException("Missing input file name");
        if (parameters.outputName.empty()) throw CORE::EXCEPTIONS::CoreException("Missing output file name");
    }
    catch (std::exception& e) {
        logger << CORE::LOGGING::LL_ERROR << e.what();
        return false;
    }
    return true;
}

int MyApp::OnRun() {
    try {
        TANNDetector detector(logger);
        detector.run(parameters);
        return 0;
    }
    catch (const CORE::EXCEPTIONS::CoreException& e) {
        logger.c_log(CORE::LOGGING::LL_ERROR, "ANNDetector run error: %s", e.what());
        return -1;
    }
    catch (const std::exception& e) {
        logger.c_log(CORE::LOGGING::LL_ERROR, "ANNDetector run system error: %s", e.what());
        return -1;
    }
    catch (...) {
        logger.c_log(CORE::LOGGING::LL_ERROR, "ANNDetector run unknown error");
        return -1;
    }
    return 0;
}

void MyApp::OnInitCmdLine(wxCmdLineParser& parser) {
    static const wxCmdLineEntryDesc g_cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, "h", "help", "help", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_OPTION, "v", "verbose", "increase the verbosity", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
        { wxCMD_LINE_OPTION, "c", "config", "config path", wxCMD_LINE_VAL_STRING , wxCMD_LINE_PARAM_OPTIONAL },
        { wxCMD_LINE_OPTION, "i", "input", "input mutliplexed file name", wxCMD_LINE_VAL_STRING , wxCMD_LINE_PARAM_OPTIONAL },
        { wxCMD_LINE_OPTION, "o", "output", "output file name", wxCMD_LINE_VAL_STRING , wxCMD_LINE_PARAM_OPTIONAL },
        { wxCMD_LINE_NONE }
    };
    parser.SetDesc(g_cmdLineDesc);
    parser.SetSwitchChars(wxT("-"));
}

bool MyApp::OnCmdLineParsed(wxCmdLineParser& parser) {
    wxString value;
    long ivalue;
    if (parser.Found(wxT("v"),&ivalue)) {
        //if (broadcaster.viewLevel > 0) broadcaster.viewLevel--;
        if ( ivalue >= 0 && ivalue < 4) broadcaster.viewLevel = ivalue;
        logger.c_log(CORE::LOGGING::LL_REPORT,"Log level %d",broadcaster.viewLevel);
    }
    if (parser.Found(wxT("c"), &value)) {
        parameters.configPath = value.ToStdString();
        logger.c_log(CORE::LOGGING::LL_REPORT,"Config path '%s'",value.ToStdString().c_str());
    }
    if (parser.Found(wxT("i"), &value)) {
        parameters.inputName = value.ToStdString();
        logger.c_log(CORE::LOGGING::LL_REPORT,"Input file '%s'",value.ToStdString().c_str());
    }
    if (parser.Found(wxT("o"), &value)) {
        parameters.outputName = value.ToStdString();
        logger.c_log(CORE::LOGGING::LL_REPORT,"Output file '%s'",value.ToStdString().c_str());
    }
    return true;
}

TConsoleBroadcaster::TConsoleBroadcaster() {
    for (auto c : colors) c[0] = 0;
    std::ifstream colorsStream("./Config/Colors.def");
    if (!colorsStream) return;
    char line[16];
    for (auto c : colors) {
        colorsStream.getline(line, 16);
        if (colorsStream.good()) {
            strncpy(c, line, 16);
        }
        else {
            break;
        }
    }
}

void TConsoleBroadcaster::PushLog(enum CORE::LOGGING::LoggingLevels level, const std::string &date, const std::string& message) {
    if (level >= viewLevel) {
        if(level > CORE::LOGGING::LL_REPORT) {
            std::cerr << colors[level] << "[" << CORE::LOGGING::LoggingLevelToString(level) << "] " << message << colors[5] << std::endl;
        }
        else {
            std::cerr << colors[level] << message << colors[5] << std::endl;
        }
    }
}


wxIMPLEMENT_APP(MyApp);
