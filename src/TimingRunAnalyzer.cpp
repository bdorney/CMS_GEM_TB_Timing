//
//  TimingRunAnalyzer.cpp
//  
//
//  Created by Brian L Dorney on 31/10/15.
//
//

#include "TimingRunAnalyzer.h"

//C++ includes
using std::cout;
using std::endl;
using std::map;
using std::pair;
using std::string;
using std::vector;

//My namespaces
using namespace Timing;

//Constructor
TimingRunAnalyzer::TimingRunAnalyzer(){
    bRunSet = false;
    
    bVerboseMode_Analysis   = false;
    bVerboseMode_IO         = false;
    
    strSecBegin_AND = "BEGIN_AND_INFO";
    strSecBegin_OR  = "BEGIN_OR_INFO";
    strSecBegin_DET = "BEGIN_DETECTOR_INFO";
    
    strSecEnd_AND   = "END_AND_INFO";
    strSecEnd_OR    = "END_OR_INFO";
    strSecEnd_DET   = "END_DETECTOR_INFO";
} //End Constructor TimingRunAnalyzer::TimingRunAnalyzer()

//Analyze method, this is the central access point for the analysis
//Inherited class can over ride this method but it MUST exist
//The entire analysis needs to be executed through this method (but not necessarily in this method)
void TimingRunAnalyzer::analyze(){
    
} //End TimingRunAnalyzer::analyze()

//Loads all relevant analysis parameters from a text file
void TimingRunAnalyzer::setAnalysisConfig(string strInputFile){
    //Variable Declaration
    bool bExitSuccess = false;
    //bool bInfoHeaderEnd = false;
    
    pair<string,string> pair_strParam; //<Field, Value>
    
    string strLine = "";
    
    vector<string> vec_strList;
    
    //Open the Data File
    //------------------------------------------------------
    if (bVerboseMode_IO) { //Case: User Requested Verbose Error Messages - I/O
        cout<< "TimingRunAnalyzer::setAnalysisConfig(): trying to open and read: " << strInputFile << endl;
    } //End Case: User Requested Verbose Error Messages - I/O
    
    ifstream fSetup( strInputFile.c_str() );
    
    //Check to See if Data File Opened Successfully
    //------------------------------------------------------
    if (!fSetup.is_open() && bVerboseMode_IO) {
        perror( ("TimingRunAnalyzer::setAnalysisConfig(): error while opening file: " + strInputFile).c_str() );
        printStreamStatus(fSetup);
    }
    
    //Loop Over data Input File
    //------------------------------------------------------
    //Read the file via std::getline().  Obey good coding practice rules:
    //  -first the I/O operation, then error check, then data processing
    //  -failbit and badbit prevent data processing, eofbit does not
    //See: http://gehrcke.de/2011/06/reading-files-in-c-using-ifstream-dealing-correctly-with-badbit-failbit-eofbit-and-perror/
    while ( getlineNoSpaces(fSetup, strLine) ) {
        //Does the user want to comment out this line?
        if ( 0 == strLine.compare(0,1,"#") ) continue;
        
        //Identify Section Headers
        if ( 0 == strLine.compare(strSecBegin_AND) ) { //Case: AND SECTION
            
        } //End Case: AND SECTION
        else if ( 0 == strLine.compare(strSecBegin_DET) ){ //Case: DET SECTION
            
        } //End Case: DET SECTION
        else if ( 0 == strLine.compare(strSecBegin_OR) ){ //Case: OR SECTION
            
        } //End Case: OR SECTION
        
    } //End Loop Over Input File
    
    //Check to see if we had problems while reading the file
    if (fSetup.bad() && bVerboseMode_IO) {
        perror( ("TimingRunAnalyzer::setAnalysisConfig(): error while reading file: " + fileName_Data).c_str() );
        printStreamStatus(dataInput);
    }
    
} //End TimingRunAnalyzer::setAnalysisConfig()


void TimingRunAnalyzer::setFitParamDet(std::string strDet_Name, std::string strFitFormula, std::map<std::string, float> map_Fit_Param){
    
}

void TimingRunAnalyzer::setFitParamDetOR(std::string strFitFormula, std::map<std::string, float> map_Fit_Param){
    
}

void TimingRunAnalyzer::setFitParamDetAND(std::string strFitFormula, std::map<std::string, float> map_Fit_Param){
    
}

void TimingRunAnalyzer::setHistoSetup(std::string strDet_Name, std::ifstream &fStream, HistoSetup& setupHisto){
    //Loop through the section
    while ( getlineNoSpaces(fSetup, strLine) ) {
        bExitSuccess = false;
        
        //Does the user want to comment out this line?
        if ( 0 == strLine.compare(0,1,"#") ) continue;
        
        //Has this section ended?
        if ( line.compare(strSecEnd_AND) == 0 ) break;
        
        //Get Parameter Pairing
        pair_strParam = Timing::getParsedLine(strLine, bExitSuccess);
        
        //transform field name to upper case
        std::transform(pair_strParam.first.begin(),pair_strParam.first.end(),pair_strParam.first.begin(),toupper);
        
        //Parse pair
        if (bExitSuccess) { //Case: Parameter Fetched Successfully
            if ( 0 == pair_strParam.first.compare("HISTO_NAME") ) {
                analysisSetup.setupAND.strHisto_Name = pair_strParam.second;
            }
            else if( 0 == pair_strParam.first.compare("HISTO_NUMBINS") ){
                analysisSetup.setupAND.iHisto_nBins = Timing::stoiSafe(pair_strParam.first, pair_strParam.second);
            }
            else if( 0 == pair_strParam.first.compare("HISTO_BINRANGE") ){
                //Get comma separated list
                vec_strList = Timing::getCommaSeparatedList(pair_strParam.second);
                
                if (vec_strList.size() >= 2) { //Case: at least 2 numbers
                    //Fetch
                    analysisSetup.setupAND.fHisto_xLower = Timing::stofSafe(pair_strParam.first, vec_strList[0]);
                    analysisSetup.setupAND.fHisto_xUpper = Timing::stofSafe(pair_strParam.first, vec_strList[1]);
                    
                    //Reset to ensure they are both correctly lower & upper values
                    analysisSetup.setupAND.fHisto_xLower = std::min(analysisSetup.setupAND.fHisto_xLower, analysisSetup.setupAND.fHisto_xUpper);
                    analysisSetup.setupAND.fHisto_xUpper = std::max(analysisSetup.setupAND.fHisto_xLower, analysisSetup.setupAND.fHisto_xUpper);
                    
                    //Tell the user they entered more than what was expected
                    if (vec_strList.size() > 2) { //Case: 3 or more numbers
                        cout<<"TimingRunAnalyzer::setAnalysisConfig() - Sorry you entered 3 or more numbers for " << pair_strParam.first << endl;
                        cout<<"\tI have only used the first two and ignored the rest:\n";
                        cout<<"\t\t"<<analysisSetup.setupAND.fHisto_xLower<<endl;
                        cout<<"\t\t"<<analysisSetup.setupAND.fHisto_xUpper<<endl;
                    } //End Case: 3 or more numbers
                } //End Case: at least 2 numbers
                else{ //Case: Not enough numbers
                    if (vec_strList.size() == 1) { //Case: only 1 number entered
                        analysisSetup.setupAND.fHisto_xUpper = Timing::stofSafe(pair_strParam.first, vec_strList[0]);
                    } //End Case: only 1 number entered
                    
                    //Reset to ensure they are both correctly lower & upper values
                    analysisSetup.setupAND.fHisto_xLower = std::min(analysisSetup.setupAND.fHisto_xLower, analysisSetup.setupAND.fHisto_xUpper);
                    analysisSetup.setupAND.fHisto_xUpper = std::max(analysisSetup.setupAND.fHisto_xLower, analysisSetup.setupAND.fHisto_xUpper);
                    
                    //Output to User
                    cout<<"TimingRunAnalyzer::setAnalysisConfig() - Sorry I was expecting a comma separated list of 2 numbers for: " << pair_strParam.first << endl;
                    cout<<"\tRight Now I have set:\n"
                    cout<<"\t\tLower Histo Value = "<<analysisSetup.setupAND.fHisto_xLower<<endl;
                    cout<<"\t\tUpper Histo Value"<<analysisSetup.setupAND.fHisto_xUpper<<endl;
                } //End Case: Not enough numbers
            } //End Case: Assign Histo Bin Range
            else if( 0 == pair_strParam.first.compare("HISTO_XTITLE") ){
                analysisSetup.setupAND.strHisto_Title_X = pair_strParam.second;
            }
            else if( 0 == pair_strParam.first.compare("HISTO_YTITLE") ){
                analysisSetup.setupAND.strHisto_Title_Y = pair_strParam.second;
            }
            else if( 0 == pair_strParam.first.compare("FIT_AUTORANGING") ){
                analysisSetup.setupAND.bFit_AutoRanging = Timing::convert2bool(pair_strParam.second, bExitSuccess);
                
                if (!bExitSuccess) {
                    cout<<"TimingRunAnalyzer::setAnalysisConfig() - Attempted to Assign 'Fit_Autoranging' value failed!!!\n";
                    cout<<"\tYour (Field name, Field value) pair = (" << pair_strParam.first << "," << pair_strParam.second << ")\n";
                    cout<<"\tField value should be from the set {t,true,1,f,false,0}\n";
                    cout<<"\tUndefined behavior may occur!!!";
                }
            }
            else if( 0 == pair_strParam.first.compare("FIT_FORMULA") ){
                analysisSetup.setupAND.strFit_Formula = pair_strParam.second
            }
            else if( 0 == pair_strParam.first.compare("FIT_NAME") ){
                analysisSetup.setupAND.strFit_Name = pair_strParam.second
            }
            else if( 0 == pair_strParam.first.compare("FIT_OPTION") ){
                analysisSetup.setupAND.strFit_Option = pair_strParam.second
            }
            else if( 0 == pair_strParam.first.compare("FIT_PARAM_MAP") ){
                analysisSetup.setupAND.vec_strFit_ParamMeaning = getCommaSeparatedList(pair_strParam.second);
            }
            else if( 0 == pair_strParam.first.compare("FIT_PARAM_IGUESS") ){
                analysisSetup.setupAND.vec_strFit_ParamIGuess = getCommaSeparatedList(pair_strParam.second);
            }
            else{ //Case: Parameter not recognized
                
            } //End Case: Parameter not recognized
        } //End Case: Parameter Fetched Successfully
        else{ //Case: Parameter was NOT fetched Successfully
            cout<<"TimingRunAnalyzer::setAnalysisConfig() - Sorry I didn't parse parameter correctly\n";
            cout<<"\tWorking on file: " << strInputFile << endl;
            cout<<"\tCurrent line: " << strLine << endl;
            cout<<"\tPlease cross-check! Skipping!!!\n";
            
            
        } //End Case: Parameter was NOT fetched Successfully
    } //End Loop through Section
}