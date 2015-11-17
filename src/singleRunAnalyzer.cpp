//
//  singleRunAnalyzer.cpp
//
//
//  Created by Brian L Dorney on 24/10/15.
//
//

#ifndef ____singleRunAnalyzer__
#define ____singleRunAnalyzer__

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
#include "TCanvas.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TTree.h"

#ifdef __MAKECINT__
#pragma link C++ class vector<int>+;
//#pragma link C++ class vector<float>+;
#pragma link C++ class map<string,int>+;
#endif

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
    Timing::AnalysisSetup aSetup;
    
    bool bVerboseMode;
    
    float fVersion = 1.0;
    
    Run run;
    
    std::string strName_inputRootFile;
    std::string strName_tree;           //TTree found in strName_inputRootFile
    std::string strName_analysisFile;
    
    //std::string strCh, strChannels;
    
    TimingRunAnalyzer timingAnalyzer;
    
    std::vector<std::string> vec_strInputArgs;
    //std::vector<std::string> vec_strTDCChan;
    
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
        //std::cout<<"\t./singleRunAnalyzer <Input Root File> <Tree Name> <comma separated list of channel numbers; no spaces> <Analysis Config File> <verbose mode true/false>\n";
        std::cout<<"\t./singleRunAnalyzer <Input Root File> <Tree Name> <Analysis Config File> <verbose mode true/false>\n";
        std::cout<<"\tExample:\n";
        std::cout<<"\t\t./singleRunAnalyzer myRootFile.root myTreeName myAnalysis.txt false\n;";
        std::cout<<"\tExiting\n";
        
        return 1;
    } //End Case: Usage
    else if (vec_strInputArgs.size() == 2 && vec_strInputArgs[1].compare("-h") == 0) { //Case: Help Menu
        std::cout<<"singleRunAnalyzer v"<<fVersion<<endl;
        std::cout<<"Author: Brian L. Dorney\n";
        std::cout<<"For help menu:\n";
        std::cout<<"\t./singleRunAnalyzer -h\n";
        std::cout<<"Offline Analysis of multi detector time response:\n";
        //std::cout<<"\t./singleRunAnalyzer <Input Root File> <Tree Name> <comma separated list of channel numbers; no spaces> <Analysis Config File> <verbose mode true/false>\n";
        std::cout<<"\t./singleRunAnalyzer <Input Root File> <Tree Name> <Analysis Config File> <verbose mode true/false>\n";
        std::cout<<"\tExample:\n";
        std::cout<<"\t\t./singleRunAnalyzer myRootFile.root myTreeName myAnalysis.txt false\n;";
        std::cout<<"\tExiting\n";
        
        //Right now this is a duplicate of the above, maybe I add additional functionality later
        
        return 1;
    } //End Case: Help Menu
    else if(vec_strInputArgs.size() == 5){ //Case: Analysis Mode
        bool bExitSuccess = false;
        
        strName_inputRootFile   = vec_strInputArgs[1];
        strName_tree            = vec_strInputArgs[2];
        strName_analysisFile    = vec_strInputArgs[3];
        
        bVerboseMode            = convert2bool(vec_strInputArgs[4], bExitSuccess);
        if (!bExitSuccess) { //Case: Input Not Understood
            cout<<"main() - vec_strInputArgs[2] expected to be boolean!!!\n";
            cout<<"main() - Parameter given:\n";
            cout<<"\t"<<vec_strInputArgs[2]<<endl;
            cout<<"Exitting!!!\n";
            
            return -2;
        } //End Case: Input Not Understood
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
    
    aSetup = timingAnalyzer.getAnalysisSetup();
    
    //Setup the Run
    //------------------------------------------------------
    run.iRun = 0;       //Filler for Now
    run.iTrig_Mode = 0; //Asynchronous
    
    run.strRunName      = strName_inputRootFile;
    run.strTreeName_Run = strName_tree;
    
    //Setup the det maps
    for (auto iterDetSetup = aSetup.map_DetSetup.begin(); iterDetSetup != aSetup.map_DetSetup.end(); ++iterDetSetup) { //Loop Over Requested Detectors
        cout<<"Detector  = " << (*iterDetSetup).first << ";\t" << "TDC Channel = " << (*iterDetSetup).second.iTDC_Chan << endl;
        
        run.map_det[(*iterDetSetup).first].iDet_Pos = std::distance(aSetup.map_DetSetup.begin(), iterDetSetup);
        run.map_det[(*iterDetSetup).first].iTDC_Chan= (*iterDetSetup).second.iTDC_Chan;
    } //End Loop Over Requested Detectors
    
    //Setup the PMT maps
    for (auto iterPMTSetup = aSetup.map_PMTSetup.begin(); iterPMTSetup != aSetup.map_PMTSetup.end(); ++iterPMTSetup) { //Loop Over Requested PMTs
        run.map_PMT[(*iterPMTSetup).first].bIsTrig  = (*iterPMTSetup).second.bIsTrig;
        run.map_PMT[(*iterPMTSetup).first].iTDC_Chan= (*iterPMTSetup).second.iTDC_Chan;
    } //End Loop Over Requested PMTs
    
    //Analyze!
    //------------------------------------------------------
    //cout<<"main() - No Fault 1\n";
    
    timingAnalyzer.analyzeRun(run);
    
    //cout<<"main() - No Fault 2\n";
    //cout<<"main() - No Fault 3\n";
    
    //Store
    //------------------------------------------------------
    TFile *file_Output = new TFile("singleRunAnalyzer_Output.root","RECREATE","",1);
    
    int iColor = 0;
    
    //Timeline Canvas
    TCanvas *cTimeView = new TCanvas("cTimeView","Event Timing",700,700);
    cTimeView->cd();
    cTimeView->cd()->SetLogy();
    
    TLegend *leg_EvtTime = new TLegend(0.2,0.2,0.6,0.4);
    leg_EvtTime->SetLineColor(kWhite);
    leg_EvtTime->SetFillColor(kWhite);
    
    //PMT's
    for (auto iterPMT = run.map_PMT.begin(); iterPMT != run.map_PMT.end(); ++iterPMT) { //Loop Over PMT's
        TDirectory *dir_thisPMT = file_Output->mkdir( ( (*iterPMT).first ).c_str() );
        
        dir_thisPMT->cd();
        ((*iterPMT).second).timingResults.hTDC_Histo->Write();
        
        //Draw
        ((*iterPMT).second).timingResults.hTDC_Histo->SetLineColor(getCyclicColor(iColor) );
        leg_EvtTime->AddEntry( (&*((*iterPMT).second).timingResults.hTDC_Histo), (*iterPMT).first.c_str(), "LPE" );
        if (iterPMT == run.map_PMT.begin() ) { //Case: First Element
            ((*iterPMT).second).timingResults.hTDC_Histo->GetXaxis()->UnZoom();
            ((*iterPMT).second).timingResults.hTDC_Histo->Draw();
            
        } //End Case: First Element
        else { //Case: All Other Elements
            ((*iterPMT).second).timingResults.hTDC_Histo->Draw("same");
        } //End Case: All Other Elements
        
        //Increment iColor
        iColor++;
    } //End Loop Over PMT's
    
    //Detectors
    for (auto iterDet = run.map_det.begin(); iterDet != run.map_det.end(); ++iterDet) { //Loop Over Detectors
        TDirectory *dir_thisDet = file_Output->mkdir( ( (*iterDet).first ).c_str() );
        
        dir_thisDet->cd();
        ((*iterDet).second).timingResults.func_TDC_Fit->Write();
        ((*iterDet).second).timingResults.hTDC_Histo->Write();
        
        //Draw()
        ((*iterDet).second).timingResults.hTDC_Histo->SetLineColor(getCyclicColor(iColor) );
        leg_EvtTime->AddEntry( (&*((*iterDet).second).timingResults.hTDC_Histo), (*iterDet).first.c_str(), "LPE" );
        ((*iterDet).second).timingResults.hTDC_Histo->Draw("same");
    } //End Loop Over Detectors
    
    //Detector OR
    TDirectory *dir_detOR = file_Output->mkdir( "Detector_OR" );
    dir_detOR->cd();
    run.timingResultsOR.func_TDC_Fit->Write();
    run.timingResultsOR.hTDC_Histo->Write();
    
    //Detector AND
    TDirectory *dir_detAND = file_Output->mkdir( "Detector_AND" );
    dir_detAND->cd();
    run.hTDC_DeltaT->Write();
    run.hTDC_Correlation->Write();
    run.timingResultsAND.func_TDC_Fit->Write();
    run.timingResultsAND.hTDC_Histo->Write();
    
    //Event Time Canvas
    leg_EvtTime->Draw("same");
    cTimeView->Write();
    
    //Close the File
    file_Output->Close();
    
    return 0;
} //End main()

#endif /* defined(____singleRunAnalyzer__) */
