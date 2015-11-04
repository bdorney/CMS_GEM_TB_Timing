//
//  treeProducerTDC.h
//  
//
//  Created by Brian L Dorney on 15/01/15.
//
//

#ifndef ____treeProducerTDC__
#define ____treeProducerTDC__

//C++ Includes
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
//#include <tuple>
#include <utility>
#include <vector>

//My Includes
#include "TimingRunAnalyzer.h"
#include "TimingUtilityFunctions.h"
#include "TimingUtilityOperators.h"
#include "TimingUtilityTypes.h"
#include "TRunParameters.h"

//ROOT Includes
#include "TBranch.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TROOT.h"
#include "TSpectrum.h"
#include "TTree.h"

//For working with stl containers and TTree
#ifdef __MAKECINT__
//#pragma link C++ class vector<int>+;
#pragma link C++ class vector<float>+;
#pragma link C++ class map<string,float>+;
//#pragma link C++ class map<string,int>+;
#endif

using namespace ROOT;

namespace Timing {
    //Create a container like std::tuple, but 4 elements
    /*template<typename T1, typename T2, typename T3, typename T4>
    struct quad{
        T1 first;
        T2 second;
        T3 third;
        T4 fourth;
    };*/

    /*template<typename T1, typename T2, typename T3, typename T4>
    quad<T1,T2,T3,T4> make_quad(const T1 &m1, const T2 &m2, const T3 &m3, const T4 &m4){
        quad<T1,T2,T3,T4> quadInstance;
        quadInstance.first = m1;
        quadInstance.second = m2;
        quadInstance.third = m3;
        quadInstance.fourth = m4;
        
        return quadInstance;
    }*/
    
    //Create a container like std::tuple, but 5 elements
    template<typename T1, typename T2, typename T3, typename T4, typename T5>
    struct quint{
        T1 first;
        T2 second;
        T3 third;
        T4 fourth;
        T5 fifth;
    };
    
    template<typename T1, typename T2, typename T3, typename T4, typename T5>
    quint<T1,T2,T3,T4,T5> make_quint(const T1 &m1, const T2 &m2, const T3 &m3, const T4 &m4, const T5 &m5){
        quint<T1,T2,T3,T4,T5> quintInstance;
        quintInstance.first = m1;
        quintInstance.second = m2;
        quintInstance.third = m3;
        quintInstance.fourth = m4;
        quintInstance.fifth = m5;
        
        return quintInstance;
    }

    class treeProducerTDC {
    public:
        //Constructor
        treeProducerTDC();
        
        //Getters - Methods that Get (i.e. Return) Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        
        //Printers - Methods that Print Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        virtual void printStoredData(TTree *inputTree);
        
        //Setters - Methods that Set Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        virtual void setAnalyzer(Timing::TimingRunAnalyzer *inputAnalyzer){analyzer = inputAnalyzer; return;};
        
        //Set data File
        virtual void setFilesData(std::string inputFileName){fileName_Data = inputFileName; return;};
        
        virtual void setFitOption(std::string inputOption){fitOption = inputOption; return;};
        
        //virtual void setNumericDeconvoModel(std::string inputFormula, std::vector<float> inputParameters);
        
        virtual void setIgnoredParameter(std::string inputParameter);
        
        /*virtual void setHistoRebinFactor(int inputFactor){
            iRebinFactor = inputFactor;
            return;
        };*/
        
        //Set Verbose Print Modes
        virtual void setVerboseModeIO(bool inputMode){verbose_IO = inputMode; return;};
        virtual void setVerboseModeLUT(bool inputMode){verbose_LUT = inputMode; return;};
        virtual void setVerboseModePFN(bool inputMode){verbose_PFN = inputMode; return;};
        virtual void setVerboseModePkCalc(bool inputMode){verbose_PkCalc = inputMode; return;};
        virtual void setVerboseModePrintRuns(bool inputMode){verbose_PrintRuns = inputMode; return;};
        
        //Miscillaneous Methods
        //------------------------------------------------------------------------------------------------------------------------------------------
        virtual void clearIgnoredParameters(){ vecIgnoredParam.clear(); return; };
        
        //Loop over runs given in fileName_Data
        virtual void readRuns(std::string inputTreeName, std::string outputDataFile);
        virtual void readRuns(std::string inputTreeName, std::string outputDataFile, std::string inputFileName){
            fileName_Data = inputFileName;
            readRuns(inputTreeName, outputDataFile);
            return;
        }
        
    private:
        //Data Members
        bool b1stRun;
        
        bool verbose_IO;            //Output IO Related Statements
        bool verbose_LUT;           //Output Parsed Look-up-table
        bool verbose_PFN;           //Output Parsed File Name
        bool verbose_PkCalc;        //Outputs Peak Integral Iterative Calculations During getPeakIntegral()
        bool verbose_PrintRuns;     //Outputs a Table of all stored run information at the end of analysis
        
        std::string fitOption;
        
        std::string fileName_Data;   //List of Root Files
        std::string fileName_LUT;    //Look Up File
        std::string fileName_ROOT;   //Specific root file found in fileName_Data;
        
        std::string secName_AUTO;
        std::string secName_DET;
        std::string secName_END;
        std::string secName_MAN;
        
        Timing::TimingRunAnalyzer *analyzer;  //Generic Analyzer class set by the user; a default one is provided but the user can make an inherited class whose methods are over written
        
        std::vector<std::string> vecIgnoredParam; //Vector of Parameters to be ignored during getParsedFileName()
        
        enum dataType_t {
            typeBool=0,
            typeFloat,
            typeInt,
            typeString
        };
        
        //This is:
        //  first -> name of physical quantity
        //  second -> vector of identifiers
        //  third -> data type to match too
        //  fourth -> name of detector
        //  fifth -> method index in TRunParameters
        typedef quint<std::string,std::vector<std::string>,dataType_t,std::string,int> LUTType;
        
        //Methods
        
        //Getters - Methods that Get (i.e. Return) Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        float getPeakIntegral(TH1F *hInputHisto, float fInputPeakPos_X);
        
        std::list<std::string> getParsedFileName(std::string inputFileName);
        
        std::vector<LUTType> getLookUpTable(std::string inputROOTFileName, std::string inputLUTFileName, TRunParameters &runLogger);
        
        //Printers - Methods that Print Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        
        //Setters - Methods that Set Something
        //------------------------------------------------------------------------------------------------------------------------------------------
        virtual void setHistogram(std::string inputFileName, TH1F &inputHisto, int chanNum, bool &bExitFlag);
        
        virtual void setMappedParam(std::string &parsedInput, LUTType &lutItem, Timing::TRunParameters &runLogger);
        virtual void setMappedParam(std::list<std::string> &parsedFileNames, std::vector<LUTType> &lookUpTable, Timing::TRunParameters &runLogger);
        
        virtual void setParsedLUTLine(std::string &inputLine, std::vector<std::string> &vec_strLUTIdents, std::string &strTreeName, std::string &strDataType, std::string &strDetName, int &iMthdIdx, bool &bExitFlag);
        
        virtual void setRun(std::string inputROOTFileName, std::string inputLUTFileName, TRunParameters &runLogger);
        
        //Write To File (I/O)
        //------------------------------------------------------------------------------------------------------------------------------------------
        virtual void writeTree(Timing::Run &inputRun, TTree &treeInput);
        
    }; //End Class Definition treeProducerTDC
} //End namespace Timing
#endif /* defined(____treeProducerTDC__) */
