//
//  produceTDCTTree.C
//  
//
//  Created by Brian L Dorney on 29/01/15.
//
//

//C++ Includes
#include <algorithm>
#include <fstream>
#include <iostream>
//#include <ifstream>
#include <mcheck.h> //for mcheck_check_all()
//#include <ofstream>
#include <stdio.h>
#include <string>
#include <utility>
#include <vector>
//ROOT Includes

//My Includes
#include "treeProducerTDC.h"
#include "TimingUtilityFunctions.h"
#include "TimingRunAnalyzer.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::string;

using namespace Timing;

struct ProdInfo{
    bool bVerboseMode_IO;
    bool bVerboseMode_PFN;
    bool bVerboseMode_LUT;
    bool bVerboseMode_PrintRuns;
    
    std::string strFitOption;
    
    std::string strPFP_RawData;     //Phyiscal FilePath of raw data files
    std::string strPFN_RunList;     //Physical FileName of run list file
    std::string strPFN_RunList_PFP; //Run list file with PFP applied
    
    std::string strOutput_ROOTFile; //Name of output ROOT file
    std::string strOutput_Tree;     //Name of output ROOT::TTree
    
    std::vector<std::string> vec_strIgnoredIdent;
};

//Gets Production Info from input config file
void getProdInfo(ifstream &file_Input, ProdInfo &pInfo, bool bVerboseMode){
    //Variable Declaration
    bool bInfoHeaderEnd = false;

    std::pair<string,string> pair_Param;

    std::string strLine = "";

    //Loop Through Data File
    //Check for faults immediately afterward
    //------------------------------------------------------
    while ( getlineNoSpaces(file_Input, strLine) /* && !bInfoHeaderEnd */) { //Loop Over Input File
        //Skip Commented Lines
        if (strLine.compare(0,1,"#") == 0) continue;
        
        //Check for Start of Info Header
        if (strLine.compare("[BEGIN_PROD_INFO]") == 0 ) { //Case: Info Header Found!
            cout<<"getProInfo(): Info header found!\n";
            
            while (getlineNoSpaces(file_Input, strLine) ) { //Loop Through Info Header
                bool bExitSuccess;
                
                //Skip Commented Lines
                if (strLine.compare(0,1,"#") == 0) continue;
                
                //Has the Header Ended?
                if (strLine.compare("[END_PROD_INFO]") == 0) { //Case: End of Info Header
                    if (bVerboseMode) { //Case: User Requested Verbose Input/Output
                        cout<<"getProInfo(): End of info header reached!\n";
                        cout<<"getProInfo(): The following information has been logged:\n";
                        //cout<<  "\tDetector = " << pInfo.strName << endl;
                        //cout<<  "\tGap Config = " << pInfo.strGapConfig << endl;
                        //cout<<  "\tSource = " << pInfo.strSource << endl;
                        //cout<<  "\tGas = " << pInfo.strGas << endl;
                        //cout<<  "\t Ambient P = " << pInfo.strPressure << endl;
                        //cout<<  "\t Ambient T = " << pInfo.strTemp << endl;
                    } //End Case: User Requested Verbose Input/Output
                    
                    bInfoHeaderEnd = true;
                    break;
                } //End Case: End of Info Header
                
                //Get Parameter Pairing
                pair_Param = getParsedLine(strLine, bExitSuccess);
                
                if (bExitSuccess) { //Case: Parameter Fetched Successfully
                    if( pair_Param.first.compare("FitOption") == 0 ){            pInfo.strFitOption   = pair_Param.second;}
                    else if( pair_Param.first.compare("PFP_RawData") == 0 ){            pInfo.strPFP_RawData   = pair_Param.second;}
                    else if( pair_Param.first.compare("PFN_RunList") == 0 ){        pInfo.strPFN_RunList         = pair_Param.second;}
                    else if( pair_Param.first.compare("OutputRootFileName") == 0 ) {  pInfo.strOutput_ROOTFile        = pair_Param.second;}
                    else if( pair_Param.first.compare("OutputTreeName") == 0 ){   pInfo.strOutput_Tree    = pair_Param.second;}
                    else if( pair_Param.first.compare("VerboseMode_IO") == 0 ){     pInfo.bVerboseMode_IO      = convert2bool(pair_Param.second, bExitSuccess);}
                    else if( pair_Param.first.compare("VerboseMode_LUT") == 0 ){ pInfo.bVerboseMode_LUT      = convert2bool(pair_Param.second, bExitSuccess);}
                    else if( pair_Param.first.compare("VerboseMode_Parsing") == 0 ){pInfo.bVerboseMode_PFN        = convert2bool(pair_Param.second, bExitSuccess);}
                    else if( pair_Param.first.compare("VerboseMode_PrintRuns") == 0 ){pInfo.bVerboseMode_PrintRuns        = convert2bool(pair_Param.second, bExitSuccess);}
                    else{ //Case: Input Not Recognized
                        cout<<"getProInfo() - Unrecognized field!!!\n";
                        cout<<("getProInfo() - " + pair_Param.first + " = " + pair_Param.second ).c_str() << endl;
                        //cout<<"getProInfo() - Please cross-check input file:" << strInputFileName << endl;
                        cout<<"getProInfo() - Please cross-check config file\n";
                    } //End Case: Input Not Recognized
                } //End Case: Parameter Fetched Successfully
            } //End Loop Through Info Header
        } //End Case: Info Header Found!

        if (bInfoHeaderEnd) break;
    } //End Loop Over Input File
    if ( file_Input.bad() && bVerboseMode) {
        //perror( ("getProInfo(): error while reading file: " + strInputFileName).c_str() );
        perror( "getProInfo(): error while reading config file" );
        Timing::printStreamStatus(file_Input);
    }
    
    //DO NOT CLOSE WILL BE USED ELSEWHERE
    //file_Input.close();

    return;
} //End getProdInfo()

//Main of Program
int main( int argc_, char * argv_[]){
    //Variable Declaration
    bool bVerboseMode = false;
    
    float fVersion = 2.0;
    
    ifstream file_Config;
    
    ProdInfo pInfo;
    
    std::string strInputConfigFile; //Input Config File
    std::string strLine; //Line from a file
    
    std::vector<std::string> vec_strInputArgs;
    
    treeProducerTDC myProducer;
    
    //Transfer Input Arguments into vec_strInputArgs
    //------------------------------------------------------
    vec_strInputArgs.resize(argc_);
    std::copy(argv_, argv_ + argc_, vec_strInputArgs.begin() );

    //Check input Arguments
    //------------------------------------------------------
    if(vec_strInputArgs.size() == 1 ){ //Case: Usage
        std::cout<<"produceTree v"<<fVersion<<endl;
        std::cout<<"Author: Brian L. Dorney\n";
        std::cout<<"For help menu:\n";
        std::cout<<"\t./produceTree -h\n";
        std::cout<<"For CMS GEM test beam production:\n";
        std::cout<<"\t./produceTree <Prod_Config_File> <Analysis_Config_File> <Verbose Mode true/false>\n";

        return 1;
    } //End Case: Usage
    else if (vec_strInputArgs.size() == 2 && vec_strInputArgs[1].compare("-h") == 0) { //Case: Help Menu
        std::cout<<"produceTree v"<<fVersion<<endl;
        std::cout<<"Author: Brian L. Dorney\n";
        std::cout<<"Usage: ./produceTree <Input_Config_File> <Verbose Mode true/false>\n";
        cout<<"Format of expected input file (omitting tabs and angle brackets, i.e. <, and >):\n";
        cout<<"#-----Start of File-----\n";
        cout<<"[BEGIN_PROD_INFO]\n";
        cout<<"\tFitOption='<FIT OPTION>';\n";
        cout<<"\tPFP_RawData='<PHYSICAL FILEPATH OF RAW DATA>';\n";
        cout<<"\tPFN_RunList='<PHYSICAL FILENAME OF RUNE LIST>';\n";
        cout<<"\tOutputRootFileName='<OUTPUT ROOT FILE NAME>';\n";
        cout<<"\tOutputTreeName='<OUTPUT TREE NAME>';\n";
        cout<<"\tVerboseMode_IO='<VERBOSE INPUT/OUTPUT true/false>';\n";
        cout<<"\tVerboseMode_LUT='<VERBOSE LOOK UP TABLE CREATION true/false>';\n";
        cout<<"\tVerboseMode_Parsing='<VERBOSE PARSING FILE NAMES true/false>';\n";
        cout<<"\tVerboseMode_PrintRuns='<VERBOSE RUN PRINTING true/false>';\n";
        cout<<"[END_PROD_INFO]\n";
        cout<<"[BEGIN_IGNORED_IDENTIFIERS]\n";
        cout<<"#List of identifiers in run file names to be ignroed; one parameter per line\n";
        cout<<"\t<IGNORED IDENTIFIER>\n";
        cout<<"\t<IGNORED IDENTIFIER>\n";
        cout<<"\t...\n";
        cout<<"\t...\n";
        cout<<"\t...\n";
        cout<<"\t<IGNORED IDENTIFIER>\n";
        cout<<"\t<IGNORED IDENTIFIER>\n";
        cout<<"[END_IGNORED_IDENTIFIERS]\n";
        cout<<"#-----End of File-----\n";
        cout<<"The '#' symbol is recognized as a comment indication\n";
        
        return 1;
    } //End Case: Help Menu
    else if(vec_strInputArgs.size() == 4){ //Case: Production Mode!
        bool bExitSuccess = false;
        
        //Set input config file
        //The below is never actually running...
        if (bVerboseMode) { //Case: User Requested Verbose Error Messages - I/O
            cout<< "main(): trying to open and read: " << vec_strInputArgs[1] << endl;
        } //End Case: User Requested Verbose Error Messages - I/O
        
        file_Config.open(vec_strInputArgs[1].c_str() );
        
        //Check to see if run list file opened successfully
        if (!file_Config.is_open()) {
            perror( ("main(): error while opening file: " + vec_strInputArgs[1]).c_str() );
            Timing::printStreamStatus(file_Config);
            
            cout<<"Exitting!!!\n";
            
            return -1;
        } //End Case: Input Not Understood
        
        //Set the verbose mode
        bVerboseMode = convert2bool(vec_strInputArgs[3], bExitSuccess);
        if (!bExitSuccess) { //Case: Input Not Understood
            cout<<"main() - vec_strInputArgs[3] expected to be boolean!!!\n";
            cout<<"main() - Parameter given:\n";
            cout<<"\t"<<vec_strInputArgs[3]<<endl;
            cout<<"Exitting!!!\n";
            
            return -2;
        } //End Case: Input Not Understood
    } //End Case: Production Mode!
    else{ //Case: Input Not Understood
        cout<<"main() - Input parameters not understood!!!\n";
        cout<<"main() - Parameters given:\n";
        for (int i=0; i < vec_strInputArgs.size(); ++i) {
            cout<<"\t" << vec_strInputArgs[i] << endl;
        }
        cout<<"main() - Try calling ./produceTree -h for help!!!\n";
        
        return -3;
    } //End Case: Input Not Understood
    
    //Get the Production Info
    //------------------------------------------------------
    getProdInfo(file_Config, pInfo, bVerboseMode);

    //Create new run list file with filepath of files inserted
    //------------------------------------------------------
    //Open the run list file
    if (bVerboseMode) { //Case: User Requested Verbose Error Messages - I/O
        cout<< "main(): trying to open and read: " << pInfo.strPFN_RunList << endl;
    } //End Case: User Requested Verbose Error Messages - I/O
    
    ifstream file_RunList(pInfo.strPFN_RunList.c_str() );
    
    //Check to see if run list file opened successfully
    if (!file_RunList.is_open() && bVerboseMode) {
        perror( ("main(): error while opening file: " + pInfo.strPFN_RunList).c_str() );
        Timing::printStreamStatus(file_RunList);
    }
    
    //Create a new run list file for which we will insert the PFP into
    if (pInfo.strPFN_RunList.find(".txt") != string::npos ) {
        pInfo.strPFN_RunList_PFP = pInfo.strPFN_RunList;
        pInfo.strPFN_RunList_PFP.erase(pInfo.strPFN_RunList_PFP.find(".txt"), pInfo.strPFN_RunList_PFP.length() - pInfo.strPFN_RunList_PFP.find(".txt") );
        pInfo.strPFN_RunList_PFP = pInfo.strPFN_RunList_PFP + "_PFP.txt";
    }
    else{
        pInfo.strPFN_RunList_PFP = "RunList_PFP.txt";
    }
    
    //std::ofstream file_RunList_PFP(pInfo.strPFN_RunList_PFP.c_str(), std::ios_base::app);
    std::ofstream file_RunList_PFP( pInfo.strPFN_RunList_PFP.c_str() );
    
    //Loop through Run List file, check for faults immediately afterward
    while ( getline(file_RunList, strLine ) ) { //Loop through file_RunList
	//Skip Commented Lines
        if (strLine.compare(0,1,"#") == 0) continue;
        
        file_RunList_PFP<<(pInfo.strPFP_RawData + "/" + strLine ).c_str()<<endl;
    } //End Loop through file_RunList
    if ( file_RunList.bad() && bVerboseMode) {
        perror( ("main(): error while reading file: " + pInfo.strPFN_RunList).c_str() );
        Timing::printStreamStatus(file_RunList);
    }
    
    file_RunList.close();
    
    //Create the Ignored Identifier List
    //------------------------------------------------------
    //Loop through config file, check for faults immediately afterward
    cout<<"Ignored parameter stream status"<<endl;
    Timing::printStreamStatus(file_Config);
    while (getlineNoSpaces(file_Config, strLine) ) { //Loop Over file_Config
        //Skip Commented Lines
        if (strLine.compare(0,1,"#") == 0) continue;
        
        cout<<"strLine=" << strLine << endl;

        //Check for start of Data header
        if (strLine.compare("[BEGIN_IGNORED_IDENTIFIERS]") == 0) { //Case: Ignored Identifiers Header Started
            cout<<"main(): ignored identifiers header found!\n";
            
            while (getlineNoSpaces(file_Config, strLine) ) { //Loop through ignored identifiers
                //Skip Commented Lines
                if (strLine.compare(0,1,"#") == 0) continue;
                
                //Has this section ended?
                if (strLine.compare("[END_IGNORED_IDENTIFIERS]") == 0){ //Case: Section Ended
                    if (bVerboseMode) { //Case: Verbose Mode
                        cout<<"main() - End of Ignored Identifiers reached!\n";
                        cout<<"main() - The following information has been logged:\n";
                        for(int i=0; i<pInfo.vec_strIgnoredIdent.size(); ++i){
                            cout<<"\t"<<pInfo.vec_strIgnoredIdent[i]<<endl;
                        } //End Loop Over pInfo.vec_strIgnoredIdent
                    } //End Case: Verbose Mode
                    
                    break;
                } //End Case: Section Ended
                
                //Store ignored parameter
                cout<<"ignoring " << strLine << endl;
                myProducer.setIgnoredParameter(strLine);    pInfo.vec_strIgnoredIdent.push_back(strLine);
            } //End Loop through ignored identifiers
        } //End Case: Ignored Identifiers Header Started
    } //End Loop Over file_Config
    if ( file_Config.bad() && bVerboseMode) {
        perror( ("main(): error while reading file: " + vec_strInputArgs[1]).c_str() );
        Timing::printStreamStatus(file_Config);
    }
    
    file_Config.close();
    
    //Ignored Parameter List (All)
    /*myProducer.setIgnoredParameter("TDC.ROOT");
    myProducer.setIgnoredParameter("Trial01");
    myProducer.setIgnoredParameter("Lat24");
    myProducer.setIgnoredParameter("Lat26");
    myProducer.setIgnoredParameter("Lat27");
    myProducer.setIgnoredParameter("Lat30");
    myProducer.setIgnoredParameter("Lat31");
    myProducer.setIgnoredParameter("Ch10");
    myProducer.setIgnoredParameter("Ch10GE2.root");
    myProducer.setIgnoredParameter("VFAT0.root");
    myProducer.setIgnoredParameter("VFAT2.root");
    myProducer.setIgnoredParameter("VFAT3.root");
    myProducer.setIgnoredParameter("WithFuncGen");
    myProducer.setIgnoredParameter("Optimal");
    myProducer.setIgnoredParameter("values");*/

    //Memory check?
    mcheck_check_all();
    
    //Setup the Analyzer
    TimingRunAnalyzer *myAnalyzer = new TimingRunAnalyzer();
    myAnalyzer->setAnalysisConfig(vec_strInputArgs[2]);
    myAnalyzer->setTDCResolution(0.3);  //300 picoseconds for 1200ns acquisition window
    
    //Setup the producer
    //------------------------------------------------------
    myProducer.setFitOption(pInfo.strFitOption);
    //myProducer.setHistoRebinFactor(2);
    myProducer.setVerboseModeIO(pInfo.bVerboseMode_IO);
    myProducer.setVerboseModePFN(pInfo.bVerboseMode_PFN);
    myProducer.setVerboseModeLUT(pInfo.bVerboseMode_LUT);
    myProducer.setVerboseModePrintRuns(pInfo.bVerboseMode_PrintRuns);
    myProducer.doNotConvertToUpper("fHV_Det2_DividerCurrent");
    myProducer.doNotConvertToUpper("fHV_Det1_Drift");
    myProducer.doNotConvertToUpper("fHV_Det1_G1Top");
    myProducer.doNotConvertToUpper("fHV_Det1_G1Bot");
    myProducer.doNotConvertToUpper("fHV_Det1_G2Top");
    myProducer.doNotConvertToUpper("fHV_Det1_G2Bot");
    myProducer.doNotConvertToUpper("fHV_Det1_G3Top");
    myProducer.doNotConvertToUpper("fHV_Det1_G3Bot");
    myProducer.setFilesData(pInfo.strPFN_RunList_PFP);
    myProducer.setAnalyzer(myAnalyzer);
    //myProducer.writeTree(pInfo.strOutput_Tree, pInfo.strOutput_ROOTFile);
    myProducer.readRuns(pInfo.strOutput_Tree, pInfo.strOutput_ROOTFile);
    
    return 0;
} //End main()
