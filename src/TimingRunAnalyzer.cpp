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
using std::ifstream;
using std::map;
using std::pair;
using std::string;
using std::transform;
using std::vector;

//My namespaces
using namespace Timing;

//Constructor
Timing::TimingRunAnalyzer::TimingRunAnalyzer(){
    bRunSet = false;
    
    bVerbose_Logging        = false;
    bVerboseMode_Analysis   = false;
    bVerboseMode_IO         = false;
    
    fTDCResolution          = 0.3;  //300 picoseconds for a 1200ns acquisition window
    
    fStream_Log.open("logs/log_analyzer.txt", std::ios::out);    //Should overwrite the file
    
    strSecBegin_ANAL= "[BEGIN_ANALYSIS_INFO]";
    strSecBegin_AND = "[BEGIN_AND_INFO]";
    strSecBegin_DET = "[BEGIN_DETECTOR_INFO]";
    strSecBegin_OR  = "[BEGIN_OR_INFO]";
    strSecBegin_PMT = "[BEGIN_PMT_INFO]";
    
    strSecEnd_ANAL  = "[END_ANALYSIS_INFO]";
    strSecEnd_AND   = "[END_AND_INFO]";
    strSecEnd_DET   = "[END_DETECTOR_INFO]";
    strSecEnd_OR    = "[END_OR_INFO]";
    strSecEnd_PMT    = "[END_PMT_INFO]";
} //End Constructor Timing::TimingRunAnalyzer::TimingRunAnalyzer()

//Analyze method, this is the central access point for the analysis
//Inherited class can over ride this method but it MUST exist
//The entire analysis needs to be executed through this method (but not necessarily in this method)
void Timing::TimingRunAnalyzer::analyzeRun(Timing::Run &run){
    //Variable Declaration
    int iNEvtSel = 0;
    
    map<string,int> map_iTDCData_Det;
    map<string,int> map_iTDCData_PMT;
    
    map<string,TF1> map_fTDCFits;
    map<string,TH1F> map_fTDCHistos;
    
    TF1 func_TDC_Fit_AND;
    TF1 func_TDC_Fit_OR;
    
    TFile *file_ROOT_Run = NULL;
    
    //Setup the OR, AND, deltaT, & correlation histograms
    TH1F hTDC_AND = getHistogram( aSetup.setupAND, run );
    TH1F hTDC_DeltaT( ("hTDC_DeltaT_R" + getString(run.iRun) ).c_str(),"DeltaT:t_{Det1} - t_{Det2} #left( ns #right): A.U.",2 * aSetup.fTDCWinSize, -1. * aSetup.fTDCWinSize, aSetup.fTDCWinSize );
    TH2F hTDC_Correlation( ("hTDC_Corr_R" + getString(run.iRun) ).c_str(),"Correlation",aSetup.fTDCWinSize, 0., aSetup.fTDCWinSize,aSetup.fTDCWinSize, 0., aSetup.fTDCWinSize );
    TH1F hTDC_OR = getHistogram( aSetup.setupOR, run );
    
    TTree *tree_Run = nullptr;

    vector<string> vec_strMapDetKeyVal; //List of detector names for random access to the map...
    
    //Set Errors on DeltaT & Correlation histograms (this is done automatically for those created with getHistogram)
    hTDC_DeltaT.Sumw2();
    hTDC_Correlation.Sumw2();
    
    //Open this run's root file
    //------------------------------------------------------
    file_ROOT_Run = new TFile(run.strRunName.c_str(),"READ","",1);

    //cout<<"TimingRunAnalyzer::analyzeRun() - run.strRunName = " << run.strRunName << endl;

    //Check to see if data file opened successfully, if so load the tree
    //------------------------------------------------------
    if ( !file_ROOT_Run->IsOpen() || file_ROOT_Run->IsZombie() ) { //Case: failed to load ROOT file
        perror( ("Timing::TimingRunAnalyzer::analyzeRun() - error while opening file: " + run.strRunName ).c_str() );
        printROOTFileStatus(file_ROOT_Run);
        std::cout << "Exiting!!!\n";
        
        return;
    } //End Case: failed to load ROOT file
    
    //cout<<" run.strTreeName_Run = " << run.strTreeName_Run << endl;
    
    tree_Run = (TTree*) file_ROOT_Run->Get( run.strTreeName_Run.c_str() );

    if ( nullptr == tree_Run ) { //Case: failed to load TTree
        std::cout<<"Timing::TimingRunAnalyzer::analyze() - error while fetching: " << run.strTreeName_Run << endl;
        std::cout<<"\tTree Returns nullptr; tree_Run = " << tree_Run << endl;
        std::cout<<"Exiting!!!\n";
        
        return;
    } //End Case: failed to load TTree
    
    //Initialize Tree Branch Address for each Detector
    //------------------------------------------------------
    //HACK Explanation
    //Initialize map_iTDCData_Det for each object
    //Then we will set the TBranch address for each detector (e.g. TDC Channel)
    //Then we will get the data in the most hacked way imaginable; YEAH ROOT
    
    //Initialize map_iTDCData_Det for each detector
    for (auto iterDet = aSetup.map_DetSetup.begin(); iterDet != aSetup.map_DetSetup.end(); ++iterDet ){ //Loop Through Detectors
        //Create this detector in the maps
        map_iTDCData_Det[(*iterDet).first];
        
        //Form histogram for this detector
        map_fTDCHistos[(*iterDet).first] = getHistogram( aSetup.map_DetSetup[(*iterDet).first], run );
        
        //Store the key values
        vec_strMapDetKeyVal.push_back((*iterDet).first);
    } //End Loop Through Detectors
    
    //Set TBranch Address for each detector (e.g. TDC Channel)
    //  NOTE: Yes it needs to be done in TWO separate loops; yes ROOT is stupid
    for (auto iterDet = aSetup.map_DetSetup.begin(); iterDet != aSetup.map_DetSetup.end(); ++iterDet ){ //Loop Through Detectors
        //Set branch address
        tree_Run->SetBranchAddress( ("TDC_Ch" + getString( ((*iterDet).second).iTDC_Chan ) ).c_str(), &(map_iTDCData_Det[(*iterDet).first]) );
    } //End Loop Through Detectors
    
    //Set Axis titles on Correlation histogram
    if ( vec_strMapDetKeyVal.size() > 1 ){
        hTDC_Correlation.SetXTitle( vec_strMapDetKeyVal[0].c_str() );
        hTDC_Correlation.SetYTitle( vec_strMapDetKeyVal[1].c_str() );
    }
    
    //Initialize Tree Branch Address each PMT
    //------------------------------------------------------
    //HACK Explanation
    //Initialize map_iTDCData_PMT for each object
    //Then we will set the TBranch address for each PMT (e.g. TDC Channel)
    //Then we will get the data in the most hacked way imaginable; YEAH ROOT
    
    //Initialize map_iTDCData_PMT
    for (auto iterPMT = aSetup.map_PMTSetup.begin(); iterPMT != aSetup.map_PMTSetup.end(); ++iterPMT) { //Loop through PMT's
        //Create this detector in the map
        map_iTDCData_PMT[(*iterPMT).first];
        
        //Form histogram for this PMT
        map_fTDCHistos[(*iterPMT).first] = getHistogram( aSetup.map_PMTSetup[(*iterPMT).first], run );
    } //End Loop through PMT's
    
    //Set TBranch Address for each PMT (e.g. TDC Channel)
    //  NOTE: Yes it needs to be done in TWO separate loops; yes ROOT is stupid
    for (auto iterPMT = aSetup.map_PMTSetup.begin(); iterPMT != aSetup.map_PMTSetup.end(); ++iterPMT ){ //Loop Through PMT's
        //Set branch address
        tree_Run->SetBranchAddress( ("TDC_Ch" + getString( ((*iterPMT).second).iTDC_Chan ) ).c_str(), &(map_iTDCData_PMT[(*iterPMT).first]) );
    } //End Loop Through PMT's
    
    //Get data event-by-event
    //------------------------------------------------------
    //NOTE: Above we are working only with the maps in aSetup (type struct AnalysisSetup)
    //After the Event selection we are going to switch with working with the maps in the input run
    //the user should have configured the run to have the Det's/PMT's they are interested in already initialized
    for (int i=0; i < tree_Run->GetEntries(); ++i) { //Loop Over Events
        tree_Run->GetEntry(i);
        
        if (i % 1000 == 0) cout<<"Run "<< run.iRun<< "; " <<i<<" Events Analyzed\n";
        
        //BEGIN EVENT SELECTION
        //==================================================
        run.iEvtPassing[kEvt_All]++; //All Events
        
        //Are PMT's in time?
        if (rejectEvtPMTsOOT(aSetup, map_iTDCData_PMT) ) continue;
        
        run.iEvtPassing[kEvt_OOT_PMT]++; //Events Passing have PMT's in time
        
        //Are Detector's in time? (NOTE: if only 1 nonzero det evt is accepted automatically)
	if ( !(getMinForChannelOR(map_iTDCData_Det) > 0) ) continue;	//veto all events where there are no nonzero detectors
        if (getDeltaTForChannel(map_iTDCData_Det) > aSetup.fCut_MaxDeltaT_Det ) continue;
        
        run.iEvtPassing[kEvt_OOT_Det]++; //Events Passing have Detectors in time
        
        //Are Detector's in time with the trigger?
        if (rejectEvtDetsOOT(aSetup, map_iTDCData_PMT, map_iTDCData_Det) ) continue;
        
        run.iEvtPassing[kEvt_OOT_DetTrig]++;
        
        //END EVENT SELECTION
        //==================================================
        
        //Store Detector Data
        for (auto iterDet = aSetup.map_DetSetup.begin(); iterDet != aSetup.map_DetSetup.end(); ++iterDet){ //Loop Over Detectors
            //Correct for inverted times due to common_stop technique of TDC
            //  NOTE: this does not mean 0 is the trigger if this correction is made
            //        this moves the trigger time from t=0 to t=aSetup.fTDCWinSize
            if (aSetup.bInvertTime){ //Case: Invert non-zer times
                run.map_det[(*iterDet).first].vec_iTDC_Data.push_back( getInvertedTime( map_iTDCData_Det[(*iterDet).first] ) );
            } //End Case: Invert non-zero times
            else{ //Case: Use Raw Times
                run.map_det[(*iterDet).first].vec_iTDC_Data.push_back( map_iTDCData_Det[(*iterDet).first] );
            } //End Case: Use Raw Times
        } //End Loop Over Detectors
        
        //Store PMT Data
        for (auto iterPMT = aSetup.map_PMTSetup.begin(); iterPMT != aSetup.map_PMTSetup.end(); ++iterPMT) { //Loop Over PMTs
            //Correct for inverted times due to common_stop technique of TDC
            //  NOTE: this does not mean 0 is the trigger if this correction is made
            //        this moves the trigger time from t=0 to t=aSetup.fTDCWinSize
            if (aSetup.bInvertTime){ //Case: Invert non-zer times
                run.map_PMT[(*iterPMT).first].vec_iTDC_Data.push_back( getInvertedTime( map_iTDCData_PMT[(*iterPMT).first] ) );
            } //End Case: Invert non-zero times
            else{ //Case: Use Raw Times
                run.map_PMT[(*iterPMT).first].vec_iTDC_Data.push_back( map_iTDCData_PMT[(*iterPMT).first] );
            } //End Case: Use Raw Times
        } //End Loop Over PMTs
    } //End Loop Over Events
    
    //Print Event Selection Summary to User
    //------------------------------------------------------
    iNEvtSel = run.iEvtPassing[kEvt_OOT_DetTrig];   //Be mindful if the enum kCutClasses changes or if the above selection changes
    
    cout<<"======================================================\n";
    cout<<"====================Selection Summary=================\n";
    cout<<"======================================================\n";
    for (int iCutCase=kEvt_All; iCutCase < n_cut_classes; ++iCutCase) { //Loop Over Cut Classes
        switch (iCutCase) { //Switch on loop index (iCutCase)
            case kEvt_OOT_PMT:      cout<<"\tEvt - PMT OOT\t" << run.iEvtPassing[iCutCase] << endl;   break;
            case kEvt_OOT_Det:      cout<<"\tEvt - Det OOT\t" << run.iEvtPassing[iCutCase] << endl;   break;
            case kEvt_OOT_DetTrig:  cout<<"\tEvt - Det, Trig OOT\t" << run.iEvtPassing[iCutCase] << endl;   break;
            default:                cout<<"\tEvt - All\t" << run.iEvtPassing[iCutCase] << endl;   break;
        } //End Switch on loop index (iCutCase)
    } //End Loop Over Cut Classes
    cout<<"======================================================\n";
    cout<<"======================================================\n";
    cout<<"======================================================\n";
    
    //Correct for offset in mean arrival time?
    //------------------------------------------------------
    //Consider only the case were number of detectors is greater than 2 (obviously)
    if (aSetup.bMatchArrivalTime && run.map_det.size() > 1) { //Case: Correct for Offsets in Arrival Time
        map<string, Timing::Detector, Timing::map_cmp_str>::iterator detMapBegin = run.map_det.begin();
        
        float fOffset = 0.;
        
        for (auto iterDet = run.map_det.begin(); iterDet != run.map_det.end(); ++iterDet) { //Loop Over Detectors
            //Skip the first element, it is stored above (maybe there's a better way to do this)
            if (run.map_det.begin() == iterDet) continue;
            
            fOffset = deltaMean( ((*detMapBegin).second).vec_iTDC_Data, ((*iterDet).second).vec_iTDC_Data );
            
            if (fOffset > fTDCResolution) { //Case: offset greater than TDC Resolution
                //Won't work, fOffset is not a "const float"
                //transform(((*iterDet).second).vec_iTDC_Data.begin(), ((*iterDet).second).vec_iTDC_Data.end(), ((*iterDet).second).vec_iTDC_Data.begin(), std::bind2nd(std::plus<float>(), fOffset)  )
                std::for_each(((*iterDet).second).vec_iTDC_Data.begin(), ((*iterDet).second).vec_iTDC_Data.end(), Timing::addVal(fOffset) );
            } //End Case: offset greater than TDC Resolution
        } //End Loop Over Detectors
    } //End Case: Correct for Offsets in Arrival Time
    
    //Efficiency is a round 2 thing
    
    //Fill Histograms
    //------------------------------------------------------
    for (int i=0; i < iNEvtSel; ++i) { //Loop through Events
        //Going to loop over objects in run.map_det & run.map_PMT
        //Access by index "i" elements of run.map_X[some_X].vec_iTDC_Data where X = {det,PMT}
        //By design run.map_X[some_X].vec_iTDC_Data.size() == iNEvtSel
        //For each i fill histograms, and reset value of map_iTDCData_Det & map_iTDCData_PMT
        
        //Fill Detector Histograms
        for (auto iterDet = run.map_det.begin(); iterDet != run.map_det.end(); ++iterDet) { //Loop Over Detectors
            //Reset the value of the map_iTDCData
            map_iTDCData_Det[(*iterDet).first] = ((*iterDet).second).vec_iTDC_Data[i];
            
            //cout<< map_iTDCData_Det[(*iterDet).first] << ";\t";

            //Fill the Histogram
            //Keep the entries of the histogram = iNEvtSel
            if ( ((*iterDet).second).vec_iTDC_Data[i] > 0 ) { //Case: Fill the Real Time
                map_fTDCHistos[((*iterDet).first)].Fill( ((*iterDet).second).vec_iTDC_Data[i] );
            } //End Case: Fill the Real Time
            else{ //Case: Fill a Negative Time
                map_fTDCHistos[((*iterDet).first)].Fill( -1 );
            } //End Case: Fill a Negative Time
        } //End Loop Over Detectors
        
        //Fill PMT Histograms
        for (auto iterPMT = run.map_PMT.begin(); iterPMT != run.map_PMT.end(); ++iterPMT) { //Loop Over PMT's
            //Reset the value of the map_iTDCData_PMT
            map_iTDCData_PMT[(*iterPMT).first] = ((*iterPMT).second).vec_iTDC_Data[i];
            
            //cout<< map_iTDCData_PMT[(*iterDet).first] << ";\t";
            
            //Fill the Histogram
            //Keep the entries of the histogram = iNEvtSel
            if ( ((*iterPMT).second).vec_iTDC_Data[i] > 0 ) { //Case: Fill the Real Time
                map_fTDCHistos[((*iterPMT).first)].Fill( ((*iterPMT).second).vec_iTDC_Data[i] );
            } //End Case: Fill the Real Time
            else{ //Case: Fill a Negative Time
                map_fTDCHistos[((*iterPMT).first)].Fill( -1 );
            } //End Case: Fill a Negative Time
        } //End Loop Over PMT's
        
        //cout<<"min = " << getMinForChannelOR(map_iTDCData_Det) << ";\tmax = " << getMaxForChannelAND(map_iTDCData_Det) << endl;

        //Fill the OR histogram
        //Keep the entries of the histogram = iNEvtSel
        if (getMinForChannelOR(map_iTDCData_Det) > 0){ //Case: Any Detector fired this event
            hTDC_OR.Fill( getMinForChannelOR(map_iTDCData_Det) );
        } //End Case: Any Detector fired this event
        else{ //Case: Fill a negative time
            hTDC_OR.Fill( -1 );
        } //End Case: Fill a negative time
        
        //Fill the AND, DeltaT, and Correlation histograms
        //Keep the entries of the histogram = iNEvtSel
        if (getMaxForChannelAND(map_iTDCData_Det) > 0){
            hTDC_AND.Fill( getMaxForChannelAND(map_iTDCData_Det) );
            
            //Right now only providing support for the 2 detector case
            //someone wants 3 they can write their own analyzer...
            //Need to use this hack so that deltaT and correlation are always the same
            if (map_iTDCData_Det.size() == 2) { //Case: Two Detectors
                hTDC_DeltaT.Fill( map_iTDCData_Det[vec_strMapDetKeyVal[0]] - map_iTDCData_Det[vec_strMapDetKeyVal[1]] );
                hTDC_Correlation.Fill( map_iTDCData_Det[vec_strMapDetKeyVal[0]], map_iTDCData_Det[vec_strMapDetKeyVal[1]] );
            } //End Case: Two Detectors
        } //End Case: Both Detectors fired this event
        else{ //Case: Fill a negative time
            hTDC_AND.Fill( -1 );
            hTDC_Correlation.Fill( -1, -1 );
            hTDC_DeltaT.Fill( -10000 ); //Can't use -1 here, it might actually be -1!!!!
        } //End Case: Fill a negative time
    } //End Loop through Events
    
    //Record Detector Data
    //------------------------------------------------------
    //set performance data for det's
    for (auto iterDet = run.map_det.begin(); iterDet != run.map_det.end(); ++iterDet) { //Loop Over Detectors
        setPerformanceData( ((*iterDet).second).timingResults, map_fTDCHistos[(*iterDet).first] );
    } //End Loop Over Detectors
    
    //set performance data for PMT's
    for (auto iterPMT = run.map_PMT.begin(); iterPMT != run.map_PMT.end(); ++iterPMT) { //Loop Over PMT's
        setPerformanceData( ((*iterPMT).second).timingResults, map_fTDCHistos[(*iterPMT).first] );
    } //End Loop Over PMT's
    
    //Store the AND and the OR of the detectors
    setPerformanceData( run.timingResultsAND, hTDC_AND );
    setPerformanceData( run.timingResultsOR, hTDC_OR );
    
    //Fit Histograms
    //------------------------------------------------------
    //Right now the fitting is only supported for the Asynchronous mode
    if ( 0 == run.iTrig_Mode) { //Case: Async Trigger Mode
        //Initialize map_fTDCFits for each detector and then fit
        for (auto iterDet = run.map_det.begin(); iterDet != run.map_det.end(); ++iterDet) { //Loop Through Detectors
            //Initialize
            map_fTDCFits[(*iterDet).first] = getFunction( aSetup.map_DetSetup[(*iterDet).first], map_fTDCHistos[(*iterDet).first], run );
            
            //Fit histogram
            fitHistogram( aSetup.map_DetSetup[(*iterDet).first], map_fTDCHistos[(*iterDet).first], map_fTDCFits[(*iterDet).first] );
            
            //Store the performance data
            setPerformanceData( ((*iterDet).second).timingResults, map_fTDCFits[(*iterDet).first], aSetup.map_DetSetup[(*iterDet).first] );
        } //End Loop Through Detectors
        
        //Initialize AND & OR fits
        func_TDC_Fit_AND= getFunction( aSetup.setupAND, hTDC_AND, run );
        func_TDC_Fit_OR = getFunction( aSetup.setupOR, hTDC_OR, run );
        
        //Fit AND & OR
        fitHistogram(aSetup.setupAND, hTDC_AND, func_TDC_Fit_AND);
        fitHistogram(aSetup.setupOR, hTDC_OR, func_TDC_Fit_OR);
        
        //Store the performance data for the AND & OR
        setPerformanceData( run.timingResultsAND, func_TDC_Fit_AND, aSetup.setupAND );
        setPerformanceData( run.timingResultsOR, func_TDC_Fit_OR, aSetup.setupOR );
    } //End Case: Async Trigger Mode
    
    //Store Objects/Parameters into the Run
    //We are done with the analysis now, store everything into the run
    //  NOTE: We do not work with the TObjects stored in the run above because passing the pointers "TH1F *" and "TF1 *" around has lead to problems in the past
    //------------------------------------------------------
	
	run.hTDC_Correlation = std::make_shared<TH2F>(hTDC_Correlation);
	run.hTDC_DeltaT	= std::make_shared<TH1F>(hTDC_DeltaT);

	run.hTDC_DeltaT->SetDirectory(gROOT);
	run.hTDC_Correlation->SetDirectory(gROOT);
    
    //Clear stl containers? (Not doing this seems to cause some pointer to be freed)
    //------------------------------------------------------
    map_iTDCData_Det.clear();
    map_iTDCData_PMT.clear();
    map_fTDCFits.clear();
    map_fTDCHistos.clear();

    vec_strMapDetKeyVal.clear();

    //Close the file
    //------------------------------------------------------
    
    //Debugging
    //printROOTFileStatus(file_ROOT_Run);
    //cout<<"Timing::TimingRunAnalyzer::analyzeRun() - file_ROOT_Run = " << file_ROOT_Run << endl;

    //cout<<"Timing::TimingRunAnalyzer::analyzeRun() - Trying to close file\n";
    file_ROOT_Run->Close();
    //cout<<"Timing::TimingRunAnalyzer::analyzeRun() - successfully closed file\n";    

    return;
} //End Timing::TimingRunAnalyzer::analyze()

//Fits an input histogram based on parameters given
void Timing::TimingRunAnalyzer::fitHistogram(HistoSetup &setupHisto, TH1F & hInput, TF1 &funcInput){
    
    if (setupHisto.bFit_AutoRanging) {
        hInput.Fit(&funcInput, setupHisto.strFit_Option.c_str(), "", hInput.GetMean() - 4. * hInput.GetRMS(), hInput.GetMean() + 4. * hInput.GetRMS() );
    }
    else{
        hInput.Fit(&funcInput, setupHisto.strFit_Option.c_str() );
    }
    
    return;
} //End Timing::TimingRunAnalyzer::fitHistogram

//Used in event selection
//Returns false (true) if Det's are in (out of) time with trigger based on specified user input
bool Timing::TimingRunAnalyzer::rejectEvtDetsOOT(AnalysisSetup &aSetup, std::map<std::string,int> mapInputPMTData, std::map<std::string,int> mapInputDetData){
    //Variable Declaration
    //Variable Declaration
    int iMaxTime = 0;   //Maximum Det time
    int iMinTime = 0;   //Minimum Det time
    
    int iTrigTime = 0;  //Time of the trigger
    int iTrigCount = 0; //Number of "triggers" (track to make sure the user didn't make a mistake)
    
    vector<int> vec_iTDC_Data;
    
    for (auto iterPMT = mapInputPMTData.begin(); iterPMT != mapInputPMTData.end(); ++iterPMT) { //Loop Over input PMT Data
        
        if ( aSetup.map_PMTSetup[(*iterPMT).first].bIsTrig ) { //Case: This PMT is the coincidence determining PMT
            iTrigTime = (*iterPMT).second;
            ++iTrigCount;
        } //End Case: This PMT is the coincidence determining PMT
    } //End Loop Over input PMT Data
    
    //Cross-check input: Nonzero Trigger Time
    if ( !(iTrigTime > 0) ) {
        cout<<"Timing::TimingRunAnalyzer::rejectEvtPMTsOOT() - Problem?\n";
        cout<<"\tiTrigTime = " << iTrigTime << endl;
        cout<<"\tThis is expected to be non-zero\n";
        cout<<"\tHave you setup the analysis config file correctly?\n";
    }
    
    //Cross-check input: No Trigger Time!
    //The user will for sure check for a problem if all events are rejected
    //In principle this should never happen since this method is called after rejectEvtPMTsOOT
    if ( !(iTrigCount == 1) ) {
        cout<<"Timing::TimingRunAnalyzer::rejectEvtPMTsOOT() - Problem!!!\n";
        cout<<"\tiTrigCount = " << iTrigCount << endl;
        cout<<"\tThis is expected to be non-zero\n";
        cout<<"\tHave you setup the analysis config file correctly?\n";
        cout<<"\tONLY ONE PMT must have IsTrigger set to true\n";
        cout<<"\tI am Rejecting ALL EVENTS\n";
        
        return true;
    }
    
    for (auto iterDet = mapInputDetData.begin(); iterDet != mapInputDetData.end(); ++iterDet) { //Loop Over input Det Data
        if ( (*iterDet).second > 0 ) { vec_iTDC_Data.push_back( (*iterDet).second); }
    } //End Loop Over input Det Data
    
    if( !(vec_iTDC_Data.size() > 0) ) return false;

    iMaxTime = *std::max_element(vec_iTDC_Data.begin(), vec_iTDC_Data.end() );
    iMinTime = *std::min_element(vec_iTDC_Data.begin(), vec_iTDC_Data.end() );
    
    if ( abs(iMaxTime - iTrigTime) > aSetup.fCut_MaxDeltaT_DetTrig || abs(iMinTime - iTrigTime) > aSetup.fCut_MaxDeltaT_DetTrig ) { //Check to see if detector is in time with trigger
        return true;
    } //End Check to see if detector is in time with trigger
    else{ //Event Passes
        return false;
    } //End Event Passes
} //End Timing::TimingRunAnalyzer::rejectEvtDetsOOT

//Used in event selection
//Returns false (true) if PMT's are in (out of) time based on specified user input
bool Timing::TimingRunAnalyzer::rejectEvtPMTsOOT(AnalysisSetup &aSetup, map<string,int> mapInputPMTData){
    //Variable Declaration
    int iMaxTime = 0;   //Maximum PMT time
    int iMinTime = 0;   //Minimum PMT time
    
    int iTrigTime = 0;  //Time of the trigger
    int iTrigCount = 0; //Number of "triggers" (track to make sure the user didn't make a mistake)
    
    vector<int> vec_iTDC_Data;
    
    for (auto iterPMT = mapInputPMTData.begin(); iterPMT != mapInputPMTData.end(); ++iterPMT) { //Loop Over input PMT Data
        
        if ( aSetup.map_PMTSetup[(*iterPMT).first].bIsTrig ) { //Case: This PMT is the coincidence determining PMT
            iTrigTime = (*iterPMT).second;
            ++iTrigCount;
        } //End Case: This PMT is the coincidence determining PMT
        else{ //Case: All other PMT's
            vec_iTDC_Data.push_back( (*iterPMT).second );
        } //End Case: All other PMT's
    } //End Loop Over input PMT Data
    
    iMaxTime = *std::max_element(vec_iTDC_Data.begin(), vec_iTDC_Data.end() );
    iMinTime = *std::min_element(vec_iTDC_Data.begin(), vec_iTDC_Data.end() );
    
    //Cross-check input: Nonzero Trigger Time
    if ( !(iTrigTime > 0) ) {
        cout<<"Timing::TimingRunAnalyzer::rejectEvtPMTsOOT() - Problem?\n";
        cout<<"\tiTrigTime = " << iTrigTime << endl;
        cout<<"\tThis is expected to be non-zero\n";
        cout<<"\tHave you setup the analysis config file correctly?\n";
    }
    
    //Cross-check input: No Trigger Time!
    //The user will for sure check for a problem if all events are rejected
    if ( !(iTrigCount == 1) ) {
        cout<<"Timing::TimingRunAnalyzer::rejectEvtPMTsOOT() - Problem!!!\n";
        cout<<"\tiTrigCount = " << iTrigCount << endl;
        cout<<"\tThis is expected to be non-zero\n";
        cout<<"\tHave you setup the analysis config file correctly?\n";
        cout<<"\tONLY ONE PMT must have IsTrigger set to true\n";
        cout<<"\tI am Rejecting ALL EVENTS\n";
        
        return true;
    }
    
    //Perform the selection
    if ( (iMaxTime - iMinTime ) > aSetup.fCut_MaxDeltaT_PMT ){ //Check: non-coincidence determining PMT's out of time?
        return true;
    } //End Check: Check: non-coincidence determining PMT's out of time?
    else if( abs(iMaxTime - iTrigTime) > aSetup.fCut_MaxDeltaT_PMTTrig || abs(iMinTime - iTrigTime ) > aSetup.fCut_MaxDeltaT_PMTTrig ){ //Check: Coincidence determining PMT out of time?
        return true;
    } //End Check: Coincidence determining PMT out of time?
    else{ //Event Passes
        return false;
    } //End Event Passes
} //End Timing::TimingRunAnalyzer::rejectEvtPMTsOOT

//Returns a TF1 (function) whose parameters are set by setupHisto
//Good candidate for TimingUtitlityFunctions.h?
TF1 Timing::TimingRunAnalyzer::getFunction(HistoSetup &setupHisto, TH1F & hInput, Run & run){
    //Variable Declaration
    TF1 ret_Func(setupHisto.strFit_Name.c_str(), setupHisto.strFit_Formula.c_str(), setupHisto.fHisto_xLower, setupHisto.fHisto_xUpper);
    
    //Append the run number to the function name
    if( bRunSet ){
        ret_Func.SetName( ( setupHisto.strFit_Name + "_R" + getString(run.iRun) ).c_str() );
    }
    else{
        //In principle the run should always be set before this is called, but YOLO errors?
        ret_Func.SetName( ( setupHisto.strFit_Name + "_R0" ).c_str() );
    }
    
    //Set Fit Parameters
    //------------------------------------------------------
    //Check to see if user has correctly linking the meaning of the fit parameters and their initial guess
    if ( setupHisto.vec_strFit_ParamMeaning.size() == setupHisto.vec_strFit_ParamIGuess.size() ) {
        
        //AMPLITUDE, MEAN, SIGMA
        //We assume the user has correctly matched the indices of setupHisto.vec_strFit_ParamMeaning and setupHisto.vec_strFit_ParamIGuess to the formula given in setupHisto.strFit_Formula
        for (int i=0; i<setupHisto.vec_strFit_ParamMeaning.size(); ++i) {
            //Try to automatically assign a value
            if (0 == setupHisto.vec_strFit_ParamIGuess[i].compare("AMPLITUDE") ) { //Case: Histo Amplitude
            	ret_Func.SetParameter(i, hInput.GetBinContent( hInput.GetMaximumBin() ) );
            } //End Case: Histo Amplitude
            else if (0 == setupHisto.vec_strFit_ParamIGuess[i].compare("MEAN") ) { //Case: Histo Mean
                ret_Func.SetParameter(i, hInput.GetMean() );
            } //End Case: Histo Mean
            else if (0 == setupHisto.vec_strFit_ParamIGuess[i].compare("SIGMA") ) { //Case: Histo RMS
                ret_Func.SetParameter(i, hInput.GetRMS() );
            } //End Case: Histo RMS
            else{ //Case: manual assignment
                ret_Func.SetParameter(i, stofSafe("Fit_Param_IGuess", setupHisto.vec_strFit_ParamIGuess[i] ) );
            } //End Case: manual assignment
        } //End Loop over parameters
    } //End Case: equal number of inputs, containers (should be) linked!!!
    else{
        cout<<"Timing::TimingRunAnalyzer::getFunction() - Trying to set initial parameters for:\n";
        cout<<"\tHisto Name = " << hInput.GetName() << endl;
        cout<<"\tFunction Name = " << ret_Func.GetName() << endl;
        
        cout<<"\tFit_Param_Map (Parameter Meaning) Given:\n\t\t";
        for (int i = 0; i < setupHisto.vec_strFit_ParamMeaning.size(); ++i) {
            cout<<setupHisto.vec_strFit_ParamMeaning[i]<<"\t";
        }
        cout<<endl;
        
        cout<<"tFit_Param_IGuess (Parameter Initial Guess) Given:\n\t\t";
        for (int i = 0; i < setupHisto.vec_strFit_ParamIGuess.size(); ++i) {
            cout<<setupHisto.vec_strFit_ParamIGuess[i]<<"\t";
        }
        cout<<endl;
        cout<<"\tParameters not linked!!! Please cross check input analysis config file!!!\n";
    } //End Case: vec_strFit_ParamIGuess not linked with vec_strFit_ParamMeaning; skip
    
    return ret_Func;
} //End getFunction()

//Returns a TH1F (histogram) whose parameters are set by setupHisto
//Good candidate for TimingUtitlityFunctions.h
TH1F Timing::TimingRunAnalyzer::getHistogram(HistoSetup &setupHisto, Run & run){
    //Variable Declaration
    TH1F ret_Histo(setupHisto.strHisto_Name.c_str(), "", setupHisto.iHisto_nBins, setupHisto.fHisto_xLower, setupHisto.fHisto_xUpper );
    
    //Append the run number to the histogram name
    ret_Histo.SetName( ( setupHisto.strHisto_Name + "_R" + getString(run.iRun) ).c_str() );
    //if( bRunSet ){
        //ret_Histo.SetName( ( setupHisto.strHisto_Name + "_R" + getString(run.iRun) ).c_str() );
    //}
    //else{
        //In principle the run should always be set before this is called, but YOLO errors?
        //ret_Histo.SetName( ( setupHisto.strHisto_Name + "_R0" ).c_str() );
    //}
    
    ret_Histo.SetXTitle( setupHisto.strHisto_Title_X.c_str() );
    ret_Histo.SetYTitle( setupHisto.strHisto_Title_Y.c_str() );
    
    ret_Histo.Sumw2();
    
    //ret_Histo.SetDirectory(gROOT);  //This should definitely go in setPerformanceData() and NOT here
    
    return ret_Histo;
} //End getHistogram

//Loads all relevant analysis parameters from a text file
void Timing::TimingRunAnalyzer::setAnalysisConfig(string &strInputFile){
    //Variable Declaration
    bool bExitSuccess = false;
    bool bSetup = false;
    //bool bInfoHeaderEnd = false;
    
    pair<string,string> pair_strParam; //<Field, Value>
    
    string strDetOrPMTHeading = "";
    string strLine = "";
    string strName = ""; //name of current detector;
    
    vector<string> vec_strList;
    
    //Open the Data File
    //------------------------------------------------------
    if (bVerboseMode_IO) { //Case: User Requested Verbose Error Messages - I/O
        cout<< "Timing::TimingRunAnalyzer::setAnalysisConfig(): trying to open and read: " << strInputFile << endl;
    } //End Case: User Requested Verbose Error Messages - I/O
    
    ifstream fStream( strInputFile.c_str() );
    
    //Check to See if Data File Opened Successfully
    //------------------------------------------------------
    if (!fStream.is_open() && bVerboseMode_IO) {
        perror( ("Timing::TimingRunAnalyzer::setAnalysisConfig(): error while opening file: " + strInputFile).c_str() );
        printStreamStatus(fStream);
    }
    
    //Loop Over data Input File
    //------------------------------------------------------
    //Read the file via std::getline().  Obey good coding practice rules:
    //  -first the I/O operation, then error check, then data processing
    //  -failbit and badbit prevent data processing, eofbit does not
    //See: http://gehrcke.de/2011/06/reading-files-in-c-using-ifstream-dealing-correctly-with-badbit-failbit-eofbit-and-perror/
    while ( getlineNoSpaces(fStream, strLine) ) {
        //Does the user want to comment out this line?
        if ( 0 == strLine.compare(0,1,"#") ) continue;
        
        //Identify Section Headers
        if ( 0 == strLine.compare(strSecBegin_ANAL ) ) {
            //Filler
        }
        else if ( 0 == strLine.compare(strSecBegin_AND) ) { //Case: AND SECTION
            //Set the flag to compute the AND of all detectors
            //aSetup.bCompute_AND = true;
            
            //Setup the Histogram struct for analysis
            setHistoSetup(strInputFile, fStream, aSetup.setupAND );
        } //End Case: AND SECTION
        else if ( 0 == strLine.compare(strSecBegin_DET) || 0 == strLine.compare(strSecBegin_PMT) ){ //Case: DET or PMT SECTION
            //So if the user has done this section correctly the next noncommented line should the detector name
            bSetup = false;  //reset at the start of each new detector section
            
            strDetOrPMTHeading = strLine; //This should be set only once per execution
            
            cout<<"TimingRunAnalyzer::setAnalysisConfig() - strDetOrPMTHeading = " << strDetOrPMTHeading << endl;

            while ( getlineNoSpaces(fStream, strLine) ) { //Loop through file to find "Name"
                //Does the user want to comment out this line?
                if ( 0 == strLine.compare(0,1,"#") ) continue;
                
                //Do we reach the end of the section? If so the user has configured the file incorrectly
                if ( 0 == strLine.compare(strSecEnd_DET) || 0 == strLine.compare(strSecEnd_PMT) ) { //Section End Reached Prematurely
                    cout<<"Timing::TimingRunAnalyzer::setAnalysisConfig() - I have reached the END of a Detector or PMT Heading\n";
                    cout<<"\tBut Have NOT found the 'Name' field\n"<<endl;
                    cout<<"\tYou have configured this heading incorrectly, the 'Name' field is expected to be the FIRST line after the Heading\n";
                    cout<<"\tThis object has been skipped, please cross-check\n";
                    
                    //Exit the Loop to find "Detector_Name"
                    break;
                } //End Case: Section End Reached Prematurely
                
                pair_strParam = getParsedLine(strLine,bExitSuccess);
                
                if (bExitSuccess) { //Case: Parameter Fetched Successfully
                    transform(pair_strParam.first.begin(),pair_strParam.first.end(),pair_strParam.first.begin(),toupper);
                    
                    if ( 0 == pair_strParam.first.compare("NAME") ) { //Case: Name found!
                        //Store name locally & convert to upper case
                        strName = pair_strParam.second;
			transform(strName.begin(), strName.end(), strName.begin(), toupper);
                        
                        //Setup the Histogram struct for analysis
                        HistoSetup hSetup;
                        
                        //Initialize the correct map with hSetup
                        if (0 == strDetOrPMTHeading.compare(strSecBegin_DET) ) { //Case: Detector Declaration
                            aSetup.map_DetSetup[pair_strParam.second] = hSetup;
                        } //End Case: Detector Declaration
                        else if (0 == strDetOrPMTHeading.compare(strSecBegin_PMT) ) { //Case: PMT Declaration
                            aSetup.map_PMTSetup[pair_strParam.second] = hSetup;
                        } //End Case: PMT Declaration
                        else{ //Case: Undefined Behavior
                            cout<<"Timing::TimingRunAnalyzer::setAnalysisConfig() - Brian you made a mistake, this output should not be possible\n";
                        } //End Case: Undefined Behavior
                        
                        //Set the correct exit flag
                        bSetup = true;
                        
                        //Exit the Loop to find "Detector_Name"
                        break;
                    } //End Case: Detector Name found!
                    else{ //Case: Detector Name not found!
                        cout<<"Timing::TimingRunAnalyzer::setAnalysisConfig() - I am parsing a Detector or PMT Heading\n";
                        cout<<"\tHowever I expected the 'Name' field to be the first line after the heading\n";
                        cout<<"\tThe current line I am parsing: " << strLine << endl;
                        cout<<"\tHas been skipped and may lead to undefined behavior" << endl;
                    } //End Case: Name not found!
                }//End Case: Parameter Fetched Successfully
                else{ //Case: Parameter was NOT fetched Successfully
                    cout<<"Timing::TimingRunAnalyzer::setAnalysisConfig() - Sorry I didn't parse parameter correctly\n";
                    cout<<"\tWorking on file: " << strInputFile << endl;
                    cout<<"\tCurrent line: " << strLine << endl;
                    //cout<<"\tExpecting a detector name that matches one of detector names in the TreeSetup File\n";
                    //cout<<"\tPlease enter the correct name here (or press Ctrl+C to exit):\n"
                    //cin>>strLine;
                    
                    /*if (strLine.size() > 0) {
                     
                    }
                    else{
                        
                    }*/
                } //End Case: Parameter was NOT fetched Successfully
            } //Loop through file to find "Name"
            
            //Fetch the values for the map for the correct case
            if (bSetup) { //Case: Setup Correct
                //setHistoSetup(strInputFile, fStream, aSetup.map_DetSetup[strName] );
                //Store hSetup for the correct usage case
                if (0 == strDetOrPMTHeading.compare(strSecBegin_DET) ) { //Case: Detector Declaration
                    setHistoSetup(strInputFile, fStream, aSetup.map_DetSetup[strName] );
                } //End Case: Detector Declaration
                else if (0 == strDetOrPMTHeading.compare(strSecBegin_PMT) ) { //Case: PMT Declaration
                    setHistoSetup(strInputFile, fStream, aSetup.map_PMTSetup[strName] );
                } //End Case: PMT Declaration
                else{ //Case: Undefined Behavior
                    cout<<"Timing::TimingRunAnalyzer::setAnalysisConfig() - Brian you made a mistake, this output should not be possible\n";
                } //End Case: Undefined Behavior
            } //End Case: Setup Correct
            
            //Reset stored test (not sure I want to do this yet)
            //strDetOrPMTHeading = "";
            //strName = "";
        } //End Case: DET or PMT SECTION
        else if ( 0 == strLine.compare(strSecBegin_OR) ){ //Case: OR SECTION
            //Set the flag to compute the OR of all detectors
            //aSetup.bCompute_OR = true;
            
            //Setup the Histogram struct for analysis
            setHistoSetup(strInputFile, fStream, aSetup.setupOR );
        } //End Case: OR SECTION
        else if ( 0 == strLine.compare(strSecEnd_ANAL) ){ //Case: END OF FILE
            break;
        } //End Case: END OF FILE
        else{ //Case: Unsorted Parameters
            pair_strParam = getParsedLine(strLine, bExitSuccess);
            
            if (bExitSuccess) { //Case: Parameter Fetched Successfully
                transform(pair_strParam.first.begin(),pair_strParam.first.end(),pair_strParam.first.begin(),toupper);
                
                if( 0 == pair_strParam.first.compare("CUT_MAXDELTAT_DET") ){
                    aSetup.fCut_MaxDeltaT_Det = stofSafe(pair_strParam.first, pair_strParam.second);
                }
                else if( 0 == pair_strParam.first.compare("CUT_MAXDELTAT_DETTRIG") ){
                    aSetup.fCut_MaxDeltaT_DetTrig = stofSafe(pair_strParam.first, pair_strParam.second);
                }
                else if( 0 == pair_strParam.first.compare("CUT_MAXDELTAT_PMT") ){
                    aSetup.fCut_MaxDeltaT_PMT = stofSafe(pair_strParam.first, pair_strParam.second);
                }
                else if( 0 == pair_strParam.first.compare("CUT_MAXDELTAT_PMTTRIG") ){
                    aSetup.fCut_MaxDeltaT_PMTTrig = stofSafe(pair_strParam.first, pair_strParam.second);
                }
                else if( 0 == pair_strParam.first.compare("INVERT_TIMING") ){
                    aSetup.bInvertTime = convert2bool(pair_strParam.second, bExitSuccess);
                }
                else if( 0 == pair_strParam.first.compare("MATCH_ARRIVAL_TIME") ){
                    aSetup.bMatchArrivalTime = convert2bool(pair_strParam.second, bExitSuccess);
                }
                else if ( 0 == pair_strParam.first.compare("TDC_WINDOW_SIZE") ) {
                    aSetup.fTDCWinSize = stofSafe(pair_strParam.first, pair_strParam.second);
                }
                else{ //Case: Parameter not recognized
                    cout<<"Timing::TimingRunAnalyzer::setAnalysisConfig() - Unrecognized field!!!\n";
                    cout<<"\tField Name = " << pair_strParam.first << "; Value = " << pair_strParam.second << endl;
                    cout<<"\tParameter not set!!! Please cross-check config file: " << strInputFile << endl;
                    cout<<"\tMaybe you made a typo?\n";
                } //End Case: Parameter not recognized
            }//End Case: Parameter Fetched Successfully
            else{ //Case: Parameter was NOT fetched Successfully
                cout<<"Timing::TimingRunAnalyzer::setHistoSetup() - Sorry I didn't parse parameter correctly\n";
                cout<<"\tWorking on file: " << strInputFile << endl;
                cout<<"\tCurrent line: " << strLine << endl;
                cout<<"\tPlease cross-check! Skipping!!!\n";
            } //End Case: Parameter was NOT fetched Successfully
        } //End Case: Unsorted Parameters
        
        bExitSuccess = false; //reset for next iteration of file I/O
    } //End Loop Over Input File
    
    //Check to see if we had problems while reading the file
    if (fStream.bad() && bVerboseMode_IO) {
        perror( ("Timing::TimingRunAnalyzer::setAnalysisConfig(): error while reading file: " + strInputFile).c_str() );
        printStreamStatus(fStream);
    }
    
} //End Timing::TimingRunAnalyzer::setAnalysisConfig()

void Timing::TimingRunAnalyzer::setHistoSetup(std::string &strInputFile, std::ifstream &fStream, HistoSetup& setupHisto){
    //Variable Declaration
    bool bExitSuccess;
    
    pair<string,string> pair_strParam;

    string strLine;
    
    vector<string> vec_strList;
    
    //Loop through the section
    while ( getlineNoSpaces(fStream, strLine) ) {
        bExitSuccess = false;
        
        //Debugging
        //cout<<"strLine = " << strLine << endl;

        //Does the user want to comment out this line?
        if ( 0 == strLine.compare(0,1,"#") ) continue;
        
        //Has this section ended?
        if ( 0 == strLine.compare(strSecEnd_AND) || 0 == strLine.compare(strSecEnd_DET) || 0 == strLine.compare(strSecEnd_PMT) || 0 == strLine.compare(strSecEnd_OR) ) break;
        
        //Get Parameter Pairing
        pair_strParam = Timing::getParsedLine(strLine, bExitSuccess);
        
        //transform field name to upper case
        transform(pair_strParam.first.begin(),pair_strParam.first.end(),pair_strParam.first.begin(),toupper);
        
        //Parse pair
        if (bExitSuccess) { //Case: Parameter Fetched Successfully
            if( 0 == pair_strParam.first.compare("FIT_AUTORANGING") ){
                setupHisto.bFit_AutoRanging = Timing::convert2bool(pair_strParam.second, bExitSuccess);
                
                if (!bExitSuccess) {
                    cout<<"Timing::TimingRunAnalyzer::setAnalysisConfig() - Attempted to Assign 'Fit_Autoranging' value failed!!!\n";
                    cout<<"\tYour (Field name, Field value) pair = (" << pair_strParam.first << "," << pair_strParam.second << ")\n";
                    cout<<"\tField value should be from the set {t,true,1,f,false,0}\n";
                    cout<<"\tUndefined behavior may occur!!!";
                }
            }
            else if( 0 == pair_strParam.first.compare("FIT_FORMULA") ){
                setupHisto.strFit_Formula = pair_strParam.second;
            }
            else if( 0 == pair_strParam.first.compare("FIT_NAME") ){
                setupHisto.strFit_Name = pair_strParam.second;
            }
            else if( 0 == pair_strParam.first.compare("FIT_OPTION") ){
                setupHisto.strFit_Option = pair_strParam.second;
            }
            else if( 0 == pair_strParam.first.compare("FIT_PARAM_IGUESS") ){
                //setupHisto.vec_strFit_ParamIGuess = getCommaSeparatedList(pair_strParam.second);
                setupHisto.vec_strFit_ParamIGuess = getCharSeparatedList(pair_strParam.second,',');
            }
            else if( 0 == pair_strParam.first.compare("FIT_PARAM_MAP") ){
                //setupHisto.vec_strFit_ParamMeaning = getCommaSeparatedList(pair_strParam.second);
                setupHisto.vec_strFit_ParamMeaning = getCharSeparatedList(pair_strParam.second,',');
            }
            else if( 0 == pair_strParam.first.compare("HISTO_BINRANGE") ){
                //Get comma separated list
                //vec_strList = Timing::getCommaSeparatedList(pair_strParam.second);
                vec_strList = Timing::getCharSeparatedList(pair_strParam.second,',');
                
                //Debugging
                //for(int i=0; i<vec_strList.size(); ++i){
                //	cout<<"vec_strList["<<i<<"] = " << vec_strList[i] << endl;
                //}
                
                if (vec_strList.size() >= 2) { //Case: at least 2 numbers
                    //Fetch
                    setupHisto.fHisto_xLower = Timing::stofSafe(pair_strParam.first, vec_strList[0]);
                    setupHisto.fHisto_xUpper = Timing::stofSafe(pair_strParam.first, vec_strList[1]);
                    
                    //Reset to ensure they are both correctly lower & upper values
                    setupHisto.fHisto_xLower = std::min(setupHisto.fHisto_xLower, setupHisto.fHisto_xUpper);
                    setupHisto.fHisto_xUpper = std::max(setupHisto.fHisto_xLower, setupHisto.fHisto_xUpper);
                    
                    //Tell the user they entered more than what was expected
                    if (vec_strList.size() > 2) { //Case: 3 or more numbers
                        cout<<"Timing::TimingRunAnalyzer::setAnalysisConfig() - Sorry you entered 3 or more numbers for " << pair_strParam.first << endl;
                        cout<<"\tI have only used the first two and ignored the rest:\n";
                        cout<<"\t\t"<<setupHisto.fHisto_xLower<<endl;
                        cout<<"\t\t"<<setupHisto.fHisto_xUpper<<endl;
                    } //End Case: 3 or more numbers
                } //End Case: at least 2 numbers
                else{ //Case: Not enough numbers
                    if (vec_strList.size() == 1) { //Case: only 1 number entered
                        setupHisto.fHisto_xUpper = Timing::stofSafe(pair_strParam.first, vec_strList[0]);
                    } //End Case: only 1 number entered
                    
                    //Reset to ensure they are both correctly lower & upper values
                    setupHisto.fHisto_xLower = std::min(setupHisto.fHisto_xLower, setupHisto.fHisto_xUpper);
                    setupHisto.fHisto_xUpper = std::max(setupHisto.fHisto_xLower, setupHisto.fHisto_xUpper);
                    
                    //Output to User
                    cout<<"Timing::TimingRunAnalyzer::setAnalysisConfig() - Sorry I was expecting a comma separated list of 2 numbers for: " << pair_strParam.first << endl;
                    cout<<"\tRight Now I have set:\n";
                    cout<<"\t\tLower Histo Value = "<<setupHisto.fHisto_xLower<<endl;
                    cout<<"\t\tUpper Histo Value"<<setupHisto.fHisto_xUpper<<endl;
                } //End Case: Not enough numbers
            } //End Case: Assign Histo Bin Range
            else if( 0 == pair_strParam.first.compare("HISTO_NAME") ) {
                setupHisto.strHisto_Name = pair_strParam.second;
            }
            else if( 0 == pair_strParam.first.compare("HISTO_NUMBINS") ){
                setupHisto.iHisto_nBins = Timing::stoiSafe(pair_strParam.first, pair_strParam.second);
            }
            else if( 0 == pair_strParam.first.compare("HISTO_XTITLE") ){
                setupHisto.strHisto_Title_X = pair_strParam.second;
            }
            else if( 0 == pair_strParam.first.compare("HISTO_YTITLE") ){
                setupHisto.strHisto_Title_Y = pair_strParam.second;
            }
            else if( 0 == pair_strParam.first.compare("ISTRIGGER") ){
                setupHisto.bIsTrig = convert2bool(pair_strParam.second, bExitSuccess);
            }
            else if( 0 == pair_strParam.first.compare("PERFORM_FIT") ){
                setupHisto.bFit = convert2bool(pair_strParam.second, bExitSuccess);
            }
            else if( 0 == pair_strParam.first.compare("TDC_CHAN") ) {
                setupHisto.iTDC_Chan = stoiSafe(pair_strParam.first, pair_strParam.second);
            }
            else{ //Case: Parameter not recognized
                cout<<"Timing::TimingRunAnalyzer::setHistoSetuo() - Unrecognized field!!!\n";
                cout<<"\tField Name = " << pair_strParam.first << "; Value = " << pair_strParam.second << endl;
                cout<<"\tParameter not set!!! Please cross-check config file: " << strInputFile << endl;
                cout<<"\tMaybe you made a type?\n";
            } //End Case: Parameter not recognized
        } //End Case: Parameter Fetched Successfully
        else{ //Case: Parameter was NOT fetched Successfully
            cout<<"Timing::TimingRunAnalyzer::setHistoSetuo() - Sorry I didn't parse parameter correctly\n";
            cout<<"\tWorking on file: " << strInputFile << endl;
            cout<<"\tCurrent line: " << strLine << endl;
            cout<<"\tPlease cross-check! Skipping!!!\n";
        } //End Case: Parameter was NOT fetched Successfully
    } //End Loop through Section
    
    return;
} //End Timing::TimingRunAnalyzer::setHistoSetup()

//Set Detector Data after all events of a given run have been analyzed
//  ->timingResults struct containing detector & fit data
//  ->hInput Detector Histogram
void Timing::TimingRunAnalyzer::setPerformanceData(TDCAnalysisResults &inputTimingResults, TH1F &hInput){
    //Variable Declaration
    /*TSpectrum *timingSpec = new TSpectrum();
    
    //Get Number of peaks identified
    //Determine the number of Peaks in the Histogram (this may require additional user inputs...we'll see)
    //inputTimingResults.iTDC_Histo_nPks = timingSpec->Search( &hInput,3,"goff",0.001);
    inputTimingResults.iTDC_Histo_nPks = timingSpec->Search( &hInput,2,"goff",0.05);
    
    //If there is one peak, get it's position and center histogram on said peak
    //The record the mean and RMS of the histogram
    if ( 1 == inputTimingResults.iTDC_Histo_nPks ) { //Case: Peak Found
        Double_t *dHistoPeakPos_X = timingSpec->GetPositionX();
        
        hInput.GetXaxis()->SetRangeUser(dHistoPeakPos_X[0] - 5. * hInput.GetRMS(), dHistoPeakPos_X[0] + 5. * hInput.GetRMS() );
        
        //Ideally dHistoPeakPos_X[0] == hInput.GetMean()
        //May need to implement a background subtraction method
        inputTimingResults.fTDC_Histo_Mean = hInput.GetMean();
        inputTimingResults.fTDC_Histo_RMS  = hInput.GetRMS();
    } //End Case: Peak Found
    else{ //Case: No Peak Found
        inputTimingResults.fTDC_Histo_Mean = -1;
        inputTimingResults.fTDC_Histo_RMS  = -1;
    }*/ //End Case: No Peak Found
    
    //For now don't use TSpectrum
    hInput.GetXaxis()->SetRangeUser(hInput.GetMean() - 5. * hInput.GetRMS(), hInput.GetMean() + 5. * hInput.GetRMS() );
    
    //Ideally dHistoPeakPos_X[0] == hInput.GetMean()
    //May need to implement a background subtraction method
    inputTimingResults.fTDC_Histo_Mean = hInput.GetMean();
    inputTimingResults.fTDC_Histo_RMS  = hInput.GetRMS();
    
    //Set the Histogram
    //inputTimingResults.hTDC_Histo = &hInput;
    inputTimingResults.hTDC_Histo = std::make_shared<TH1F>(hInput);
    
    //Set the directory to be the global directory
    inputTimingResults.hTDC_Histo->SetDirectory(gROOT);
    
    return;
} //End Timing::TimingRunAnalyzer::setPerformanceData() - Histogram Version

 //Set Detector Data after all events of a given run have been analyzed
 //  ->timingResults struct containing detector & fit data
 //  ->funcInput fit of detector histogram
void Timing::TimingRunAnalyzer::setPerformanceData(TDCAnalysisResults &inputTimingResults, TF1 &funcInput, HistoSetup &setupHisto){
    //Set the meaning of the function parameters
    inputTimingResults.vec_strParamName = setupHisto.vec_strFit_ParamMeaning;
    
    //Set the normalized Chi2 value
    inputTimingResults.fTDC_Fit_Chi2= funcInput.GetChisquare();
    inputTimingResults.fTDC_Fit_NDF  = funcInput.GetNDF();
    
    //Set the Parameters & Their Error
    for (int i = 0; i < setupHisto.vec_strFit_ParamMeaning.size(); ++i) { //Loop Over Function Parameters
        inputTimingResults.map_fTDC_Fit_Param[setupHisto.vec_strFit_ParamMeaning[i]] = funcInput.GetParameter(i);
        inputTimingResults.map_fTDC_Fit_ParamErr[setupHisto.vec_strFit_ParamMeaning[i]] = funcInput.GetParError(i);
    } //End Loop Over Function Parameters
    
    //Set the Function
    //inputTimingResults.func_TDC_Fit = &funcInput;
    inputTimingResults.func_TDC_Fit = std::make_shared<TF1>(funcInput);
    
    return;
} //End Timing::TimingRunAnalyzer::setPerformanceData() - Histogram Version

