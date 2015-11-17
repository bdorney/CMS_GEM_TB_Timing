//
//  TimingRunAnalyzer.h
//  
//
//  Created by Brian L Dorney on 31/10/15.
//
//  This class analyzes timing runs taken in GEM test beams
//  A user of this framework is free to write their own class; but it should inherit from this one!!!
//  The user authored class may write their own methods.
//  However the user must override the following method at a minimum:
//
//      void analyzeRun();
//
//  The actions of the users analysis must be executed through the analyze() method.
//  Of course the users analyze() method can call other methods/functions;
//  but the users analysis must start and end in their analyze method.
//  AND it must manipulate the Timing::Run run data member of this base class
//

#ifndef ____TimingRunAnalyzer__
#define ____TimingRunAnalyzer__

//C++ Includes
#include <algorithm>
//#include <cmath>
#include <fstream>
//#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <stdio.h>
#include <string>
#include <utility>
#include <vector>

//My Incldues
#include "TimingUtilityFunctions.h"
#include "TimingUtilityOperators.h"
#include "TimingUtilityTypes.h"
//#include "TRunParameters.h"   //Unused atm

//ROOT Includes
#include "TF1.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TSpectrum.h"
#include "TTree.h"

//For working with stl containers and TTree
#ifdef __MAKECINT__
#pragma link C++ class vector<int>+;
//#pragma link C++ class vector<float>+;
#pragma link C++ class map<string,int>+;
#endif

using namespace ROOT;

namespace Timing {
    class TimingRunAnalyzer {
    public:
        //Constructor
        TimingRunAnalyzer();
        
        //Data members
        AnalysisSetup aSetup;
        
        //Actions - Methods that DO Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        //Analyze the input run
        virtual void analyzeRun(Timing::Run &run);
        //virtual void analyzeRun(Timing::Run &runInput){
        //    setRun(runInput);
        //    analyzeRun();
        //    return;
        //}
        //virtual void analyzeRun(Timing::TRunParameters &inputRunParam){
        //    setRun(inputRunParam);
        //    analyzeRun();
        //    return;
        //}
        
        //Getters - Methods that Get (i.e. Return) Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        virtual Timing::AnalysisSetup getAnalysisSetup(){return aSetup;};
        
        //virtual Timing::Run getRun(){return run;};
        
        //Miscillaneous Methods
        //------------------------------------------------------------------------------------------------------------------------------------------
        //virtual void bkgSubtract(bool );
        
        //Printers - Methods that Print Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        
        //Setters - Methods that Set Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        //Analysis Config File
        virtual void setAnalysisConfig(std::string &strInputFile);
        
        //virtual void setRun(Timing::Run &runInput){run = runInput; bRunSet = true; return;};
        //virtual void setRun(Timing::TRunParameters &inputRunParam){run = inputRunParam.getRun(); bRunSet = true; return;};
        
        virtual void setTDCResolution(float fInput){
            fTDCResolution = fInput;
            return;
        };
        
        virtual void setVerboseModeAnalysis(bool bInputMode){bVerboseMode_Analysis = bInputMode;return;};
        virtual void setVerboseModeIO(bool bInputMode){bVerboseMode_IO = bInputMode;return;};
        
    private:
        //Data members
        bool bRunSet;               //Has the user set the run?
        
        bool bVerbose_Logging;      //Make a log file of all messages? (not yet implemented)
        bool bVerboseMode_Analysis; //Output analysis messages to user
        bool bVerboseMode_IO;       //Output file I/O messages to the user
        
        float fTDCResolution;       //TDC Resolution in nanoseconds
        
        //int iNEvtSel;               //Number of Selected events in the run
        
        std::fstream fStream_Log;   //Logging File (not yet implemented)
        
        std::string strSecBegin_ANAL;   //Analysis section header beginning
        std::string strSecBegin_AND;    //Detector AND  "                   "
        std::string strSecBegin_DET;    //Detector "                        "
        std::string strSecBegin_OR;     //Detector OR   "                   "
        std::string strSecBegin_PMT;    //Detector PMT "                    "
        
        std::string strSecEnd_ANAL;   //As above but for the ending
        std::string strSecEnd_AND;
        std::string strSecEnd_DET;
        std::string strSecEnd_OR;
        std::string strSecEnd_PMT;
        
        //TRunParameters runLogger
        //Timing::Run run;            //The run!!!
        
        //Actions - Methods that DO Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        //Fit Histogram
        //Good Candidate for TimingUtilityFunctions?
        virtual void fitHistogram(HistoSetup &setupHisto, TH1F & hInput, TF1 &funcInput);
        
        //Used in event selection
        //Returns false (true) if Det's are in (out of) time with trigger based on specified user input
        virtual bool rejectEvtDetsOOT(AnalysisSetup &aSetup, std::map<std::string,int> mapInputPMTData, std::map<std::string,int> mapInputDetData);

        //Used in event selection
        //Returns false (true) if PMT's are in (out of) time based on specified user input
        virtual bool rejectEvtPMTsOOT(AnalysisSetup &aSetup, std::map<std::string,int> mapInputPMTData);
        
        //Getters - Methods that Get (i.e. Return) Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        //Inverts a time recorded by the TDC by the TDC Window (e.g. due to common_stop technique)
        virtual int getInvertedTime(int iInput){
            return (iInput > 0) ? (aSetup.fTDCWinSize - iInput) : iInput;
        };

        //TF1 getFunction(HistoSetup &setupHisto, TH1F & hInput);
        TF1 getFunction(HistoSetup &setupHisto, TH1F & hInput, Timing::Run & run);
        //TH1F getHistogram(HistoSetup &setupHisto);
        TH1F getHistogram(HistoSetup &setupHisto, Timing::Run & run);
        
        //Miscillaneous Methods
        //------------------------------------------------------------------------------------------------------------------------------------------
        
        //Printers - Methods that Print Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        
        //Setters - Methods that Set Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        //Set Histo Setup
        virtual void setHistoSetup(std::string &strInputFile, std::ifstream &fStream, HistoSetup &setupHisto);

        //Set Detector Data after all events of a given run have been analyzed
        //  ->timingResults struct containing detector & fit data
        //  ->hInput Detector Histogram
        //  ->funcInput fit of detector histogram
        virtual void setPerformanceData(TDCAnalysisResults &inputTimingResults, TH1F &hInput);
        virtual void setPerformanceData(TDCAnalysisResults &inputTimingResults, TF1 &funcInput, HistoSetup &setupHisto);
        
    //protected:
        
        //Something goes here?
        
    }; //End class TestBeamRunAnalyzer
} //End namespace Timing

#endif /* defined(____TestBeamRunAnalyzer__) */
