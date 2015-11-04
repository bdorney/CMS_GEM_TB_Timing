//
//  TRunParameters.cpp
//  
//
//  Created by Brian L Dorney on 18/03/15.
//
//

#include "TRunParameters.h"
#include "TimingUtilityFunctions.h"

using namespace Timing;

using std::endl;

//Constructor
TRunParameters::TRunParameters(){

} //End Constructor

//Set Run; use 1st character of input to convert to ASCII Key Code then assign proper PDG ID
//NOTE: This method fails if Proton and Pion are ever used in the same run
void TRunParameters::setBeam(std::string strRun_Name, std::string input){
    //char firstChar = (input.substr(0,1)).c_str();
    char firstChar;
    
    if (input.length() > 0) {
        firstChar = input[0];
    }
    else{
        run.iBeam_Type = 0;
        
        std::cout<<"TRunParameters::setBeam() - Expected input string::length() > 1\n";
        std::cout<<"TRunParameters::setBeam() - Given: '" << input << "' with length " << input.length() << std::endl;
        std::cout<<"TRunParameters::setBeam() - Please Cross-Check Input" << std::endl;
    }
    
    //Maybe instead of switch statement we should adopt what is used in setTrigMode()???
    
    switch ( (int) firstChar ) {
        case 69:    //Electron, |pdg id| = 11
            run.iBeam_Type = 11;
            break;
        case 77:    //Muon, |pdg id| = 13
            run.iBeam_Type = 13;
            break;
        case 80:    //Pion, |pdg id| = 211 (assuming charged pion)
            run.iBeam_Type = 211;
            break;
        default:    //Unrecognized
            run.iBeam_Type = 0;
            std::cout<<"TRunParameters::setBeam() - Unrecognized Input!!!\n";
            std::cout<<"TRunParameters::setBeam() - Was given " << input << std::endl;
            std::cout<<"TRunParameters::setBeam() - Expected 1st Letter to be from set {'E','P','M'}\n";
            break;
    } //End Switch
    
    return;
} //End TRunParameters::setBeam()

//Set Trig Mode; use 1st character of input to convert to ASCII Key Code then assign 1 or 0 based on input
void TRunParameters::setTrigMode(std::string strRun_Name, std::string input){
    //char firstChar = (input.substr(0,1)).c_str();
    
    if ( (input.substr(0,1)).compare("A") == 0 ) {
        run.iTrig_Mode = 0;
    }
    else if ( (input.substr(0,1)).compare("S") == 0 ) {
        run.iTrig_Mode = 1;
    }
    else {
        run.iTrig_Mode = -1;
        std::cout<<"TRunParameters::setTrigMode() - Unrecognized Input!!!\n";
        std::cout<<"TRunParameters::setTrigMode() - Was given " << input << std::endl;
        std::cout<<"TRunParameters::setTrigMode() - Expected 1st Letter to be 'A' or 'S'\n";
    }
    
    return;
} //End TRunParameters::setTrigMode()

//Set the Detector Current
void TRunParameters::setDetCurrent(std::string strDet_Name, float fInput){
    //Set Detector 1
    if (run.map_det[strDet_Name].bDet_Gain_IndepVar_Imon) {
        run.map_det[strDet_Name].fDet_Imon = fInput;
        run.map_det[strDet_Name].fDet_VDrift = fInput * 1e-6 * (1.125e6 + 563e3 + 438e3 + 550e3 + 875e3 + 525e3 + 625e3);
    }
    else{
        run.map_det[strDet_Name].fDet_Imon = fInput;
    }
    
    return;
};

//Set the detector gain
//Determing the gain of the detector; assumes the following parameters ahve already been set:
//  fDet_Gain_Const;     //Gain Curve - Amplitude
//  fDet_Gain_Const_Err; //Gain Curve - Amplitude, Error
//  fDet_Gain_Slope;     //Gain Curve - Exponential Slope
//  fDet_Gain_Slope_Err; //Gain Curve - Exponential Slope, Error
//The input fDepVar is the physical quantity which parameterizes the gain (e.g. VDrift, Imon, etc...)
void TRunParameters::setDetGain(std::string strDet_Name, float fDepVar){
    //Detector 1
    run.map_det[strDet_Name].fDet_Gain  = exp(run.map_det[strDet_Name].fDet_Gain_Const + run.map_det[strDet_Name].fDet_Gain_Slope * fDepVar );
    run.map_det[strDet_Name].fDet_Gain_Err   = sqrt( exp(2. * run.map_det[strDet_Name].fDet_Gain_Const + run.map_det[strDet_Name].fDet_Gain_Slope * fDepVar) * ( pow(run.map_det[strDet_Name].fDet_Gain_Const_Err, 2) + pow(fDepVar * run.map_det[strDet_Name].fDet_Gain_Slope_Err, 2) ) );
    
    return;
} //End Case: TRunParameters::calcGain()

//Determine if a detector should use the multichannel HV perscription or the divider perscription
//      Multichannel HV: strMultiChan = {t, true, 1, Multi}     (case insensitive)
//      Ceramic Divider: strMultiChan = {f, false, 0, Divider}  (case insensitive)
void TRunParameters::setDetMultiChanHVCase(std::string strDet_Name, std::string strMultiChan){
    //Variable Declaration
    bool bExitSuccess = false;
    bool bMultiChanHV = Timing::convert2bool(strMultiChan, bExitSuccess);
    
    //Accept sets {t, true, T, TRUE, 1} or {f, false, F, FALSE, 0}
    if (bExitSuccess) { //Case: identified by logical
        run.map_det[strDet_Name].bDet_HV_MultiChan = bMultiChanHV;
        return;
    } //End Case: identified by logical
    else{ //Case: identified by keyword {Divider, Multi}
        //Transform to upper case
        std::transform(strMultiChan.begin(), strMultiChan.end(), strMultiChan.begin(), toupper);
        
        if (0 == strMultiChan.compare("MULTI") ) { //Case: Multi Channel HV
            run.map_det[strDet_Name].bDet_HV_MultiChan = true;
        } //End Case: Multi Channel HV
        else if ( 0 == strMultiChan.compare("DIVIDER") ){ //Case: Ceramic Divider
            run.map_det[strDet_Name].bDet_HV_MultiChan = false;
        } //End Case: Ceramic Divider
        else{ //Case: Input Not Understood
            std::cout<<"TRunParameters::setDetMultiChanHVCase() - Input Not Recognized\n";
            std::cout<<"\tReceived strMultiChan = " << strMultiChan << endl;
            std::cout<<"\tExpect the set {t, true, 1, f, false, 0, Multi, Divider}\n";
            std::cout<<"\tSetting run.map_det["<<strDet_Name<<"].bDet_HV_MultiChan to false\n";
            std::cout<<"\tPlease cross-check input Tree Setup File\n";
            
            run.map_det[strDet_Name].bDet_HV_MultiChan = false;
        } //End Case: Input Not Understood
    } //End Case: identified by keyword {Divider, Multi}
    
    return;
} //End TRunParameters::setDetMultiChanHVCase

//Set a given parameter from a TTree
void TRunParameters::setDetParameterFromTree(std::string strDet_Name, std::string strBranchName, float &fInput){
    //Cross check on input parameters
    if (run.strRunName.length() == 0 || run.strTreeName_RunParam_DUT.length() == 0) {
        std::cout<<"TRunParameters::setDetParameterFromTree() - Input Parameters Not Set!!!\n";
        std::cout<<"\t Run Name = " << run.strRunName << std::endl;
        std::cout<<"\t TTree Name = " << run.strTreeName_RunParam_DUT << std::endl;
        std::cout<<"\t Please cross-check input Tree Setup File\n";
        std::cout<<"\t Parameters may not have been set!\n";
        
        return;
    }
    
    //Variable Declaration
    float fParam = -1;   //Hack for ROOT, e.g. can't have float &fInput then pass &fInput below (evaluates to &&fInput which is nonsense?)
    
    TFile *file_ROOT_Run = new TFile(run.strRunName.c_str(),"READ","",1);
    
    TTree *tree_ParamDUT = (TTree*) file_ROOT_Run->Get( run.strTreeName_RunParam_DUT.c_str() );
    
    if (tree_ParamDUT->FindBranch(strBranchName.c_str() ) == nullptr) {
        std::cout<<"TRunParameters::setDetParameterFromTree() - Input Branch Does Not Exist!!!\n";
        std::cout<<"\t Branch Name = " << strBranchName.c_str() << std::endl;
        std::cout<<"\t Please cross-check input Tree Setup File\n";
        std::cout<<"\t Parameters may not have been set!\n";
        
        file_ROOT_Run->Close();
        
        delete tree_ParamDUT;
        
        return;
    }
    
    tree_ParamDUT->SetBranchAddress(strBranchName.c_str(), &fParam);
    
    //Get fParam
    if (tree_ParamDUT->GetEntries() > 0) {
        tree_ParamDUT->GetEntry(0);
    }
    
    //Now set fInput as fParam, fInput passed by reference so original is now also set!!!
    fInput = fParam;
    
    file_ROOT_Run->Close();
    
    delete tree_ParamDUT;
    delete file_ROOT_Run;
    
    return;
} //End setDetParameterFromTree

void TRunParameters::setTDCFit(std::string strDet_Name, TF1 *func_Input, std::vector<std::string> vec_strParamName){
    //run.map_det[strDet_Name].vec_fTDC_Fit_Param.clear();
    //run.map_det[strDet_Name].vec_fTDC_Fit_ParamErr.clear();
    
    run.map_det[strDet_Name].timingResults.map_fTDC_Fit_Param.clear();
    run.map_det[strDet_Name].timingResults.map_fTDC_Fit_ParamErr.clear();
    //run.map_det[strDet_Name].vec_strParamName.clear();
    
    run.map_det[strDet_Name].timingResults.func_TDC_Fit = func_Input;
    
    //for (int i=0; i < func_Input->GetNpar(); ++i) {
    //run.map_det[strDet_Name].vec_fTDC_Fit_Param.push_back( func_Input->GetParameter(i) );
    //run.map_det[strDet_Name].vec_fTDC_Fit_ParamErr.push_back( func_Input->GetParError(i) );
    //}
    
    for (int i = 0; i < vec_strParamName.size(); ++i) {
        run.map_det[strDet_Name].timingResults.map_fTDC_Fit_Param[vec_strParamName[i]] = func_Input->GetParameter(i);
        run.map_det[strDet_Name].timingResults.map_fTDC_Fit_ParamErr[vec_strParamName[i]] = func_Input->GetParError(i);
    }
    
    run.map_det[strDet_Name].timingResults.fTDC_Fit_Chi2  = func_Input->GetChisquare();
    run.map_det[strDet_Name].timingResults.fTDC_Fit_NDF   = func_Input->GetNDF();
    
    return;
};

virtual void setTDCFitOR(TF1 *func_Input, std::vector<std::string> vec_strParamName){
    run.timingResultsOR.map_fTDC_Fit_Param.clear();
    run.timingResultsOR.map_fTDC_Fit_ParamErr.clear();
    
    run.timingResultsOR.func_TDC_Fit = func_Input;
    
    for (int i = 0; i < vec_strParamName.size(); ++i) {
        run.timingResultsOR.map_fTDC_Fit_Param[vec_strParamName[i]] = func_Input->GetParameter(i);
        run.timingResultsOR.map_fTDC_Fit_ParamErr[vec_strParamName[i]] = func_Input->GetParError(i);
    }
    
    run.timingResultsOR.fTDC_Fit_Chi2  = func_Input->GetChisquare();
    run.timingResultsOR.fTDC_Fit_NDF   = func_Input->GetNDF();
    
    return;
};


void TRunParameters::setTDCFitAND(TF1 *func_Input, std::vector<std::string> vec_strParamName){
    run.timingResultsAND.map_fTDC_Fit_Param.clear();
    run.timingResultsAND.map_fTDC_Fit_ParamErr.clear();
    
    run.timingResultsAND.func_TDC_Fit = func_Input;
    
    for (int i = 0; i < vec_strParamName.size(); ++i) {
        run.timingResultsAND.map_fTDC_Fit_Param[vec_strParamName[i]] = func_Input->GetParameter(i);
        run.timingResultsAND.map_fTDC_Fit_ParamErr[vec_strParamName[i]] = func_Input->GetParError(i);
    }
    
    run.timingResultsAND.fTDC_Fit_Chi2  = func_Input->GetChisquare();
    run.timingResultsAND.fTDC_Fit_NDF   = func_Input->GetNDF();
    
    return;
};

//Set boolean type parameters
void TRunParameters::setParameter(std::string strDetOrRunName, bool bInput, int iMthdIdx){
    switch (iMthdIdx) {
            //Run Methods                               //Expected Input Data Type
            
            //Detector Methods
        case 114: setDetGainIndepVarIsCurrent(strDetOrRunName, bInput); break;   //boolean
            
            //VFAT Methods
            
            //TDC Methods
            
            //methodIdx Not Recognized
        default:
            std::cout<<"=================================================\n";
            std::cout<<"TRunParameters::setParameter() - methodIdx = " << iMthdIdx << " for STRING data type NOT recognized\n";
            std::cout<<"TRunParameters::setParameter() - Performing No Action\n";
            std::cout<<"TRunParameters::setParameter() - Please Check Input Again\n";
            std::cout<<"=================================================\n";
            break;
    } //End switch on methodIdx
    
    return;
} //End TRunParameters::setParameter()

//Set int type parameters
void TRunParameters::setParameter(std::string strDetOrRunName, int iInput, int iMthdIdx){
    switch (iMthdIdx) {
            //Run Methods                                           //Expected Input Data Type
        case 1: setRunNumber(strDetOrRunName, iInput); break;                      //int
        case 5: setTDCChanTrig(strDetOrRunName, iInput); break;                    //int
            
            //Detector Methods
        case 101: setDetPos(strDetOrRunName, iInput); break;                       //int
        case 102: setDetEta(strDetOrRunName, iInput); break;                       //int
        case 103: setDetPhi(strDetOrRunName, iInput); break;                       //int
            
            //VFAT Methods
        case 202: setVFATPos(strDetOrRunName, iInput); break;                      //int
        case 204: setVFATIPreAmpIn(strDetOrRunName, iInput); break;                //int
        case 205: setVFATIPreAmpFeed(strDetOrRunName, iInput); break;              //int
        case 206: setVFATIPreAmpOut(strDetOrRunName, iInput); break;               //int
        case 207: setVFATIShaper(strDetOrRunName, iInput); break;                  //int
        case 208: setVFATIShaperFeed(strDetOrRunName, iInput); break;              //int
        case 209: setVFATIComp(strDetOrRunName, iInput); break;                    //int
        case 210: setVFATMSPL(strDetOrRunName, iInput); break;                     //int
        case 212: setVFATThresh(strDetOrRunName, iInput); break;                   //int
            
            //TDC Methods
        case 301: setTDCChanDet(strDetOrRunName, iInput); break;                   //int
            
            //methodIdx Not Recognized
        default:
            std::cout<<"=================================================\n";
            std::cout<<"TRunParameters::setParameter() - methodIdx = " << iMthdIdx << " for INT data type NOT recognized\n";
            std::cout<<"TRunParameters::setParameter() - Performing No Action\n";
            std::cout<<"TRunParameters::setParameter() - Please Check Input Again\n";
            std::cout<<"=================================================\n";
            break;
    } //End switch on methodIdx
    
    return;
} //End TRunParameters::setParameter()

//Set float type parameters
void TRunParameters::setParameter(std::string strDetOrRunName, float fInput, int iMthdIdx){
    switch (iMthdIdx) {
        //Run Methods                               //Expected Input Data Type
        case 4: setTrigDelay(strDetOrRunName, fInput); break;                    //float
        case 8: setSupermoduleHVSetPoint(strDetOrRunName, fInput); break;        //float
    
        //Detector Methods
        case 104: setDetCurrent(strDetOrRunName, fInput); break;                 //float
        case 105: setDetDriftV(strDetOrRunName, fInput); break;                  //float
        case 107: setDetGasFracAr(strDetOrRunName, fInput); break;               //float
        case 108: setDetGasFracCO2(strDetOrRunName, fInput); break;              //float
        case 109: setDetGasFracCF4(strDetOrRunName, fInput); break;              //float
        case 110: setDetGainConst(strDetOrRunName, fInput); break;               //float
        case 111: setDetGainConstErr(strDetOrRunName, fInput); break;            //float
        case 112: setDetGainSlope(strDetOrRunName, fInput); break;               //float
        case 113: setDetGainSlopeErr(strDetOrRunName, fInput); break;            //float
            
        case 115: setDetGEM1TopV(strDetOrRunName, fInput); break;                //float
        case 116: setDetGEM1BotV(strDetOrRunName, fInput); break;                //float
        case 117: setDetGEM2TopV(strDetOrRunName, fInput); break;                //float
        case 118: setDetGEM2BotV(strDetOrRunName, fInput); break;                //float
        case 119: setDetGEM3TopV(strDetOrRunName, fInput); break;                //float
        case 120: setDetGEM3BotV(strDetOrRunName, fInput); break;                //float
            
        //VFAT Methods
            
        //TDC Methods
            
            //methodIdx Not Recognized
        default:
            std::cout<<"=================================================\n";
            std::cout<<"TRunParameters::setParameter() - methodIdx = " << iMthdIdx << " for FLOAT data type NOT recognized\n";
            std::cout<<"TRunParameters::setParameter() - Performing No Action\n";
            std::cout<<"TRunParameters::setParameter() - Please Check Input Again\n";
            std::cout<<"=================================================\n";
            break;
    } //End switch on methodIdx
    
    return;
} //End TRunParameters::setParameter()

//Set string type parameters
void TRunParameters::setParameter(std::string strDetOrRunName, std::string strInput, int iMthdIdx){
    switch (iMthdIdx) {
            //Run Methods                               //Expected Input Data Type
        case 2: setBeam(strDetOrRunName, strInput); break;                 //string
        case 3: setTrigMode(strDetOrRunName, strInput); break;             //string
        case 6: setTDCFitParamNameOR(strDetOrRunName, strInput); break;            //string
        case 7: setTDCFitParamNameAND(strDetOrRunName, strInput); break;           //string
        case 9: setTreeNameRun(strDetOrRunName, strInput); break;                //string
        case 10:setTreeNameRunParamDUT(strDetOrRunName, strInput); break;        //string
            
            //Detector Methods
        case 121: setDetMultiChanHVCase(strDetOrRunName, strInput); break;     //string
        case 122: setDetGEM1TopV(strDetOrRunName, strInput); break;   //string
        case 123: setDetGEM1BotV(strDetOrRunName, strInput); break;   //string
        case 124: setDetGEM2TopV(strDetOrRunName, strInput); break;   //string
        case 125: setDetGEM2BotV(strDetOrRunName, strInput); break;   //string
        case 126: setDetGEM3TopV(strDetOrRunName, strInput); break;   //string
        case 127: setDetGEM3BotV(strDetOrRunName, strInput); break;   //string
            
            //VFAT Methods
            
            //TDC Methods
        case 302: setTDCFitParamName(strDetOrRunName, strInput); break;            //string
            
            //methodIdx Not Recognized
        default:
            std::cout<<"=================================================\n";
            std::cout<<"TRunParameters::setParameter() - methodIdx = " << iMthdIdx << " for STRING data type NOT recognized\n";
            std::cout<<"TRunParameters::setParameter() - Performing No Action\n";
            std::cout<<"TRunParameters::setParameter() - Please Check Input Again\n";
            std::cout<<"=================================================\n";
            break;
    } //End switch on methodIdx
    
    return;
} //End TRunParameters::setParameter()
