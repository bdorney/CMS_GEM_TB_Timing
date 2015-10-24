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
#include <stdio.h>
#include <sstream>
#include <string>
#include <vector>

//ROOT Includes
#include "TFile.h"
#include "TROOT.h"

//My Includes

/*Usage:
 * 
 * multiDetTimeRes <Input Root File> <Tree Name> {comma separated list of channel numbers}
 *
 */

using std::cout;
using std::endl;
using std::string;

bool convert2bool(string str, bool &bExitSuccess){
    //convert to upper case
    transform(str.begin(), str.end(), str.begin(), toupper);
    
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

int main( int argc_, char * argv_[]){
    //Variable Declaration
    bool bVerboseMode;
    
    float fVersion = 1.0;
    
    size_t iLfBrace, iRtBrace, iComma;  //Position of left brace "{", right brace "}", and comma ","
    
    std::string strName_InputRootFile;
    std::string strName_tree;           //TTree found in strName_InputRootFile
    std::string strCh, strChannels;
    std::string strTDCChanHisto = "TDC_Ch";
    
    std::vector<std::string> vec_strInputArgs;
    std::vector<std::string> vec_strTDCChan;
    
    //Transfer Input Arguments into vec_strInputArgs
    //------------------------------------------------------
    vec_strInputArgs.resize(argc_);
    std::copy(argv_, argv_ + argc_, vec_strInputArgs.begin() );

    //Check input arguments
    if(vec_strInputArgs.size() == 1 ){ //Case: Usage
        std::cout<<"multiDetTimeRes v"<<fVersion<<endl;
        std::cout<<"Author: Brian L. Dorney\n";
        std::cout<<"For help menu:\n";
        std::cout<<"\t./multiDetTimeRes -h\n";
        std::cout<<"Offline Analysis of multi detector time response:\n";
        std::cout<<"\t./multiDetTimeRes <Input Root File> <Tree Name> {comma separated list of channel numbers; no spaces} <verbose mode true/false>\n";
        std::cout<<"\tExample:\n";
        std::cout<<"\t\t./mulitDetTimeResp myRootFile.root myTreeName {5,6,3} false\n;";
        std::cout<<"\tExiting\n";
        
        return 1;
    } //End Case: Usage
    else if (vec_strInputArgs.size() == 2 && vec_strInputArgs[1].compare("-h") == 0) { //Case: Help Menu
        std::cout<<"multiDetTimeRes v"<<fVersion<<endl;
        std::cout<<"Author: Brian L. Dorney\n";
        std::cout<<"Offline Analysis of multi detector time response:\n";
        std::cout<<"\t./multiDetTimeRes <Input Root File> <Tree Name> {comma separated list of channel numbers} <verbose mode true/false>\n";
        std::cout<<"\tExample:\n";
        std::cout<<"\t\t./mulitDetTimeResp myRootFile.root myTreeName {5,6,3} false\n;";
        std::cout<<"\tExiting\n";
        
        //Right now this is a duplicate omultiDetTimeRespf above, maybe I add additional functionality later
        
        return 1;
    } //End Case: Help Menu
    else if(vec_strInputArgs.size() == 5){ //Case: Analysis Mode
        bool bExitSuccess = false;
        
        strName_InputRootFile   = vec_strInputArgs[1];
        strName_tree            = vec_strInputArgs[2];
        strChannels             = vec_strInputArgs[3];
        
        bVerboseMode            = convert2bool(vec_strInputArgs[4], bExitSuccess);
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
        
        if ( std::string::npos == strChannels.find_first_of(",") ) { //Case: Single Channel
            iLfBrace    = strChannels.find("{");
            iRtBrace    = strChannels.find("}");
            
            vec_strTDCChan.push_back(strChannels.substr(iLfBrace+1,iRtBrace-iLfBrace) );
        } //End Case: Single Channel
        else{
            //std::stringstream strStream(strChannels);
            std::stringstream strStream;
            
            strStream<<strChannels;
            
            while ( strStream.good() ) {
                getline( strStream, strCh, ',');
                vec_strTDCChan.push_back( strCh );
            }
        } //End Case: Multi Channels
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
    
    //Analyze Requested Channels
    //------------------------------------------------------
    if (bVerboseMode) {
        cout<<"main() - Analyzing Channels:\n";
        
        for (int i=0; i < vec_strTDCChan.size(); ++i) {
            cout<<"Ch \t" << vec_strTDCChan[i] << endl;
        }
    }
    
    return 0;
} //End main()