//
//  timeResponse_OR_AND.cpp
//
//
//  Created by Brian L Dorney on 24/10/15.
//
//

//C++ Includes
#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <stdio.h>
#include <sstream>
#include <string>
//#include <vector>

//ROOT Includes
#include "TFile.h"
//#include "TH1F.h"
//#include "TH2F.h"
#include "TROOT.h"
#include "TTree.h"

//#ifdef __MAKECINT__
//#pragma link C++ class vector<int>+;
//#pragma link C++ class vector<float>+;
//#pragma link C++ class map<string,int>+;
//#endif

//My Includes
#include "TimingRunAnalyzer.h"
#include "TimingUtilityFunctions.h"
#include "TimingUtilityOperators.h"
#include "TimingUtilityTypes.h"

/*Usage:
 *
 * singleRunAnalyzer <Input Root File> <Tree Name> {comma separated list of channel numbers}
 *
 * Return Codes:
 *  1 -> Help menu called
 *  0 -> Completed Successfully
 * -1 ->
 * -2 -> Correct number of parameters; but argument not understood (boolean variable)
 * -3 -> Incorrect number of parameters
 * -4 -> Input ROOT file does not open successfully
 */

using std::cout;
using std::endl;
using std::map;
using std::string;

using namespace Timing;

int main( int argc_, char * argv_[]){
    //Variable Declaration
    bool bVerboseMode;
    
    float fVersion = 1.0;
    
    Run run;
    
    std::string strName_inputRootFile;
    std::string strName_tree;           //TTree found in strName_inputRootFile
    std::string strName_analysisFile;
    
    std::string strCh, strChannels;
    
    TimingRunAnalyzer timingAnalyzer;
    
    std::vector<std::string> vec_strInputArgs;
    std::vector<std::string> vec_strTDCChan;
    
    //Transfer Input Arguments into vec_strInputArgs
    //------------------------------------------------------
    vec_strInputArgs.resize(argc_);
    std::copy(argv_, argv_ + argc_, vec_strInputArgs.begin() );
    
    //Check input arguments
    if(vec_strInputArgs.size() == 1 ){ //Case: Usage
        std::cout<<"singleRunAnalyzer v"<<fVersion<<endl;
        std::cout<<"Author: Brian L. Dorney\n";
        std::cout<<"For help menu:\n";
        std::cout<<"\t./singleRunAnalyzer -h\n";
        std::cout<<"Offline Analysis of multi detector time response:\n";
        std::cout<<"\t./singleRunAnalyzer <Input Root File> <Tree Name> <comma separated list of channel numbers; no spaces> <Analysis Config File> <verbose mode true/false>\n";
        std::cout<<"\tExample:\n";
        std::cout<<"\t\t./singleRunAnalyzer myRootFile.root myTreeName 5,6,3 myAnalysis.txt false\n;";
        std::cout<<"\tExiting\n";
        
        return 1;
    } //End Case: Usage
    else if (vec_strInputArgs.size() == 2 && vec_strInputArgs[1].compare("-h") == 0) { //Case: Help Menu
        std::cout<<"singleRunAnalyzer v"<<fVersion<<endl;
        std::cout<<"Author: Brian L. Dorney\n";
        std::cout<<"Offline Analysis of multi detector time response:\n";
        std::cout<<"\t./singleRunAnalyzer <Input Root File> <Tree Name> <comma separated list of channel numbers> <Analysis Config File> <verbose mode true/false>\n";
        std::cout<<"\tExample:\n";
        std::cout<<"\t\t./singleRunAnalyzer myRootFile.root myTreeName myAnalysis.txt 5,6,3 false\n";
        std::cout<<"\tExiting\n";
        
        //Right now this is a duplicate of the above, maybe I add additional functionality later
        
        return 1;
    } //End Case: Help Menu
    else if(vec_strInputArgs.size() == 6){ //Case: Analysis Mode
        bool bExitSuccess = false;
        
        strName_inputRootFile   = vec_strInputArgs[1];
        strName_tree            = vec_strInputArgs[2];
        strChannels             = vec_strInputArgs[3];
        strName_analysisFile    = vec_strInputArgs[4];
        
        bVerboseMode            = convert2bool(vec_strInputArgs[5], bExitSuccess);
        if (!bExitSuccess) { //Case: Input Not Understood
            cout<<"main() - vec_strInputArgs[2] expected to be boolean!!!\n";
            cout<<"main() - Parameter given:\n";
            cout<<"\t"<<vec_strInputArgs[2]<<endl;
            cout<<"Exitting!!!\n";
            
            return -2;
        } //End Case: Input Not Understood
        
        //Parse Requested Channels
        //------------------------------------------------------
        
        //Debugging
        if (bVerboseMode) {
            cout<<"main() - Channel list given:\n";
            cout<<"\t"<<strChannels<<endl;
        }
        
        vec_strTDCChan = getCommaSeparatedList(strChannels);
    } //End Case: Analysis Mode
    else{ //Case: Input Not Understood
        cout<<"main() - Input parameters not understood!!!\n";
        cout<<"main() - Parameters given:\n";
        for (int i=0; i < vec_strInputArgs.size(); ++i) {
            cout<<"\t" << vec_strInputArgs[i] << endl;
        }
        cout<<"main() - Try calling ./produceTree -h for help!!!\n";
        
        return -3;
    } //End Case: Input Not Understood
    

    //Have the TimingRunAnalyzer load the analysis config file
    //------------------------------------------------------
    timingAnalyzer.setVerboseModeAnalysis(true);
    timingAnalyzer.setVerboseModeIO(true);
    timingAnalyzer.setAnalysisConfig( strName_analysisFile );
    timingAnalyzer.setTDCResolution(0.300);
    
    //Setup the Run
    //------------------------------------------------------
    run.iRun = 0;       //Filler for Now
    run.iTrig_Mode = 0; //Asynchronous
    
    run.strRunName      = strName_inputRootFile;
    run.strTreeName_Run = strName_tree;
    
    //Setup the detector map
    if ( vec_strTDCChan.size() == timingAnalyzer.analysisSetup.map_DetSetup.size() ) {
        //map<string, HistoSetup, map_cmp_str>::iterator iterAnaSetup = timingAnalyzer.analysisSetup.map_DetSetup.begin();
        
        for (auto iterAnaSetup = timingAnalyzer.analysisSetup.map_DetSetup.begin(); iterAnaSetup != timingAnalyzer.analysisSetup.map_DetSetup.end(); ++iterAnaSetup) {
            cout<<"Detector  = " << (*iterAnaSetup).first << ";\t" << "TDC Channel = " << vec_strTDCChan[std::distance( )] << endl;
        }
    } //End Case configuration file & user input match

    return 0;
} //End main()
