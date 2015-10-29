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
//#include <vector>

//ROOT Includes
#include "TFile.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TTree.h"

#ifdef __MAKECINT__
//#pragma link C++ class vector<int>+;
//#pragma link C++ class vector<float>+;
#pragma link C++ class map<string,int>+;
#endif

//My Includes
#include "multiDetTimeResp.h"

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

/*namespace{

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

struct cmp_chan{
    bool operator()(const string &str1, const string &str2) const{
        std::string alphabet = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz_";
        
        return std::stoi(str1.substr(str1.find_first_not_of(alphabet), str1.length() - str1.find_first_not_of(alphabet) ) ) > std::stoi(str2.substr(str2.find_first_not_of(alphabet), str2.length() - str2.find_first_not_of(alphabet) ) );
    }
};

typedef std::pair<std::string, int> PairType_Str_Int;
struct CompareSecond_Min
{
    bool operator()(const PairType_Str_Int& left, const PairType_Str_Int& right) const
    {
        return left.second < right.second;
    }
};
struct CompareSecond_Max
{
    bool operator()(const PairType_Str_Int& left, const PairType_Str_Int& right) const
    {
        //return left.second > right.second;
	return left.second < right.second;
    }
};

int getMinForChannelOR(std::map<std::string, int> inputMap){
    //Variable Declaration
    std::map<std::string, int>::iterator iterMap = inputMap.begin();
    std::map<std::string, int>::iterator iterMapEnd = inputMap.end();

    while(iterMap != iterMapEnd){
	if( 0 == (*iterMap).second){
		std::map<std::string, int>::iterator iterTemp = iterMap;
		++iterMap;
		inputMap.erase(iterTemp);

		iterMapEnd = inputMap.end(); //??

		if(iterMap == iterMapEnd) break;
	}
	else{
		++iterMap;

		if(iterMap == iterMapEnd) break;
	}
    }
    
    std::pair<std::string, int> min = *min_element(inputMap.begin(), inputMap.end(), CompareSecond_Min());
    
    return min.second;
}

int getMaxForChannelAND(std::map<std::string, int> inputMap){
    //Variable Declaration
    int iRetVal;
    
    //Require
    if ( getMinForChannelOR(inputMap) > 0 ) {
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

} //End namespace
*/

int main( int argc_, char * argv_[]){
    //Variable Declaration
    bool bVerboseMode;
    
    float fVersion = 1.0;
    
    //size_t iLfBrace, iRtBrace, iComma;  //Position of left brace "{", right brace "}", and comma ","
    
    std::map<std::string,int> map_fTDCData;
    std::map<std::string,TH1F*,cmp_chan> map_fTDCHistos;
    
    std::string strName_InputRootFile;
    std::string strName_tree;           //TTree found in strName_InputRootFile
    std::string strCh, strChannels;
    std::string strTDCOR = "TDC_ChOR";
    std::string strTDCAND = "TDC_ChAND";
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
    TFile *file_InputROOTFile = new TFile(strName_InputRootFile.c_str(), "READ", "", 1);
    
    //Check to see if data file opened successfully, if so load the tree
    //------------------------------------------------------
    if ( !file_InputROOTFile->IsOpen() || file_InputROOTFile->IsZombie() ) {
        perror( ("main() - error while opening file: " + strName_InputRootFile ).c_str() );
        std::cout << "Input ROOT File Status:\n";
        std::cout << "\tIsOpen() = " << file_InputROOTFile->IsOpen() << endl;
        std::cout << "\tIsZombie() = " << file_InputROOTFile->IsZombie() << endl;
        std::cout << "Exiting!!!\n";
        
        return -4;
    }
    
    TTree *tree_Input = (TTree*) file_InputROOTFile->Get( strName_tree.c_str() );
    
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
            cout<<"Ch \t" << vec_strTDCChan[i];
            //cout<<"\t"<<vec_strTDCChan[i].find_first_not_of("TDC_Ch");
            //cout<<"\t"<<vec_strTDCChan[i].length() - vec_strTDCChan[i].find_first_not_of("TDC_Ch");
            //cout<<"\t"<<vec_strTDCChan[i].substr(vec_strTDCChan[i].find_first_not_of("TDC_Ch"), vec_strTDCChan[i].length() - vec_strTDCChan[i].find_first_not_of("TDC_Ch") );
            //cout<<"\t"<<std::stoi(vec_strTDCChan[i].substr(vec_strTDCChan[i].find_first_not_of("TDC_Ch"), vec_strTDCChan[i].length() - vec_strTDCChan[i].find_first_not_of("TDC_Ch") ) ) <<endl;
        }
    }
    
    for(int i=0; i < vec_strTDCChan.size(); ++i){
        //This is a hack...?
        //Yes, you need to initialize all entries in the map so they have memory addresses.
        //Then below in a separate loop you set them as a branch address
        map_fTDCData[vec_strTDCChan[i]];
    } //End Loop Over vec_strTDCChan
    
    for(int i=0; i < vec_strTDCChan.size(); ++i){
        tree_Input->SetBranchAddress( ("TDC_Ch" + vec_strTDCChan[i]).c_str(), &(map_fTDCData[vec_strTDCChan[i]]) );
        map_fTDCHistos[vec_strTDCChan[i]] = new TH1F( ("TDC_Ch" + vec_strTDCChan[i]).c_str(),"Timing",1200,0,1200);
        map_fTDCHistos[vec_strTDCChan[i]]->Sumw2();
    }
    
    //map_fTDCHistos[strTDCOR] = new TH1F( "TDC_ChOR","Timing",1200,0,1200);      map_fTDCHistos[strTDCOR]->Sumw2();
    //map_fTDCHistos[strTDCAND] = new TH1F( "TDC_ChAND","Timing",1200,0,1200);    map_fTDCHistos[strTDCAND]->Sumw2();
    
    TH1F *hTDC_ChOR = new TH1F( "TDC_ChOR","Timing - Channel OR",1200,0,1200);                  hTDC_ChOR->Sumw2();
    TH1F *hTDC_ChAND = new TH1F( "hTDC_ChAND","Timing - Channel AND",1200,0,1200);              hTDC_ChAND->Sumw2();
    //TH1F *hTDC_ChDeltaT = new TH1F( "hTDC_ChDeltaT","Timing - Channel #Deltat",1200,0,1200);    hTDC_ChDeltaT->Sumw2();
    TH1F *hTDC_ChDeltaT = new TH1F( "hTDC_ChDeltaT","Timing - Channel #Deltat",4800,-1200,1200);    hTDC_ChDeltaT->Sumw2();
    TH2F *hTDC_Corr = new TH2F("hTDC_Corr","Timing Correlation",1200,0,1200,1200,0,1200);
    
    hTDC_Corr->SetXTitle( map_fTDCData[vec_strTDCChan[0]].c_str() );
    hTDC_Corr->SetYTitle( map_fTDCData[vec_strTDCChan[1]].c_str() );
    
    //Loop Over Input Data
    //------------------------------------------------------
    for (int i=0; i < tree_Input->GetEntries(); ++i) {
        tree_Input->GetEntry(i);
        
        //Fill Individual Channels
        for (int i=0; i < vec_strTDCChan.size(); ++i) {
            if ( map_fTDCData[vec_strTDCChan[i]] > 0) {
                map_fTDCHistos[vec_strTDCChan[i]]->Fill(map_fTDCData[vec_strTDCChan[i]]);
            }
        }
        
        for (auto mapIter=map_fTDCData.begin(); mapIter!=map_fTDCData.end(); ++mapIter) {
            cout<<(*mapIter).second<<"\t";
        }
        
        cout<<"getMinForChannelOR(map_fTDCData) = " << getMinForChannelOR(map_fTDCData) << "\t";
        cout<<"getMaxForChannelAND(map_fTDCData) = " <<getMaxForChannelAND(map_fTDCData) << endl;
        
        //Fill OR && AND
        hTDC_ChOR->Fill( getMinForChannelOR(map_fTDCData) );
        hTDC_ChAND->Fill( getMaxForChannelAND(map_fTDCData) );
        
        if ( getMaxForChannelAND(map_fTDCData) > 0 ) {
            //hTDC_ChDeltaT->Fill( fabs( getMaxForChannelAND(map_fTDCData) - getMinForChannelOR(map_fTDCData) ) );
            hTDC_ChDeltaT->Fill( map_fTDCData[vec_strTDCChan[0]] - map_fTDCData[vec_strTDCChan[1]]  );
            
            hTDC_Corr->Fill(map_fTDCData[vec_strTDCChan[0]], map_fTDCData[vec_strTDCChan[1]]);
        }
        
    } //End Loop Over tree_Input
    
    //Create Output File
    //------------------------------------------------------
    TFile *file_OutputROOTFile = new TFile("multiDetTimeRes_Output.root","RECREATE","",1);
    
    /*for (auto mapIter=map_fTDCHistos.begin(); mapIter!=map_fTDCHistos.end(); ++mapIter) {
        ( (*mapIter).second )->Write();
    }*/

    for (int i=0; i < vec_strTDCChan.size(); ++i) {map_fTDCHistos[vec_strTDCChan[i]]->Write();}
    
    hTDC_ChOR->Write();
    hTDC_ChAND->Write();
    hTDC_ChDeltaT->Write();
    hTDC_Corr->Write();
    
    file_OutputROOTFile->Close();

    return 0;
} //End main()
