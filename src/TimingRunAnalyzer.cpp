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
    
    strSecBegin_ANAL= "BEGIN_ANALYSIS_INFO";
    strSecBegin_AND = "BEGIN_AND_INFO";
    strSecBegin_DET = "BEGIN_DETECTOR_INFO";
    strSecBegin_OR  = "BEGIN_OR_INFO";
    
    strSecEnd_ANAL  = "END_ANALYSIS_INFO";
    strSecEnd_AND   = "END_AND_INFO";
    strSecEnd_DET   = "END_DETECTOR_INFO";
    strSecEnd_OR    = "END_OR_INFO";
} //End Constructor Timing::TimingRunAnalyzer::TimingRunAnalyzer()

//Analyze method, this is the central access point for the analysis
//Inherited class can over ride this method but it MUST exist
//The entire analysis needs to be executed through this method (but not necessarily in this method)
void Timing::TimingRunAnalyzer::analyzeRun(){
    //Check to see if a run was set
    if (!bRunSet) {
        cout<<"Timing::TimingRunAnalyzer::analyze() - Sorry, but you asked for analysis on a blank run!!!\n";
        cout<<"\tSkipping!!! Please cross-check your usage of the TimingRunAnalyzer class\n";
        
        return;
    }
    
    //Variable Declaration
    //map<string,int> map_iTDCData;
    map<string,float> map_fTDCData;
    map<string,TF1, map_cmp_str> map_fTDCFits;
    map<string,TH1F, map_cmp_str> map_fTDCHistos;
    
    TF1 func_TDC_Fit_AND;
    TF1 func_TDC_Fit_OR;
    
    TFile *file_ROOT_Run;
    
    //Setup the OR, AND, deltaT, & correlation histograms
    TH1F hTDC_AND = getHistogram( analysisSetup.setupAND );
    TH1F hTDC_DeltaT( ("hTDC_DeltaT_R" + getString(run.iRun) ).c_str(),"DeltaT:t_{Det1} - t_{Det2} #left( ns #right): A.U.",2 * analysisSetup.fTDCWinSize, -1. * analysisSetup.fTDCWinSize, analysisSetup.fTDCWinSize );
    TH2F hTDC_Correlation( ("hTDC_Corr_R" + getString(run.iRun) ).c_str(),"Correlation",analysisSetup.fTDCWinSize, 0., analysisSetup.fTDCWinSize,analysisSetup.fTDCWinSize, 0., analysisSetup.fTDCWinSize );
    TH1F hTDC_OR = getHistogram( analysisSetup.setupOR );
    
    TTree *tree_Run;
    
    vector<string> vec_strMapDetKeyVal; //List of detector names for random access to the map...
    
    //Set Errors on DeltaT & Correlation histograms (this is done automatically for those created with getHistogram)
    hTDC_DeltaT.Sumw2();
    hTDC_Correlation.Sumw2();
    
    //Open this run's root file
    //------------------------------------------------------
    file_ROOT_Run = new TFile(run.strRunName.c_str(),"READ","",1);
    
    //Check to see if data file opened successfully, if so load the tree
    //------------------------------------------------------
    if ( !file_ROOT_Run->IsOpen() || file_ROOT_Run->IsZombie() ) { //Case: failed to load ROOT file
        perror( ("Timing::TimingRunAnalyzer::analyze() - error while opening file: " + run.strRunName ).c_str() );
        printROOTFileStatus(file_ROOT_Run);
        std::cout << "Exiting!!!\n";
        
        return;
    } //End Case: failed to load ROOT file
    
    tree_Run = (TTree*) file_ROOT_Run->Get( run.strTreeName_Run.c_str() );
    
    if ( nullptr == tree_Run ) { //Case: failed to load TTree
        std::cout<<"Timing::TimingRunAnalyzer::analyze() - error while fetching: " << run.strTreeName_Run << endl;
        std::cout<<"\tTree Returns nullptr; tree_Run = " << tree_Run << endl;
        std::cout<<"Exiting!!!\n";
        
        return;
    } //End Case: failed to load TTree
    
    //Get Data for each detector
    //------------------------------------------------------
    //HACK Explanation
    //Initialize map_fTDCData for each detector
    //Then we will set the TBranch address for each detector (e.g. TDC Channel)
    //Then we will get the data in the most hacked way imaginable; YEAH ROOT
    
    //Initialize map_fTDCData for each detector
    for (auto iterDet = run.map_det.begin(); iterDet != run.map_det.end(); ++iterDet) { //Loop Through Detectors
        map_fTDCData[(*iterDet).first];
        
        //NOTE: Changed, we do this below because we use inputs from the histogram to setup the function
        //Form fit for this detector
        //((*iterDet).second).func_TDC_Fit = &getFunction( analysisSetup.map_DetSetup[(*iterDet).first] );
        //map_fTDCFits[(*iterDet).first] = getFunction( analysisSetup.map_DetSetup[(*iterDet).first] );
        
        //Form histogram for this detector
        //((*iterDet).second).hTDC_Histo = &getHistogram( analysisSetup.map_DetSetup[(*iterDet).first] );
        map_fTDCHistos[(*iterDet).first] = getHistogram( analysisSetup.map_DetSetup[(*iterDet).first] );
        
        //Append the Histo name with the run number
        //map_fTDCHistos[(*iterDet).first].SetName( ( analysisSetup.map_DetSetup[(*iterDet).first].strHisto_Name + "_" + getString(run.iRun ) ).c_str() );
        
        //Store the key values
        vec_strMapDetKeyVal.push_back((*iterDet).first);
    } //End Loop Through Detectors
    
    //Set TBranch Address for each detector (e.g. TDC Channel)
    //  NOTE: Yes it needs to be done in TWO separate loops; yes ROOT is stupid
    for (auto iterDet = run.map_det.begin(); iterDet != run.map_det.end(); ++iterDet) { //Loop Through Detectors
        //Set branch address
        tree_Run->SetBranchAddress( ("TDC_Ch" + getString( ((*iterDet).second).iTDC_Chan ) ).c_str(), &(map_fTDCData[(*iterDet).first]) );
    } //End Loop Through Detectors
    
    //Set Axis titles on Correlation histogram
    if ( vec_strMapDetKeyVal.size() > 1 ){
        hTDC_Correlation.SetXTitle( vec_strMapDetKeyVal[0].c_str() );
        hTDC_Correlation.SetYTitle( vec_strMapDetKeyVal[1].c_str() );
    }
    
    //Get Data event-by-event from individual channels and nonzero invert times if requested
    for (int i=0; i < tree_Run->GetEntries(); ++i) {
        tree_Run->GetEntry(i);
        
        //Get data event-by-event
        for (auto iterDet = run.map_det.begin(); iterDet != run.map_det.end(); ++iterDet) {
            //Correct for inverted times due to common_stop technique of TDC
            //  NOTE: this does not mean 0 is the trigger if this correction is made
            //        this moves the trigger time from t=0 to t=analysisSetup.fTDCWinSize
            if (analysisSetup.bInvertTime){ //Case: Invert non-zer times
                ((*iterDet).second).vec_fTDC_Data.push_back( getInvertedTime( map_fTDCData[(*iterDet).first] ) );
            } //End Case: Invert non-zero times
            else{ //Case: Use Raw Times
                ((*iterDet).second).vec_fTDC_Data.push_back( map_fTDCData[(*iterDet).first] );
            } //End Case: Use Raw Times
        } //End
    } //End Loop Through Tree
    
    //Correct for offset in mean arrival time?
    //Consider only the case were number of detectors is greater than 2 (obviously)
    if (analysisSetup.bMatchArrivalTime && run.map_det.size() > 1) { //Case: Correct for Offsets in Arrival Time
        map<string, Timing::Detector, Timing::map_cmp_str>::iterator detMapBegin = run.map_det.begin();
        
        float fOffset = 0.;
        
        for (auto iterDet = run.map_det.begin(); iterDet != run.map_det.end(); ++iterDet) { //Loop Over Detectors
            //Skip the first element, it is stored above (maybe there's a better way to do this)
            if (run.map_det.begin() == iterDet) continue;
            
            fOffset = deltaMean( ((*detMapBegin).second).vec_fTDC_Data, ((*iterDet).second).vec_fTDC_Data );
            
            if (fOffset > fTDCResolution) { //Case: offset greater than TDC Resolution
                //Won't work, fOffset is not a "const float"
                //transform(((*iterDet).second).vec_fTDC_Data.begin(), ((*iterDet).second).vec_fTDC_Data.end(), ((*iterDet).second).vec_fTDC_Data.begin(), std::bind2nd(std::plus<float>(), fOffset)  )
                std::for_each(((*iterDet).second).vec_fTDC_Data.begin(), ((*iterDet).second).vec_fTDC_Data.end(), Timing::addVal(fOffset) );
            } //End Case: offset greater than TDC Resolution
        } //End Loop Over Detectors
    } //End Case: Correct for Offsets in Arrival Time
    
    //Efficiency is a round 2 thing
    
    //Fill Histograms
    //------------------------------------------------------
    //Use the tree again to get the number of events without a hassle
    //  NOTE we are not looping through the tree data; but the vector data in the run.map_det[some_det].vec_fTDC_Data
    for (int i=0; i < tree_Run->GetEntries(); ++i) { //Loop through Events
        //Fill individual histograms
        //Here we are going to loop over the detectors in run.map_det
        //And we will access by index "i" the elements of run.map_det[some_det].vec_fTDC_Data
        //This works since by design run.map_det[some_det].vec_fTDC_Data.size() == tree_Run->GetEntries()
        //For each i we are going to fill the histograms, and reset the value of map_fTDCData
        
        for (auto iterDet = run.map_det.begin(); iterDet != run.map_det.end(); ++iterDet) { //Loop Over Detectors
            //Reset the value of the map_fTDCData
            map_fTDCData[(*iterDet).first] = ((*iterDet).second).vec_fTDC_Data[i];
            
            //Fill the Histogram
            if ( ((*iterDet).second).vec_fTDC_Data[i] > 0 ) {
                map_fTDCHistos[((*iterDet).first)].Fill( ((*iterDet).second).vec_fTDC_Data[i] );
            } //End Fill Histogram
        } //End Loop Over Detectors
        
        //Fill the OR histogram
        if (getMinForChannelOR(map_fTDCData) > 0){ //Case: Any Detector fired this event
            hTDC_OR.Fill( getMinForChannelOR(map_fTDCData) );
        } //End Case: Any Detector fired this event
        
        //Fill the AND, DeltaT, and Correlation histograms
        if (getMaxForChannelAND(map_fTDCData) > 0){
            hTDC_AND.Fill( getMaxForChannelAND(map_fTDCData) );
            
            //Right now only providing support for the 2 detector case
            //someone wants 3 they can write their own analyzer...
            //Need to use this hack so that deltaT and correlation are always the same
            if (map_fTDCData.size() == 2) { //Case: Two Detectors
                hTDC_DeltaT.Fill( map_fTDCData[vec_strMapDetKeyVal[0]] - map_fTDCData[vec_strMapDetKeyVal[1]] );
                hTDC_Correlation.Fill(map_fTDCData[vec_strMapDetKeyVal[0]], map_fTDCData[vec_strMapDetKeyVal[1]]);
            } //End Case: Two Detectors
        } //End Case: Both Detectors fired this event
    } //End Loop through Events
    
    //Record Detector Data
    //------------------------------------------------------
    //Event loop finished set performance data for single detectors
    for (auto iterDet = run.map_det.begin(); iterDet != run.map_det.end(); ++iterDet) { //Loop Over Detectors
        setPerformanceData( ((*iterDet).second).timingResults, map_fTDCHistos[(*iterDet).first] );
    } //End Loop Over Detectors
    
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
            map_fTDCFits[(*iterDet).first] = getFunction( analysisSetup.map_DetSetup[(*iterDet).first], map_fTDCHistos[(*iterDet).first] );
            
            //Fit histogram
            fitHistogram( analysisSetup.map_DetSetup[(*iterDet).first], map_fTDCHistos[(*iterDet).first], map_fTDCFits[(*iterDet).first] );
            
            //Store the performance data
            setPerformanceData( ((*iterDet).second).timingResults, map_fTDCFits[(*iterDet).first], analysisSetup.map_DetSetup[(*iterDet).first] );
        } //End Loop Through Detectors
        
        //Initialize AND & OR fits
        func_TDC_Fit_AND= getFunction( analysisSetup.setupAND, hTDC_AND );
        func_TDC_Fit_OR = getFunction( analysisSetup.setupOR, hTDC_OR );
        
        //Fit AND & OR
        fitHistogram(analysisSetup.setupAND, hTDC_AND, func_TDC_Fit_AND);
        fitHistogram(analysisSetup.setupOR, hTDC_OR, func_TDC_Fit_OR);
        
        //Store the performance data for the AND & OR
        setPerformanceData( run.timingResultsAND, func_TDC_Fit_AND, analysisSetup.setupAND );
        setPerformanceData( run.timingResultsOR, func_TDC_Fit_OR, analysisSetup.setupOR );
    } //End Case: Async Trigger Mode
    else if ( 1 == run.iTrig_Mode) { //Case: Sync Trigger Mode
        //Blank for now
        
        //From the old analysis:
        //Loop Over Found Peaks
        /*Double_t *dHistoPeakPos_X = timingSpec->GetPositionX();
        
        vector<pair<float,float> > vec_PksInfo; //pair.first -> Integral; pair.second -> X-Pos
        
        //NOTE: This could be done with a std::map<float,float> but I want random access "[]" for easy usage
        
        //Get All the Peaks
        cout<<"============================================" << endl;
        cout<<"treeProducerTDC::setRun() - Searching for Peaks\n";
        for (int i=0; i<timingSpec->GetNPeaks(); ++i) { //Loop Through Spec Peaks
            //vec_PksInfo.push_back(std::make_pair( getPeakIntegral(timingHisto, float (dHistoPeakPos_X[i]) ), float (dHistoPeakPos_X[i]) ) );
            vec_PksInfo.push_back(std::make_pair( getPeakIntegral(&timingHisto, float (dHistoPeakPos_X[i]) ), float (dHistoPeakPos_X[i]) ) );
            
            cout<<"treeProducerTDC::setRun() - Peak Found! Continuing Search!\n";
            
            //Map comparison by default is std::less which acts like "<" operator
            //Upon completion map will be ordered by increasing key value
            //map_Peaks[ float (dHistoPeakPos_X[i]) ] = getPeakIntegral(timingHisto, float (dHistoPeakPos_X[i]) );
        } //End Loop Through Spec Peaks
        cout<<"treeProducerTDC::setRun() - All Peaks Found!\n";
        cout<<"============================================" << endl;
        
        //Sort the peaks in Ascending Order
        std::sort(vec_PksInfo.begin(), vec_PksInfo.end() );
        
        //The Peaks are now sorted Lowest to Highest Integral
        //NOTE: if Peak integrals are same they are then sorted by peak position (default std::pair comparator)
        
        //Store the Peaks
        if ( vec_PksInfo.size() >= 3 ) {
            runLogger.setTDCHistoPksInt_1stMax( (vec_PksInfo[vec_PksInfo.size()-1]).first );
            runLogger.setTDCHistoPksInt_2ndMax( (vec_PksInfo[vec_PksInfo.size()-2]).first );
            runLogger.setTDCHistoPksInt_3rdMax( (vec_PksInfo[vec_PksInfo.size()-3]).first );
            
            runLogger.setTDCHistoPksPos_1stMax( (vec_PksInfo[vec_PksInfo.size()-1]).second );
            runLogger.setTDCHistoPksPos_2ndMax( (vec_PksInfo[vec_PksInfo.size()-2]).second );
            runLogger.setTDCHistoPksPos_3rdMax( (vec_PksInfo[vec_PksInfo.size()-3]).second );
        }
        else if ( vec_PksInfo.size() >= 2 ) {
            runLogger.setTDCHistoPksInt_1stMax( (vec_PksInfo[vec_PksInfo.size()-1]).first );
            runLogger.setTDCHistoPksInt_2ndMax( (vec_PksInfo[vec_PksInfo.size()-2]).first );
            
            runLogger.setTDCHistoPksPos_1stMax( (vec_PksInfo[vec_PksInfo.size()-1]).second );
            runLogger.setTDCHistoPksPos_2ndMax( (vec_PksInfo[vec_PksInfo.size()-2]).second );
        }
        else if ( vec_PksInfo.size() >= 1 ) {
            runLogger.setTDCHistoPksInt_1stMax( (vec_PksInfo[vec_PksInfo.size()-1]).first );
            
            runLogger.setTDCHistoPksPos_1stMax( (vec_PksInfo[vec_PksInfo.size()-1]).second );
        }*/

    } //End Case: Sync Trigger Mode
    
    //Store Objects/Parameters into the Run
    //We are done with the analysis now, store everything into the run
    //  NOTE: We do not work with the TObjects stored in the run above because passing the pointers "TH1F *" and "TF1 *" around has lead to problems in the past
    //------------------------------------------------------
    run.hTDC_Correlation= &hTDC_Correlation;//Set the run's histogram
    run.hTDC_DeltaT     = &hTDC_DeltaT;     //Set the run's histogram
    
    //Close the file and delete the tree
    //------------------------------------------------------
    file_ROOT_Run->Close();
    
    delete tree_Run;
    
    return;
} //End Timing::TimingRunAnalyzer::analyze()

//Fits an input histogram based on parameters given
void Timing::TimingRunAnalyzer::fitHistogram(HistoSetup &setupHisto, TH1F & hInput, TF1 &funcInput){
    
    if (setupHisto.bFit_AutoRanging) {
        hInput.Fit(funcInput, setupHisto.strFit_Option.c_str(), hInput.GetMean() - 4. * hInput.GetRMS(), hInput.GetMean() + 4. * hInput.GetRMS() );
    }
    else{
        hInput.Fit(funcInput, setupHisto.strFit_Option.c_str() );
    }
    
    return;
} //End Timing::TimingRunAnalyzer::fitHistogram

//Returns a TF1 (function) whose parameters are set by setupHisto
//Good candidate for TimingUtitlityFunctions.h?
TF1 Timing::TimingRunAnalyzer::getFunction(HistoSetup &setupHisto, TH1F & hInput){
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
            if ( 0 == setupHisto.vec_strFit_ParamIGuess[i].compare("AMPLITUDE") ||
                 0 == setupHisto.vec_strFit_ParamIGuess[i].compare("MEAN") ||
                 0 == setupHisto.vec_strFit_ParamIGuess[i].compare("SIGMA")  ) {
                
                if (0 == setupHisto.vec_strFit_ParamIGuess[i].compare("AMPLITUDE") ) {
                    ret_Func.SetParameter(i, hInput.GetBinContent( hInput.GetMaximumBin() ) );
                }
                else if (0 == setupHisto.vec_strFit_ParamIGuess[i].compare("MEAN") ) {
                    ret_Func.SetParameter(i, hInput.GetMean() );
                }
                else if (0 == setupHisto.vec_strFit_ParamIGuess[i].compare("SIGMA") ) {
                    ret_Func.SetParameter(i, hInput.GetRMS() );
                }
            } //End Case: automatic assignment from input histogram
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
TH1F Timing::TimingRunAnalyzer::getHistogram(HistoSetup &setupHisto){
    //Variable Declaration
    TH1F ret_Histo(setupHisto.strHisto_Name.c_str(), "", setupHisto.iHisto_nBins, setupHisto.fHisto_xLower, setupHisto.fHisto_xUpper );
    
    //Append the run number to the histogram name
    if( bRunSet ){
        ret_Histo.SetName( ( setupHisto.strHisto_Name + "_R" + getString(run.iRun) ).c_str() );
    }
    else{
        //In principle the run should always be set before this is called, but YOLO errors?
        ret_Histo.SetName( ( setupHisto.strHisto_Name + "_R0" ).c_str() );
    }
    
    ret_Histo.SetXTitle( setupHisto.strHisto_Title_X.c_str() );
    ret_Histo.SetYTitle( setupHisto.strHisto_Title_Y.c_str() );
    
    ret_Histo.Sumw2();
    
    return ret_Histo;
} //End getHistogram

//Loads all relevant analysis parameters from a text file
void Timing::TimingRunAnalyzer::setAnalysisConfig(string strInputFile){
    //Variable Declaration
    bool bExitSuccess = false;
    bool bDetSetup = false;
    //bool bInfoHeaderEnd = false;
    
    pair<string,string> pair_strParam; //<Field, Value>
    
    string strLine = "";
    string strThisDetName = ""; //name of current detector;
    
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
            analysisSetup.bCompute_AND = true;
            
            //Setup the Histogram struct for analysis
            setHistoSetup(strInputFile, fStream, analysisSetup.setupAND );
        } //End Case: AND SECTION
        else if ( 0 == strLine.compare(strSecBegin_DET) ){ //Case: DET SECTION
            //So if the user has done this section correctly the next noncommented line should the detector name
            bDetSetup = false;  //reset at the start of each new detector section
            
            while ( getlineNoSpaces(fStream, strLine) ) { //Loop through file to find "Detector_Name"
                //Does the user want to comment out this line?
                if ( 0 == strLine.compare(0,1,"#") ) continue;
                
                //Do we reach the end of the section? If so the user has configured the file incorrectly
                if ( 0 == strLine.compare(strSecEnd_DET) ) { //Section End Reached Prematurely
                    cout<<"Timing::TimingRunAnalyzer::setAnalysisConfig() - I have reached the END of a Detector Heading\n";
                    cout<<"\tBut Have NOT found the 'Detector_Name' field\n"<<endl;
                    cout<<"\tYou have configured this heading incorrectly, the 'Detector_Name' field is expected to be the FIRST' line of a Detector Heading\n";
                    cout<<"\tThis Detector has been skipped, please cross-check\n";
                    
                    //Exit the Loop to find "Detector_Name"
                    break;
                } //End Case: Section End Reached Prematurely
                
                pair_strParam = getParsedLine(strLine,bExitSuccess);
                
                if (bExitSuccess) { //Case: Parameter Fetched Successfully
                    transform(pair_strParam.first.begin(),pair_strParam.first.end(),pair_strParam.first.begin(),toupper);
                    
                    if ( 0 == pair_strParam.first.compare("DETECTOR_NAME") ) { //Case: Detector Name found!
                        //Store detector name locally
                        strThisDetName = pair_strParam.second;
                        
                        //Setup the Histogram struct for analysis
                        HistoSetup detSetup;
                        analysisSetup.map_DetSetup[pair_strParam.second] = detSetup;
                        
                        //Set the correct exit flag
                        bDetSetup = true;
                        
                        //Exit the Loop to find "Detector_Name"
                        break;
                    } //End Case: Detector Name found!
                    else{ //Case: Detector Name not found!
                        cout<<"Timing::TimingRunAnalyzer::setAnalysisConfig() - I am parsing a Detector Heading\n";
                        cout<<"\tHowever I expected the 'Detector_Name' field to be at the tope of the heading\n";
                        cout<<"\tThe current line I am parsing: " << strLine << endl;
                        cout<<"\tHas been skipped and may lead to undefined behavior" << endl;
                    } //End Case: Detector Name not found!
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
            } //Loop through file to find "Detector_Name"
            
            if (bDetSetup) {
                setHistoSetup(strInputFile, fStream, analysisSetup.map_DetSetup[strThisDetName] );
            }
        } //End Case: DET SECTION
        else if ( 0 == strLine.compare(strSecBegin_OR) ){ //Case: OR SECTION
            //Set the flag to compute the OR of all detectors
            analysisSetup.bCompute_OR = true;
            
            //Setup the Histogram struct for analysis
            setHistoSetup(strInputFile, fStream, analysisSetup.setupOR );
        } //End Case: OR SECTION
        else{ //Case: Unsorted Parameters
            pair_strParam = getParsedLine(strLine, bExitSuccess);
            
            if (bExitSuccess) { //Case: Parameter Fetched Successfully
                transform(pair_strParam.first.begin(),pair_strParam.first.end(),pair_strParam.first.begin(),toupper);
                
                if ( 0 == pair_strParam.first.compare("TDC_WINDOW_SIZE") ) {
                    analysisSetup.fTDCWinSize = stofSafe(pair_strParam.first, pair_strParam.second);
                }
                else if( 0 == pair_strParam.first.compare("INVERT_TIMING") ){
                    analysisSetup.bInvertTime = convert2bool(pair_strParam.second, bExitSuccess);
                }
                else if( 0 == pair_strParam.first.compare("MATCH_ARRIVAL_TIME") ){
                    analysisSetup.bMatchArrivalTime = convert2bool(pair_strParam.second, bExitSuccess);
                }
                else{ //Case: Parameter not recognized
                    cout<<"Timing::TimingRunAnalyzer::setAnalysisConfig() - Unrecognized field!!!\n";
                    cout<<"\tField Name = " << pair_strParam.first << "; Value = " << pair_strParam.second << endl;
                    cout<<"\tParameter not set!!! Please cross-check config file: " << strInputFile << endl;
                    cout<<"\tMaybe you made a type?\n";
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
        
        //Does the user want to comment out this line?
        if ( 0 == strLine.compare(0,1,"#") ) continue;
        
        //Has this section ended?
        if ( 0 == strLine.compare(strSecEnd_AND) || 0 == strLine.compare(strSecEnd_DET) || 0 == strLine.compare(strSecEnd_OR) ) break;
        
        //Get Parameter Pairing
        pair_strParam = Timing::getParsedLine(strLine, bExitSuccess);
        
        //transform field name to upper case
        transform(pair_strParam.first.begin(),pair_strParam.first.end(),pair_strParam.first.begin(),toupper);
        
        //Parse pair
        if (bExitSuccess) { //Case: Parameter Fetched Successfully
            if ( 0 == pair_strParam.first.compare("HISTO_NAME") ) {
                setupHisto.strHisto_Name = pair_strParam.second;
            }
            else if( 0 == pair_strParam.first.compare("HISTO_NUMBINS") ){
                setupHisto.iHisto_nBins = Timing::stoiSafe(pair_strParam.first, pair_strParam.second);
            }
            else if( 0 == pair_strParam.first.compare("HISTO_BINRANGE") ){
                //Get comma separated list
                vec_strList = Timing::getCommaSeparatedList(pair_strParam.second);
                
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
            else if( 0 == pair_strParam.first.compare("HISTO_XTITLE") ){
                setupHisto.strHisto_Title_X = pair_strParam.second;
            }
            else if( 0 == pair_strParam.first.compare("HISTO_YTITLE") ){
                setupHisto.strHisto_Title_Y = pair_strParam.second;
            }
            else if( 0 == pair_strParam.first.compare("FIT_AUTORANGING") ){
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
            else if( 0 == pair_strParam.first.compare("FIT_PARAM_MAP") ){
                setupHisto.vec_strFit_ParamMeaning = getCommaSeparatedList(pair_strParam.second);
            }
            else if( 0 == pair_strParam.first.compare("FIT_PARAM_IGUESS") ){
                setupHisto.vec_strFit_ParamIGuess = getCommaSeparatedList(pair_strParam.second);
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
void Timing::TimingRunAnalyzer::setPerformanceData(TDCAnalysisData &inputTimingResults, TH1F &hInput){
    //Variable Declaration
    TSpectrum *timingSpec = new TSpectrum();
    
    //Get Number of peaks identified
    //Determine the number of Peaks in the Histogram (this may require additional user inputs...we'll see)
    inputTimingResults.iTDC_Histo_nPks = timingSpec->Search( &hInput,3,"goff",0.001);
    
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
    } //End Case: No Peak Found
    
    //Set the Histogram
    inputTimingResults.hTDC_Histo = &hInput;
    
    return;
} //End Timing::TimingRunAnalyzer::setPerformanceData() - Histogram Version

 //Set Detector Data after all events of a given run have been analyzed
 //  ->timingResults struct containing detector & fit data
 //  ->funcInput fit of detector histogram
void Timing::TimingRunAnalyzer::setPerformanceData(TDCAnalysisData &inputTimingResults, TF1 &funcInput, HistoSetup &setupHisto){
    
    //Set the normalized Chi2 value
    inputTimingResults.fTDC_Fit_Chi2= funcInput.GetChisquare();
    inputTimingResults.fTDC_Fit_NDF  = funcInput.GetNDF();
    
    //Set the Parameters & Their Error
    for (int i = 0; i < setupHisto.vec_strFit_ParamMeaning.size(); ++i) { //Loop Over Function Parameters
        inputTimingResults.map_fTDC_Fit_Param[setupHisto.vec_strFit_ParamMeaning[i]] = funcInput.GetParameter(i);
        inputTimingResults.map_fTDC_Fit_ParamErr[setupHisto.vec_strFit_ParamMeaning[i]] = funcInput.GetParError(i);
    } //End Loop Over Function Parameters
    
    //Set the Function
    inputTimingResults.func_TDC_Fit = &funcInput;
    
    return;
} //End Timing::TimingRunAnalyzer::setPerformanceData() - Histogram Version