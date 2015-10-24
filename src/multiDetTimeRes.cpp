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
#include <map>
#include <stdio.h>
#include <sstream>
#include <string>
#include <vector>

//ROOT Includes
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"

#ifdef __MAKECINT__
//#pragma link C++ class vector<int>+;
//#pragma link C++ class vector<float>+;
#pragma link C++ class map<string,int>+;
#endif

//My Includes


/*Usage:
 * 
 * multiDetTimeRes <Input Root File> <Tree Name> {comma separated list of channel numbers}
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
    
    //size_t iLfBrace, iRtBrace, iComma;  //Position of left brace "{", right brace "}", and comma ","
    
    std::map<std::string,int> map_fTDCData;
    
    std::string strName_InputRootFile;
    std::string strName_tree;           //TTree found in strName_InputRootFile
    std::string strCh, strChannels;
    //std::string strTDCChanHisto = "TDC_Ch";
    
    //std::vector<int> vec_fTDCData;
    
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
        std::cout<<"\t./multiDetTimeRes <Input Root File> <Tree Name> <comma separated list of channel numbers; no spaces> <verbose mode true/false>\n";
        std::cout<<"\tExample:\n";
        std::cout<<"\t\t./multiDetTimeRes myRootFile.root myTreeName 5,6,3 false\n;";
        std::cout<<"\tExiting\n";
        
        return 1;
    } //End Case: Usage
    else if (vec_strInputArgs.size() == 2 && vec_strInputArgs[1].compare("-h") == 0) { //Case: Help Menu
        std::cout<<"multiDetTimeRes v"<<fVersion<<endl;
        std::cout<<"Author: Brian L. Dorney\n";
        std::cout<<"Offline Analysis of multi detector time response:\n";
        std::cout<<"\t./multiDetTimeRes <Input Root File> <Tree Name> <comma separated list of channel numbers> <verbose mode true/false>\n";
        std::cout<<"\tExample:\n";
        std::cout<<"\t\t./multiDetTimeRes myRootFile.root myTreeName 5,6,3 false\n";
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
            //iLfBrace    = strChannels.find("{");
            //iRtBrace    = strChannels.find("}");
            
            //vec_strTDCChan.push_back(strChannels.substr(iLfBrace+1,iRtBrace-iLfBrace) );
            vec_strTDCChan.push_back(strChannels);
        } //End Case: Single Channel
        else{
            //std::stringstream strStream(strChannels);
            std::stringstream strStream;
            
            strStream<<strChannels;
            
            while ( strStream.good() ) {
                getline( strStream, strCh, ',');
                
                if (strCh.length() > 0 ) vec_strTDCChan.push_back( strCh );
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
    
    //Open input root file
    //------------------------------------------------------
    TFile *file_ROOT = new TFile(strName_InputRootFile.c_str(), "READ", "", 1);
    
    //Check to see if data file opened successfully, if so load the tree
    //------------------------------------------------------
    if ( !file_ROOT->IsOpen() || file_ROOT->IsZombie() ) {
        perror( ("main() - error while opening file: " + strName_InputRootFile ).c_str() );
        std::cout << "Input ROOT File Status:\n";
        std::cout << "\tIsOpen() = " << file_ROOT->IsOpen() << endl;
        std::cout << "\tIsZombie() = " << file_ROOT->IsZombie() << endl;
        std::cout << "Exiting!!!\n";
        
        return -4;
    }
    
    TTree *tree_Input = (TTree*) file_ROOT->Get( strName_tree.c_str() );
    
    if ( nullptr == tree_Input ) {
        std::cout<<"main() - error while fetching: " << strName_tree.c_str() << endl;
        std::cout<<"\tTree Returns nullptr; tree_Input = " << tree_Input << endl;
        std::cout<<"Exiting!!!\n";
        
        return -4;
    }
    
    //Get Requested Channels
    //------------------------------------------------------
    if (bVerboseMode) {
        cout<<"main() - Analyzing Channels:\n";
        
        for (int i=0; i < vec_strTDCChan.size(); ++i) {
            cout<<"Ch \t" << vec_strTDCChan[i] << endl;
        }
    }
    
    cout<<"Evt\t"<<endl;
    for(int i=0; i < vec_strTDCChan.size(); ++i){
        cout<<"Ch"<<vec_strTDCChan[i]<<"\t";
        
        //This is a hack...?
        //vec_fTDCData.push_back(-1.);
        map_fTDCData[vec_strTDCChan[i]];
        
        //tree_Input->SetBranchAddress( ("TDC_Ch" + vec_strTDCChan[i]).c_str(), &(map_fTDCData[vec_strTDCChan[i]]) );
    } //End Loop Over vec_strTDCChan
    cout<<endl;

    /*for(int i=0; i < vec_fTDCData.size(); ++i){
	tree_Input->SetBranchAddress( ("TDC_Ch" + vec_strTDCChan[i]).c_str(), &vec_fTDCData[i] );
    }*/

    for(int i=0; i < vec_strTDCChan.size(); ++i){
	tree_Input->SetBranchAddress( ("TDC_Ch" + vec_strTDCChan[i]).c_str(), &(map_fTDCData[vec_strTDCChan[i]]) );
    }    

    int dummy;
    cout<<"Awaiting input\n";
    std::cin>>dummy;

    //Loop Over Input Data
    //------------------------------------------------------
    for (int i=0; i < tree_Input->GetEntries(); ++i) {
        tree_Input->GetEntry(i);
        
        cout<<i<<"\t";
        
        //for (int j=0; j<vec_fTDCData.size(); ++j) {
          //  cout<<vec_fTDCData[j]<<"\t";
        //}
        
        for (auto chIter = map_fTDCData.begin(); chIter != map_fTDCData.end(); ++chIter) {
            cout<<(*chIter).second<<"\t";
        }
        
        cout<<endl;
    } //End Loop Over tree_Input
    
    return 0;
} //End main()
