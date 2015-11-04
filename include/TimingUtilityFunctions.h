
#ifndef ____TimingUtility__
#define ____TimingUtility__

//C++ Includes
#include <algorithm>
#include <iostream>
#include <map>
#include <stdio.h>
#include <sstream>
#include <string>
#include <utitlity>
#include <vector>

//My Includes
#include "TimingUtilityOperators.h"

//ROOT Includes

/* 
 * //boolean
 * convert2bool -> converts a string from the set {t,true,1,f,false,0} to boolean (case-insenstive)
 *
 * //File I/O
 * getlineNoSpaces -> gets a line from a file, leading/trailing spaces and tabs are removed
 *
 * //Math
 * deltaMean -> Determines the difference in mean between two datasets
 *
 * //printers
 * printROOTFileStatus -> prints the status bits of a ROOT file
 * printStreamStatus -> prints the status bits of an std::ifstream
 * printStringNotFoundMsg -> prints a message instructing the user a specific pattern requested in file I/O not found; also informs user of which method, and which file the missing pattern is in
 *
 * //string manipulation
 * stofSafe -> converts a string to a float; checks for non-numeric characters; offers user chance to correct;
 * stoiSafe -> converts a string to an int; checks for non-numeric characters; offers user chance to correct
 * getCommaSeparatedList -> returns a std::vector<std::string> of a comma separated list, ignoring whitespaces
 * getParsedLine -> returns a std::pair<std::string,std::string> of a "Field,Value" under the format "Field = 'Value';" note the 4 important characters used for parsing {=,',',;}
 * getString -> converts an input type to a string
 *
 */

namespace Timing {
    //Function Declaration
    //========================================================================================
    //boolean
    bool convert2bool(std::string str, bool &bExitSuccess);
    
    //File I/O
    std::istream & getlineNoSpaces(std::istream & stream, std::string & str);
    
    //Math
    //T -> Type; A -> Allocator
    template<class T>
    struct addVal{
        T val_fixed;
        addition(T fixed) : val_fixed(fixed);
        
        void operator()(T &input) const{
            //input += val_fixed; //internet says "input += fixed"
            input += fixed;
        }
        //usage example:
        //std::for_each(myvec.begin(), myvec.end(), addVal(1.0));
    };

    template<typename T, typename A>
    float deltaMean( std::vector<T,A> const &vec1, std::vector<T,A> const &vec2);
    
    int getMaxForChannelAND(std::map<std::string, int> inputMap)
    int getMinForChannelOR(std::map<std::string, int> inputMap)
    
    //printers
    void printStringNotFoundMsg(std::string strCallingMethod, std::string strPatternNotFound, std::string strLine, std::string strFilename);
    void printROOTFileStatus(TFile *file_ROOT);
    void printStreamStatus(std::ifstream &inputStream);
    
    //string manipulation
    float stofSafe(std::string strInputField, std::string strInputValue);
    int stoiSafe(std::string strInputField, std::string strInputValue);
    std::pair<std::string,std::string> getParsedLine(std::string strInputLine, bool &bExitSuccess);
    template<class TConversion>
    std::string getString(TConversion input);
    std::vector<std::string> getCommaSeparatedList(std::string strInput);
    
    
    //Function Code Begins
    //========================================================================================
    //boolean
    //----------------------------------------------------------------------------------------
    bool convert2bool(std::string str, bool &bExitSuccess){
        //convert to upper case
        std::transform(str.begin(), str.end(), str.begin(), toupper);
        
        //Empty String?
        if ( str.empty() ) {
            bExitSuccess = false;
            return false;
        }
        
        //Input recognized?
        if ( !(str.compare("T")==0 || str.compare("TRUE")==0 || str.compare("1")==0
               || str.compare("F")==0 || str.compare("FALSE")==0 || str.compare("0")==0) ) {
            bExitSuccess = false;
            return false;
        }
        
        bExitSuccess = true;
        
        return (str.compare("T")==0 || str.compare("TRUE")==0 || str.compare("1")==0);
    } //End convert2bool()
    
    //file I/O
    //----------------------------------------------------------------------------------------
    std::istream & getlineNoSpaces(std::istream & stream, std::string & str){
        //get the line
        std::getline(stream, str);
        
        if (str.find("'",0) == std::string::npos ) { //Case: Header
            str.erase(remove(str.begin(),str.end(), ' ' ), str.end() ); //spaces
            str.erase(remove(str.begin(),str.end(), '\t' ), str.end() );//tabs
        } //End Case: Header
        else{ //Case: line input
            int iFirstQuote = str.find("'",0);
            int iLastQuote = str.rfind("'");
            
            //Spaces
            str.erase(remove(str.begin(),str.begin()+iFirstQuote, ' ' ), str.begin()+iFirstQuote ); //Until first single-quote
            str.erase(remove(str.begin()+iLastQuote,str.end(), ' ' ), str.end() ); //Until last single-quote
            
            //tabs
            str.erase(remove(str.begin(),str.begin()+iFirstQuote, '\t' ), str.begin()+iFirstQuote ); //Until first single-quote
            str.erase(remove(str.begin()+iLastQuote,str.end(), '\t' ), str.end() ); //Until last single-quote
        } //End Case: line input
        
        return stream;
    } //End getlineNoSpaces()
    
    //Math
    //----------------------------------------------------------------------------------------
    //T -> Type; A -> Allocator
    //Determines the difference in mean between two datasets
    template<typename T, typename A>
    float deltaMean( std::vector<T,A> const &vec1, std::vector<T,A> const &vec2){
        float fMean1 = (std::accumulate(vec1.begin(), vec1.end(), 0.0) / (float) vec1.size() );
        float fMean2 = (std::accumulate(vec2.begin(), vec2.end(), 0.0) / (float) vec2.size() );
        
        return fMean1 - fMean2;
    } //End deltaMean
    
    //Gets the maximum value for two channels (both channels required to be nonzero)
    int getMaxForChannelAND(std::map<std::string, int> inputMap){
        //Variable Declaration
        int iRetVal;
        
        std::pair<std::string, int> min = *min_element(inputMap.begin(), inputMap.end(), CompareSecond_Min());
        
        //Require All Elements to be nonzero (i.e. have a signal)
        if ( min.second > 0 ) {
            std::pair<std::string, int> max = *max_element(inputMap.begin(), inputMap.end(), CompareSecond_Max() );
            
            //iRetVal = getMaxForChannelAND(inputMap);
            iRetVal = max.second;
        }
        else{
            //One or more channels off
            iRetVal = -1;
        }
        
        return iRetVal;
    } //End getMaxForChannel
    
    //Gets the minimum value for two channels
    int getMinForChannelOR(std::map<std::string, int> inputMap){
        //Variable Declaration
        std::map<std::string, int>::iterator iterMap = inputMap.begin();
        std::map<std::string, int>::iterator iterMapEnd = inputMap.end();
        
        while( iterMap != inputMap.end() ){
            if( 0 == (*iterMap).second){
                //C++11 is magic
                iterMap = inputMap.erase(iterMap);
            }
            else{
                ++iterMap;
            }
        }
        
        if( 0 == inputMap.size() ){
            return -1;
        }
        else{
            std::pair<std::string, int> min = *std::min_element(inputMap.begin(), inputMap.end(), CompareSecond_Min());
            
            return min.second;
        }
    }
    
    //printers
    //----------------------------------------------------------------------------------------
    //Prints the status bits of an input TFile
    void printROOTFileStatus(TFile *file_ROOT){
        std::cout << "Input ROOT File Status:\n";
        std::cout << "\tIsOpen() = " << file_ROOT->IsOpen() << endl;
        std::cout << "\tIsZombie() = " << file_ROOT->IsZombie() << endl;
        
        return;
    } //End printROOTFileStatus
    
    //Prints an error message to the user if a specific pattern (strPatternNotFound) is not found during file I/O
    //Also informs the user which calling method and file the pattern is not found under
    void printStringNotFoundMsg(std::string strCallingMethod, std::string strPatternNotFound, std::string strLine, std::string strFilename){
        cout<<"String '" << strPatternNotFound << "' Not Found in line:\n";
        cout<< strLine <<endl;
        cout<<"Exiting " << strCallingMethod << ", Cross-Check input File:\n";
        cout<<strFilename<<endl;
        
        return;
    } //End printStringNotFoundMsg()
    
    //Prints All Bit Flags for an input ifstream
    void printStreamStatus(std::ifstream &inputStream){
        std::cout << "Input File Stream Bit Status:\n";
        std::cout << " good()=" << inputStream.good() << std::endl;
        std::cout << " eof()=" << inputStream.eof() << std::endl;
        std::cout << " fail()=" << inputStream.fail() << std::endl;
        std::cout << " bad()=" << inputStream.bad() << std::endl;
        
        return;
    } //End treeProducerTDC::printStreamStatus()
    
    //string manipulation
    //----------------------------------------------------------------------------------------
    //Safe conversion to float
    //Scientific notation not supported
    float stofSafe(std::string strInputField, std::string strInputValue){
        bool bInputUnderstood = false;
        bool bManualEntry = false;
        
        float ret_float = -1;
        
        std::string strUserInput;

        if (strInputValue.find_first_not_of( "0123456789." ) == std::string::npos) { //Case: only numeric data
            ret_float =  std::stof(strInput);
        } //End Case: only numeric data
        else{ //Case: non numeric data entered
            std::cout<<"Timing::stofSafe() - Sorry numeric conversion failed\n"
            std::cout<<"\tField = " << strInputField << std::endl;
            std::cout<<"\tValue = " << strInputValue << std::endl;
            std::cout<<"\tNonumeric characters present (sorry scientific notation not supported)\n";
            std::cout<"\tWould you like to enter a value manually [y/N]?\n";
            
            while (!bInputUnderstood) { //Loop requesting manual entry
                std::cin>>strUserInput;
                
                std::transform(strUserInput.begin(), strUserInput.end(), strUserInput.begin(), toupper);
                
                if (0 == strUserInput.compare("Y") || 0 == strUserInput.compare("YES") || 0 == strUserInput.compare("1") ) { //Case: User wants to correct via manual entry
                    bInputUnderstood = true;
                    bManualEntry = true;
                } //End Case: User wants to correct via manual entry
                else if ( 0 == strUserInput.compare("N") || 0 == strUserInput.compare("NO") || 0 == strUserInput.compare("0") ){ //Case: User does not wish for manual entry
                    bInputUnderstood = true;
                    bManualEntry = false;
                } //End Case: User does not wish for manual entry
                else{ //Case: User Input Not Understood
                    std::cout<<"\tSorry your input was not understood\n";
                    std::cout<<"\tPlease answer from the set {y,yes,1,N,NO,0}\n";
                    std::cout<<"\tWould you like to enter a value manually {y,yes,1,N,NO,0}?\n";
                } //End Case: User Input Not Understood
            } //End Loop requesting manual entry
            
            if (bManualEntry) { //Case: Manual Entry
                std::cout<<"\tOkay, for Field = " << strInputField << " Enter a Numeric Value:\n";
                std::cin>>strUserInput;
                
                ret_float = stofSafe(strInputField, strUserInput); //Recursion
            } //End Case: Manual Entry
        } //End Case: non numeric data entered
        
        return ret_float;
    } //End Timing::stofSafe()
    
    int stoiSafe(std::string strInputField, std::string strInputValue){
        bool bInputUnderstood = false;
        bool bManualEntry = false;
        
        int ret_int = -1;
        
        std::string strUserInput;
        
        if (strInputValue.find_first_not_of( "0123456789" ) == std::string::npos) { //Case: only numeric data
            ret_int =  std::stoi(strInput);
        } //End Case: only numeric data
        else{ //Case: non numeric data entered
            std::cout<<"Timing::stofSafe() - Sorry numeric conversion failed\n"
            std::cout<<"\tField = " << strInputField << std::endl;
            std::cout<<"\tValue = " << strInputValue << std::endl;
            std::cout<<"\tNonumeric characters present (or maybe you gave a floating point number instead?)\n";
            std::cout<"\tWould you like to enter a value manually [y/N]?\n";
            
            while (!bInputUnderstood) { //Loop requesting manual entry
                std::cin>>strUserInput;
                
                std::transform(strUserInput.begin(), strUserInput.end(), strUserInput.begin(), toupper);
                
                if (0 == strUserInput.compare("Y") || 0 == strUserInput.compare("YES") || 0 == strUserInput.compare("1") ) { //Case: User wants to correct via manual entry
                    bInputUnderstood = true;
                    bManualEntry = true;
                } //End Case: User wants to correct via manual entry
                else if ( 0 == strUserInput.compare("N") || 0 == strUserInput.compare("NO") || 0 == strUserInput.compare("0") ){ //Case: User does not wish for manual entry
                    bInputUnderstood = true;
                    bManualEntry = false;
                } //End Case: User does not wish for manual entry
                else{ //Case: User Input Not Understood
                    std::cout<<"\tSorry your input was not understood\n";
                    std::cout<<"\tPlease answer from the set {y,yes,1,N,NO,0}\n";
                    std::cout<<"\tWould you like to enter a value manually {y,yes,1,N,NO,0}?\n";
                } //End Case: User Input Not Understood
            } //End Loop requesting manual entry
            
            if (bManualEntry) { //Case: Manual Entry
                std::cout<<"\tOkay, for Field = " << strInputField << " Enter a Numeric Value:\n";
                std::cin>>strUserInput;
                
                ret_int = stoiSafe(strInputField, strUserInput); //Recursion
            } //End Case: Manual Entry
        } //End Case: non numeric data entered
        
        return ret_int;
    } //End Timing::stoiSafe()
    
    std::pair<std::string,std::string> getParsedLine(std::string strInputLine, bool &bExitSuccess){
        //Variable Declaration
        int iPos_Equals = strInputLine.find("=",0);
        int iPos_End    = strInputLine.find(";",0);
        int iPos_Quote1 = strInputLine.find("'",0); //Position of first single-quote
        int iPos_Quote2 = strInputLine.rfind("'");  //Position of last single-quite
        
        std::string strFieldName;
        std::string strFieldValue;
        
        //Check to make sure iPos_Equals found in input string
        if (iPos_Equals == std::string::npos && strInputLine.find("[",0) == std::string::npos ) {
            printStringNotFoundMsg("TimingUtilityFunctions::getParsedLine","=",strInputLine,"See Next Error Msg for Filename" );
            
            //std::cout<<"Character '=' Not Found in line:\n";
            //std::cout<<strInputLine<<std::endl;
            //std::cout<<"Exiting treeProducer::getParsedLine(), Cross-Check Selcetion Setup File\n";
            
            bExitSuccess = false;
            
            return std::make_pair("","");
        }
        
        //Check to make sure iPos_End found in input string
        if (iPos_End == std::string::npos && strInputLine.find("[",0) == std::string::npos) {
            printStringNotFoundMsg("TimingUtilityFunctions::getParsedLine",";",strInputLine,"See Next Error Msg for Filename" );
            
            //std::cout<<"Character ';' Not Found in line:\n";
            //std::cout<<strInputLine<<std::endl;
            //std::cout<<"Exiting treeProducer::getParsedLine(), Cross-Check Selcetion Setup File\n";
            
            bExitSuccess = false;
            
            return std::make_pair("","");
        }
        
        //Check to make sure iPos_Quote1 found in input string
        if (iPos_Quote1 == std::string::npos && strInputLine.find("[",0) == std::string::npos) {
            printStringNotFoundMsg("TimingUtilityFunctions::getParsedLine","'",strInputLine,"See Next Error Msg for Filename" );
            
            //std::cout<<"First Single-Quote (e.g. ' ) Not Found in line:\n";
            //std::cout<<strInputLine<<std::endl;
            //std::cout<<"Exiting treeProducer::getParsedLine(), Cross-Check Selcetion Setup File\n";
            
            bExitSuccess = false;
            
            return std::make_pair("","");
        }
        
        //Check to make sure iPos_Quote2 found in input string
        if (iPos_Quote2 == std::string::npos && strInputLine.find("[",0) == std::string::npos) {
            printStringNotFoundMsg("TimingUtilityFunctions::getParsedLine","'",strInputLine,"See Next Error Msg for Filename" );
            
            //std::cout<<"Last Single-Quote (e.g. ' ) Not Found in line:\n";
            //std::cout<<strInputLine<std::<endl;
            //std::cout<<"Exiting treeProducer::getParsedLine(), Cross-Check Selcetion Setup File\n";
            
            bExitSuccess = false;
            
            return std::make_pair("","");
        }
        
        //Get the Strings
        strFieldName    = strInputLine.substr(0,iPos_Equals);
        strFieldValue   = strInputLine.substr(iPos_Quote1+1, iPos_Quote2 - iPos_Quote1 - 1);
        
        //Set the Exit Flag
        bExitSuccess = true;
        
        //cout<<"getParsedLine() - strFieldName = " << strFieldName << endl;
        //cout<<"getParsedLine() - strFieldValue = " << strFieldValue << endl;
        
        //Return the Pair
        return std::make_pair(strFieldName,strFieldValue);
    } //End getParsedLine()
    
    //template method must be defined in the header file
    template<class TConversion>
    std::string getString(TConversion input){
        std::stringstream sstream;
        sstream<<input;
        return sstream.str();
    } //End getString()
    
    //Returns a vector of strings taken from a comma separated list; ignores whitespaces in between elements
    std::vector<std::string> getCommaSeparatedList(std::string strInput){
        //Variable Declaration
        std::stringstream sstream(strInput);
        
        std::string strEntry;
        
        std::vector<std::string> vec_retStrings;
        
        while (sstream >> strEntry){ //Loop through the string
            vec_retStrings.push_back(strEntry);
            
            if (sstream.peek() == ',' || sstream.peek() == ' ' || sstream.peek() == '\t')
                sstream.ignore();
        } //End loop through the string
        
        return vec_retStrings;
    } //End getCommaSeparatedList()
    
    //Unsorted
    //----------------------------------------------------------------------------------------
    
} //End namespace

#endif
