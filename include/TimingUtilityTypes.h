#ifndef ____TimingUtilityTypes__
#define ____TimingUtilityTypes__

//C++ Includes
#include <algorithm>
#include <iostream>
#include <map>
#include <stdio.h>
#include <sstream>
#include <string>

//My Includes
#include "TimingUtilityOperators.h"

//ROOT Includes
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TROOT.h"

/*
 * 
 *
 *
 *
 */

using namespace ROOT;

namespace Timing {
    //Common Data Types Goes Here
    struct TDCAnalysisData{
        float fTDC_Fit_Chi2;
        float fTDC_Fit_NDF;

        float fTDC_Eff;
        
        float fTDC_Histo_Mean;
        float fTDC_Histo_RMS;
        float iTDC_Histo_nPks;
        
        //float fTDC_Histo_PkInt_1stMax = -1;
        //float fTDC_Histo_PkInt_2ndMax = -1;
        //float fTDC_Histo_PkInt_3rdMax = -1;
        
        //float fTDC_Histo_PkPos_1stMax = -1;
        //float fTDC_Histo_PkPos_2ndMax = -1;
        //float fTDC_Histo_PkPos_3rdMax = -1;
        
        ROOT::TF1 *func_TDC_Fit;     //User Specified
        ROOT::TH1F *hTDC_Histo;      //Made from Detector.vec_iTDC_Data
        
        std::map<std::string, float> map_fTDC_Fit_Param; //Fit parameters
        std::map<std::string, float> map_fTDC_Fit_ParamErr; //Error on fit parameters

        std::vector<std::string> vec_strParamName;
        
        TDCAnalysisData(){
            fTDC_Histo_Mean = fTDC_Histo_RMS = fTDC_Eff = fTDC_Fit_Chi2 = fTDC_Fit_NDF = -1;
            iTDC_Histo_nPks = -1;
            
            func_TDC_Fit = nullptr;     //User Specified
            hTDC_Histo = nullptr;          //Made from vec_iTDC_Data

        }
    };
    
    struct Detector{
        //Detector Info
        //=============================
        bool bDet_Gain_IndepVar_Imon;
        bool bDet_HV_MultiChan;         //true -> 7 electrodes powered independently; false -> Divider
        
        float fDet_Imon;    //Current through the Divider
        float fDet_VDrift;  //Potential on the Drift
        float fDet_VG1_Top;    //Potential on GEM1 Top
        float fDet_VG1_Bot;    //Potential on GEM1 Bot
        float fDet_VG2_Top;    //"             "2 Top
        float fDet_VG2_Bot;
        float fDet_VG3_Top;
        float fDet_VG3_Bot;
        
        float fDet_Gain;            //Gain, given by f(x) = exp(Const + Slope * x)
        float fDet_Gain_Err;        //Gain Error
        float fDet_Gain_Const;      //Gain Curve - Amplitude
        float fDet_Gain_Const_Err;  //Gain Curve - Amplitude, Error
        float fDet_Gain_Slope;      //Gain Curve - Exponential Slope
        float fDet_Gain_Slope_Err;  //Gain Curve - Exponential Slope, Error
        
        float fDet_GasFrac_Ar;  //Argon Fraction in Gas
        float fDet_GasFrac_CO2; //CO2 "                 "
        float fDet_GasFrac_CF4; //CF4 "                 "
        
        int iDet_Pos;       //Position of Detector of Interest in Listing
        int iDet_Eta;       //iEta value of readout sector
        int iDet_Phi;       //iPhi value of readout sector
        
        //std::string strDet_Name; //Detector Name
        
        //std::string getTreeText(){
        //    return
        //}
        
        //VFAT Info
        //=============================
        //int iTURBO_ID = -1;      //TURBO ID, 0->Master, 1->Slave 1, 2->Slave 2, etc...
        int iVFAT_Pos;      //VFAT Position (on GE1/1)
        
        //std::string strVFAT_ID = ""; //VFAT ID, hex code
        
        int iVFAT_IPreAmpIn;
        int iVFAT_IPreAmpFeed;
        int iVFAT_IPreAmpOut;
        
        int iVFAT_IShaper;
        int iVFAT_IShaperFeed;
        
        int iVFAT_IComp;        //Current to Comparator
        
        int iVFAT_MSPL;         //Monostable Pulse Length
        
        //int iVFAT_Latency;      //VFAT Latency
        
        float fVFAT_Thresh;     //VFAT Threshold in fC
        
        //TDC
        //=============================
        TDCAnalysisData timingResults;
        
        int iTDC_Chan;
        
        std::vector<int> vec_iTDC_Data; //Event-by-Event Data
        
        Detector(){
            //Detector Info
            //=============================
            bDet_Gain_IndepVar_Imon = true;
            
            fDet_Imon=fDet_VDrift = -1;
            fDet_VG1_Top=fDet_VG1_Bot=fDet_VG2_Top=fDet_VG2_Bot=fDet_VG3_Top=fDet_VG3_Bot=-1;
            
            fDet_Gain = fDet_Gain_Err = -1;            //Gain, given by f(x) = exp(Const + Slope * x)
            fDet_Gain_Const = fDet_Gain_Const_Err = 0;
            fDet_Gain_Slope = fDet_Gain_Slope_Err = 0;
            
            fDet_GasFrac_Ar = fDet_GasFrac_CO2 = fDet_GasFrac_CF4 = -1;
            
            iDet_Pos = 0;
            iDet_Eta = iDet_Phi = -1;
            
            std::string strDet_Name = ""; //Detector Name
            
            //VFAT Info
            //=============================
            fVFAT_Thresh = -1;     //VFAT Threshold in fC
            
            iVFAT_Pos = -1;
            
            iVFAT_IPreAmpIn = iVFAT_IPreAmpFeed = iVFAT_IPreAmpOut = -1;
            
            iVFAT_IShaper = iVFAT_IShaperFeed = -1;
            
            iVFAT_IComp = -1;        //Current to Comparator
            
            iVFAT_MSPL = -1;         //Monostable Pulse Length
            
            //TDC
            //=============================
            iTDC_Chan = 0;
        }
    };
    
    //Run Info
    //Most values initialized to -1 (iBeam_Type to 0)
    //Pointers initialized to nullptr
    struct Run{
        //Run info
        //=============================
        bool bTakeHVFromRunParamTree;
        
        float fMaxDiffArrivalTime;  //Maximum difference in the mean of two distributions
        
        float fTrig_Delay;    //Trigger Delay in ns;
        
        float fSupermoduleHVSetpoint; //Setpoint of Supermodule
        
        int iRun;           //Run Number
        
        int iBeam_Type;     //11->Electron, 13->Muon, 211->Pion
        
        int iTrig_Mode;     //0->Async, 1->Sync
        
        std::string strRunName;
        std::string strTreeName_Run;            //Name of TTree that has the data
        std::string strTreeName_RunParam_DUT;   //Name of TTree that has the parameters of the DUT
        
        //Detectors
        //=============================
        std::map<std::string, Detector, Timing::map_cmp_str> map_det;
        
        //TDC Info
        //=============================
        //float fTDC_Fit_Chi2;
        //float fTDC_Fit_NDF;
        
        int iTDC_Chan_Trig;         //Channel of the Trigger in the TDC
        
        TDCAnalysisData timingResultsAND;
        TDCAnalysisData timingResultsOR;
        
        ROOT::TH1F *hTDC_DeltaT;
        
        ROOT::TH2F *hTDC_Correlation;
        
        Run(){
            //Run info
            //=============================
            bTakeHVFromRunParamTree = false;
            
            fTrig_Delay = fSupermoduleHVSetpoint = -1;    //Trigger Delay in ns;
            
            iRun = iTrig_Mode = -1;           //Run Number
            
            iBeam_Type = 0;     //11->Electron, 13->Muon, 211->Pion
            
            strTreeName_RunParam_DUT = "";
            strRunName = "";
            
            //TDC Info
            //=============================
            iTDC_Chan_Trig = 0;         //Channel of the Trigger in the TDC
            
        }
    };
    
} //End namespace

#endif
