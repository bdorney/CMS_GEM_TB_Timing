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
#include <numeric>
#include <stdio.h>
#include <string>
#include <utility>
#include <vector>

//My Incldues
#include "TimingUtilityFunctions.h"
#include "TimingUtilityOperators.h"
#include "TimingUtilityTypes.h"

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
//#pragma link C++ class vector<int>+;
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
        
        //Good candidate for TimingUtilityTypes.h?
        struct HistoSetup{
            bool bFit_AutoRanging;  //Fit range determined automatically?
            
            float fHisto_xLower;  //lower x range of histo
            float fHisto_xUpper;  //upper x range of histo
            
            int iHisto_nBins;   //number of bins
            
            std::string strFit_Formula; //Fit formula
            std::string strFit_Name;    //
            std::string strFit_Option;
            
            std::string strHisto_Name;
            std::string strHisto_Title_X;
            std::string strHisto_Title_Y;
            
            std::vector<std::string> vec_strFit_ParamMeaning;
            std::vector<std::string> vec_strFit_ParamIGuess;
            
            HistoSetup(){
                bFit_AutoRanging = false;

                fHisto_xLower = 0;
                fHisto_xUpper = 1200;
                
                iHisto_nBins = 1200;
                
                
                strFitFormula = "[0]*[2]*sqrt(TMath::Pi()/2.)*(TMath::Erf( (12.5 + (x-[1])) / ([2] * sqrt(2.) ) ) - TMath::Erf( ((x-[1]) - 12.5) / ([2] * sqrt(2.) ) ) )";
                strFit_Name = "func_Timing";
                strFit_Option = "R";
                
                strHisto_Name = "histo_Timing";
                strHisto_Title_X = "time #left(ns#right)";
                strHisto_Title_Y = "Counts #left(N/ns#right)";
            };
        };
        
        struct AnalysisSetup{
            bool bCompute_OR;
            bool bCompute_AND;
            bool bInvertTime;
            bool bMatchArrivalTime;
            
            float fTDCWinSize;
            
            HistoSetup setupOR;
            HistoSetup setupAND;
            
            std::map<std::string, HistoSetup, map_cmp_str> map_DetSetup;
            
            AnalysisSetup(){
                bCompute_AND = bCompute_OR = bMatchArrivalTime = false;
                //bInvertTime = true;
                
                bInvertTime = false;
                
                fTDCWinSize = 1200;
            }
        } analysisSetup;
        
        //Actions - Methods that DO Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        //Analyze the input run
        virtual void analyzeRun();
        virtual void analyzeRun(Timing::Run runInput){
            setRun(runInput);
            analyzeRun();
            return;
        }
        virtual void analyzeRun(Timing::TRunParameters inputRunParam){
            setRun(inputRunParam);
            analyzeRun();
            return;
        }
        
        //Getters - Methods that Get (i.e. Return) Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        virtual Timing::Run getRun(){return run;};
        
        //Miscillaneous Methods
        //------------------------------------------------------------------------------------------------------------------------------------------
        
        //Printers - Methods that Print Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        
        //Setters - Methods that Set Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        //Analysis Config File
        virtual void setAnalysisConfig(std::string strInputFile);
        
        virtual void setRun(Timing::Run runInput){run = runInput; bRunSet = true; return;};
        virtual void setRun(Timing::TRunParameters inputRunParam){run = inputRunParam.getRun(); bRunSet = true; return;};
        
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
        
        std::fstream fStream_Log;   //Logging File (not yet implemented)
        
        std::string strSecBegin_ANAL;   //Analysis section header beginning
        std::string strSecBegin_AND;    //Detector AND  "                   "
        std::string strSecBegin_OR;     //Detector OR   "                   "
        std::string strSecBegin_DET;    //Detector "                        "
        
        std::string strSecEnd_ANAL;   //As above but for the ending
        std::string strSecEnd_AND;
        std::string strSecEnd_OR;
        std::string strSecEnd_DET;
        
        //TRunParameters runLogger
        Timing::Run run;            //The run!!!
        
        //Actions - Methods that DO Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        //Fit Histogram
        //Good Candidate for TimingUtilityFunctions?
        virtual void fitHistogram(HistoSetup &setupHisto, TH1F & hInput, TF1 &funcInput);
        
        //Invert the timing data
        
        //Getters - Methods that Get (i.e. Return) Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        //Inverts a time recorded by the TDC by the TDC Window (e.g. due to common_stop technique)
        virtual int getInvertedTime(int iInput){
            (iInput > 0) ? return (analysisSetup.fTDCWinSize - iInput) : return iInput;
        };

        TF1 getFunction(HistoSetup &setupHisto, TH1F & hInput);
        TH1F getHistogram(HistoSetup &setupHisto);
        
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
        virtual void setPerformanceData(TDCAnalysisData &inputTimingResults, TH1F &hInput);
        virtual void setPerformanceData(TDCAnalysisData &inputTimingResults, TF1 &funcInput);
        
    //protected:
        
        //Something goes here?
        
    }; //End class TestBeamRunAnalyzer
} //End namespace Timing

#endif /* defined(____TestBeamRunAnalyzer__) */
