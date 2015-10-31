//
//  TRunParameters.cpp
//  
//
//  Created by Brian L Dorney on 18/03/15.
//
//

#include "TRunParameters.h"

//Constructor
Timing::TRunParameters::TRunParameters(){

} //End Constructor

//Set Run; use 1st character of input to convert to ASCII Key Code then assign proper PDG ID
//NOTE: This method fails if Proton and Pion are ever used in the same run
void Timing::TRunParameters::setBeam(std::string strRun_Name, std::string input){
    //char firstChar = (input.substr(0,1)).c_str();
    char firstChar;
    
    if (input.length() > 0) {
        firstChar = input[0];
    }
    else{
        run.iBeam_Type = 0;
        
        std::cout<<"Timing::TRunParameters::setBeam() - Expected input string::length() > 1\n";
        std::cout<<"Timing::TRunParameters::setBeam() - Given: '" << input << "' with length " << input.length() << std::endl;
        std::cout<<"Timing::TRunParameters::setBeam() - Please Cross-Check Input" << std::endl;
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
            std::cout<<"Timing::TRunParameters::setBeam() - Unrecognized Input!!!\n";
            std::cout<<"Timing::TRunParameters::setBeam() - Was given " << input << std::endl;
            std::cout<<"Timing::TRunParameters::setBeam() - Expected 1st Letter to be from set {'E','P','M'}\n";
            break;
    } //End Switch
    
    return;
} //End Timing::TRunParameters::setBeam()

//Set Trig Mode; use 1st character of input to convert to ASCII Key Code then assign 1 or 0 based on input
void Timing::TRunParameters::setTrigMode(std::string strRun_Name, std::string input){
    //char firstChar = (input.substr(0,1)).c_str();
    
    if ( (input.substr(0,1)).compare("A") == 0 ) {
        run.iTrig_Mode = 0;
    }
    else if ( (input.substr(0,1)).compare("S") == 0 ) {
        run.iTrig_Mode = 1;
    }
    else {
        run.iTrig_Mode = -1;
        std::cout<<"Timing::TRunParameters::setTrigMode() - Unrecognized Input!!!\n";
        std::cout<<"Timing::TRunParameters::setTrigMode() - Was given " << input << std::endl;
        std::cout<<"Timing::TRunParameters::setTrigMode() - Expected 1st Letter to be 'A' or 'S'\n";
    }
    
    return;
} //End Timing::TRunParameters::setTrigMode()

//Set the Detector Current
void Timing::TRunParameters::setDetCurrent(std::string strDet_Name, float fInput){
    //Set Detector 1
    if (run.map_det[strDet_Name].bDet_Gain_IndepVar_Imon) {
        run.map_det[strDet_Name].fDet_Imon = input;
        run.map_det[strDet_Name].fDet_VDrift = input * 1e-6 * (1.125e6 + 563e3 + 438e3 + 550e3 + 875e3 + 525e3 + 625e3);
    }
    else{
        run.map_det[strDet_Name].fDet_Imon = input;
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
void Timing::TRunParameters::setDetGain(std::string strDet_Name, float fDepVar){
    //Detector 1
    run.map_det[strDet_Name].fDet_Gain  = exp(run.map_det[strDet_Name].fDet_Gain_Const + run.map_det[strDet_Name].fDet_Gain_Slope * fDepVar );
    run.map_det[strDet_Name].fDet_Gain_Err   = sqrt( exp(2. * run.map_det[strDet_Name].fDet_Gain_Const + run.map_det[strDet_Name].fDet_Gain_Slope * fDepVar) * ( pow(run.map_det[strDet_Name].fDet_Gain_Const_Err, 2) + pow(fDepVar * run.map_det[strDet_Name].fDet_Gain_Slope_Err, 2) ) );
    
    return;
} //End Case: Timing::TRunParameters::calcGain()

//Set boolean type parameters
void Timing::TRunParameters::setParameter(std::string strDetOrRunName, bool bInput, int iMthdIdx){
    switch (iMthdIdx) {
            //Run Methods                               //Expected Input Data Type
            
            //Detector Methods
        case 114: setDetGainIndepVarIsCurrent(strDetOrRunName, bInput); break;   //boolean
            
            //VFAT Methods
            
            //TDC Methods
            
            //methodIdx Not Recognized
        default:
            std::cout<<"=================================================\n";
            std::cout<<"Timing::TRunParameters::setParameter() - methodIdx = " << methodIdx << " for STRING data type NOT recognized\n";
            std::cout<<"Timing::TRunParameters::setParameter() - Performing No Action\n";
            std::cout<<"Timing::TRunParameters::setParameter() - Please Check Input Again\n";
            std::cout<<"=================================================\n";
            break;
    } //End switch on methodIdx
    
    return;
} //End Timing::TRunParameters::setParameter()

//Set int type parameters
void Timing::TRunParameters::setParameter(std::string strDetOrRunName, int iInput, int iMthdIdx){
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
        case 301: setTDCChanDet(strDetOrRunName, iInput)                           //int
            
            //methodIdx Not Recognized
        default:
            std::cout<<"=================================================\n";
            std::cout<<"Timing::TRunParameters::setParameter() - methodIdx = " << iMthdIdx << " for INT data type NOT recognized\n";
            std::cout<<"Timing::TRunParameters::setParameter() - Performing No Action\n";
            std::cout<<"Timing::TRunParameters::setParameter() - Please Check Input Again\n";
            std::cout<<"=================================================\n";
            break;
    } //End switch on methodIdx
    
    return;
} //End Timing::TRunParameters::setParameter()

//Set float type parameters
void Timing::TRunParameters::setParameter(std::string strDetOrRunName, float fInput, int iMthdIdx){
    switch (iMthdIdx) {
        //Run Methods                               //Expected Input Data Type
        case 4: setTrigDelay(strDetOrRunName, fInput); break;                    //float
            
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
            std::cout<<"Timing::TRunParameters::setParameter() - methodIdx = " << iMthdIdx << " for FLOAT data type NOT recognized\n";
            std::cout<<"Timing::TRunParameters::setParameter() - Performing No Action\n";
            std::cout<<"Timing::TRunParameters::setParameter() - Please Check Input Again\n";
            std::cout<<"=================================================\n";
            break;
    } //End switch on methodIdx
    
    return;
} //End Timing::TRunParameters::setParameter()

//Set string type parameters
void Timing::TRunParameters::setParameter(std::string strDetOrRunName, std::string strInput, int iMthdIdx){
    switch (iMthdIdx) {
            //Run Methods                               //Expected Input Data Type
        case 2: setBeam(strDetOrRunName, strInput); break;                 //string
        case 3: setTrigMode(strDetOrRunName, strInput); break;             //string
            
        case 6: setTDCFitParamNameOR(strDetOrRunName, strInput)            //string
        case 7: setTDCFitParamNameAND(strDetOrRunName, strInput)           //string
            
            
            //Detector Methods
            
            //VFAT Methods
            
            //TDC Methods
        case 302: setTDCFitParamName(strDetOrRunName, strInput)            //string
            
            //methodIdx Not Recognized
        default:
            std::cout<<"=================================================\n";
            std::cout<<"Timing::TRunParameters::setParameter() - methodIdx = " << iMthdIdx << " for STRING data type NOT recognized\n";
            std::cout<<"Timing::TRunParameters::setParameter() - Performing No Action\n";
            std::cout<<"Timing::TRunParameters::setParameter() - Please Check Input Again\n";
            std::cout<<"=================================================\n";
            break;
    } //End switch on methodIdx
    
    return;
} //End Timing::TRunParameters::setParameter()