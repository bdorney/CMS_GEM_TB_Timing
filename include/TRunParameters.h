//
//  TRunParameters.h
//  
//
//  Created by Brian L Dorney on 18/03/15.
//
//

#ifndef ____TRunParameters__
#define ____TRunParameters__

//C++ Includes
#include <cmath>
#include <iostream>
#include <map>
//#include <math>
#include <stdio.h>
#include <string>
#include <utility>
#include <vector>

//ROOT Includes
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TObject.h"

/*  METHOD INDEX GUIDE
 
 //Run Methods                                                              //Expected Input Data Type
 case 1: setRunNumber(std::string strRun_Name, int iInput); break;                      //int
 case 2: setBeam(std::string strRun_Name, std::string strInput); break;                 //string
 case 3: setTrigMode(std::string strRun_Name, std::string strInput); break;             //string
 case 4: setTrigDelay(std::string strRun_Name, float fInput); break;                    //float
 case 5: setTDCChanTrig(std::string strRun_Name, int iInput); break;                    //int
 case 6: setTDCFitParamNameOR(std::string strRun_Name, string strParamName)             //string
 case 7: setTDCFitParamNameAND(std::string strRun_Name, string strParamName)            //string
 
 //Detector Methods
 case 101: setDetPos(std::string strDet_Name, int iInput); break;                       //int
 case 102: setDetEta(std::string strDet_Name, int iInput); break;                       //int
 case 103: setDetPhi(std::string strDet_Name, int iInput); break;                       //int
 case 104: setDetCurrent(std::string strDet_Name, float fInput); break;                 //float
 case 105: setDetDriftV(std::string strDet_Name, float fInput); break;                  //float
 case 107: setDetGasFracAr(std::string strDet_Name, float fInput); break;               //float
 case 108: setDetGasFracCO2(std::string strDet_Name, float fInput); break;              //float
 case 109: setDetGasFracCF4(std::string strDet_Name, float fInput); break;              //float
 case 110: setDetGainConst(std::string strDet_Name, float fInput); break;               //float
 case 111: setDetGainConstErr(std::string strDet_Name, float fInput); break;            //float
 case 112: setDetGainSlope(std::string strDet_Name, float fInput); break;               //float
 case 113: setDetGainSlopeErr(std::string strDet_Name, float fInput); break;            //float
 case 114: setDetGainIndepVarIsCurrent(std::string strDet_Name, bool bInput); break;    //boolean
 case 115: setDetGEM1TopV(std::string strDet_Name, float fInput); break;                //float
 case 116: setDetGEM1BotV(std::string strDet_Name, float fInput); break;                //float
 case 117: setDetGEM2TopV(std::string strDet_Name, float fInput); break;                //float
 case 118: setDetGEM2BotV(std::string strDet_Name, float fInput); break;                //float
 case 119: setDetGEM3TopV(std::string strDet_Name, float fInput); break;                //float
 case 120: setDetGEM3BotV(std::string strDet_Name, float fInput); break;                //float
 
 //VFAT Methods
 case 202: setVFATPos(std::string strDet_Name, int iInput); break;                      //int
 case 204: setVFATIPreAmpIn(std::string strDet_Name, int iInput); break;                //int
 case 205: setVFATIPreAmpFeed(std::string strDet_Name, int iInput); break;              //int
 case 206: setVFATIPreAmpOut(std::string strDet_Name, int iInput); break;               //int
 case 207: setVFATIShaper(std::string strDet_Name, int iInput); break;                  //int
 case 208: setVFATIShaperFeed(std::string strDet_Name, int iInput); break;              //int
 case 209: setVFATIComp(std::string strDet_Name, int iInput); break;                    //int
 case 210: setVFATMSPL(std::string strDet_Name, int iInput); break;                     //int
 case 212: setVFATThresh(std::string strDet_Name, int iInput); break;                   //int
 
 //TDC Methods
 case 301: setTDCChanDet(std::string strDet_Name, int iInput)                           //int
 case 302: setTDCFitParamName(std::string strDet_Name, string strParamName)             //string
 
 //Non Automatically assigned Parameters
 case 303: setTDCData(std::string strDet_Name, std::vector<int> vec_iData)  //vector<int>
 case 304: setTDCHistoDet(std::string strDet_Name, TH1F *hInput)    //TH1F*
 case 305: setTDCHistoNPks(std::string strDet_Name, int iInput)
 case 306: setTDCFit(std::string strDet_Name, TF1 *func_Input, std::vector<std::string> vec_strParamName)
 case 307: setTDCEff(std::string strDet_Name, float fInput)
 case 308: setTDCHistoOR(TH1F *hInput)
 case 309: setTDCHistoORNPks(int iInput)
 case 310: setTDCFitOR(TF1 *func_Input, std::vector<std::string> vec_strParamName)
 case 311: setTDCEffOR(float fInput)
 case 312: setTDCHistoAND(TH1F *hInput)
 case 313: setTDCHistoANDNPks(int iInput)
 case 314: setTDCFitAND(TF1 *func_Input, std::vector<std::string> vec_strParamName)
 case 315: setTDCEffAND(float fInput)
 case 316: setTDCHistoDeltaT(TH1F *hInput)
 case 317: setTDCHistoCorr(TH2F *hInput2D)
 
 */

namespace Timing {
    struct Detector{
        //Detector Info
        //=============================
        bool bDet_Gain_IndepVar_Imon;
        
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
        
        int iVFAT_IPreAmpIn = -1;
        int iVFAT_IPreAmpFeed = -1;
        int iVFAT_IPreAmpOut = -1;
        
        int iVFAT_IShaper = -1;
        int iVFAT_IShaperFeed = -1;
        
        int iVFAT_IComp = -1;        //Current to Comparator
        
        int iVFAT_MSPL = -1;         //Monostable Pulse Length
        
        //int iVFAT_Latency = -1;      //VFAT Latency
        
        float fVFAT_Thresh = -1;     //VFAT Threshold in fC
        
        //TDC
        //=============================
        float fTDC_Histo_Mean;
        float fTDC_Histo_RMS;
        float iTDC_Histo_nPks;
        
        float fTDC_Eff;
        
        //float fTDC_Histo_PkInt_1stMax = -1;
        //float fTDC_Histo_PkInt_2ndMax = -1;
        //float fTDC_Histo_PkInt_3rdMax = -1;
        
        //float fTDC_Histo_PkPos_1stMax = -1;
        //float fTDC_Histo_PkPos_2ndMax = -1;
        //float fTDC_Histo_PkPos_3rdMax = -1;
        
        float fTDC_Fit_Chi2;
        float fTDC_Fit_NDF;
        
        int iTDC_Chan;
        
        ROOT::TF1 *func_TDC_Fit;     //User Specified
        ROOT::TH1F *hTDC_Histo;          //Made from vec_iTDC_Data
        
        std::vector<std::string> vec_strParamName;
        
        //std::vector<float> vec_fTDC_Fit_Param;     //Fit parameter; assuming user labels first parameter in fit as [0] the indices match identically
        //std::vector<float> vec_fTDC_Fit_ParamErr;  //Error on Fit parameter
        std::map<std::string, float> map_fTDC_Fit_Param; //Fit parameters
        std::map<std::string, float> map_fTDC_Fit_ParamErr; //Error on fit parameters
        
        std::std::vector<int> vec_iTDC_Data; //Event-by-Event Data
        
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
            fTDC_Histo_Mean = fTDC_Histo_RMS = iTDC_Histo_nPks = -1;
            fTDC_Eff = -1;
            
            fTDC_Fit_Chi2 = fTDC_Fit_NDF = -1;
            
            iTDC_Chan = 0;
            
            func_TDC_Fit = nullptr;     //User Specified
            hTDC_Histo = nullptr;          //Made from vec_iTDC_Data
        }
    };
    
    //Run Info
    //Most values initialized to -1 (iBeam_Type to 0)
    //Pointers initialized to nullptr
    struct Run{
        //Run info
        //=============================
        float fTrig_Delay;    //Trigger Delay in ns;
        
        int iRun;           //Run Number
        
        int iBeam_Type;     //11->Electron, 13->Muon, 211->Pion
        
        int iTrig_Mode;     //0->Async, 1->Sync
        
        //Detectors
        //=============================
        //Detector Det1;
        //Detector Det2;
        std::map<std::string, Detector> map_det;
        //std::map<int, Detector> map_det;
        
        //TDC Info
        //=============================
        //float fTDC_Fit_Chi2;
        //float fTDC_Fit_NDF;
        
        int iTDC_Chan_Trig;         //Channel of the Trigger in the TDC
        
        ROOT::TF1 *func_TDC_Fit_OR;     //User Specified
        ROOT::TF1 *func_TDC_Fit_AND;     //User Specified
        
        ROOT::TH1F *hTDC_OR;
        ROOT::TH1F *hTDC_AND;
        ROOT::TH1F *hTDC_DeltaT;
        
        ROOT::TH2F *hTDC_Correlation;

        //OR
        float fTDC_Histo_OR_Mean;
        float fTDC_Histo_OR_RMS;
        float iTDC_Histo_OR_nPks;
        
        float fTDC_Eff_OR;
        
        float fTDC_Fit_OR_Chi2;
        float fTDC_Fit_OR_NDF;
        
        //AND
        float fTDC_Histo_AND_Mean;
        float fTDC_Histo_AND_RMS;
        float iTDC_Histo_AND_nPks;
        
        float fTDC_Eff_AND;
        
        float fTDC_Fit_AND_Chi2;
        float fTDC_Fit_AND_NDF;
        
        std::vector<std::string> vec_strParamName_OR;
        
        //std::vector<float> vec_fTDC_Fit_OR_Param;     //Fit parameter; assuming user labels first parameter in fit as [0] the indices match identically
        //std::vector<float> vec_fTDC_Fit_OR_ParamErr;  //Error on Fit parameter
        
        std::map<std::string, float> map_fTDC_Fit_OR_Param;     //Fit parameter
        std::map<std::string, float> map_fTDC_Fit_OR_ParamErr;  //Error on Fit parameter
        
        std::vector<std::string> vec_strParamName_AND;
        
        //std::vector<float> vec_fTDC_Fit_AND_Param;     //Fit parameter; assuming user labels first parameter in fit as [0] the indices match identically
        //std::vector<float> vec_fTDC_Fit_AND_ParamErr;  //Error on Fit parameter
        
        std::map<std::string, float> map_fTDC_Fit_AND_Param;     //Fit parameter
        std::map<std::string, float> map_fTDC_Fit_AND_ParamErr;  //Error on Fit parameter
        
        Run(){
            //Run info
            //=============================
            fTrig_Delay = -1;    //Trigger Delay in ns;
            
            iRun = iTrig_Mode = -1;           //Run Number
            
            iBeam_Type = 0;     //11->Electron, 13->Muon, 211->Pion
            
            //TDC Info
            //=============================
            fTDC_Fit_Chi2 = fTDC_Fit_NDF = -1;
            
            iTDC_Chan_Trig = 0;         //Channel of the Trigger in the TDC
            
            func_TDC_Fit_OR = func_TDC_Fit_AND = nullptr;     //User Specified
            hTDC_OR hTDC_AND = hTDC_DeltaT = nullptr;
            hTDC_Correlation = nullptr;
            
            fTDC_Fit_OR_Chi2 = fTDC_Fit_OR_NDF = -1;
            
            fTDC_Fit_AND_Chi2 = fTDC_Fit_AND_NDF = -1;
        }
    };
    
    //Container Class; All values are initialized to "-1"
    class TRunParameters : public TObject {
        
    public:
        TRunParameters();
        
        //Data Members==========================================================
        Run run; //This time let's make it public to feature more intuitive access
        
        //std::vector<int> vec_iTDC_Chan; //std::vector of stored TDC Channels;
        
        //Getters===============================================================
        virtual Run getRun(){ return run;};
        
        virtual Detector getDetector(std::string strDet_Name){ return run.map_det[strDet_Name]; };
        
        //Run Methods - These are methods 1 to 100
        //=============================
        virtual int getRunNumber(){ return run.iRun;};
        
        virtual int getBeam(){ return run.iBeam_Type;};
        
        virtual int getTrigMode(){ return run.iTrig_Mode;};
        virtual float getTrigDelay(){ return run.fTrig_Delay;};
        
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
        
        //Detector Methods - These are methods 101 to 200
        //=============================
        virtual int getDetPos(std::string strDet_Name){ return run.map_det[strDet_Name].iDet_Pos; }
        
        //virtual float getDetGainConst(){ return run.fDet_Gain_Const;};    //Gain assumed to be exp(P0+P1*x)
        //virtual float getDetGainSlope(){ return run.fDet_Gain_Slope;};    //Gain assumed to be exp(P0+P1*x)
        
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
        
        //VFAT Methods - These are methods 201 to 300
        //=============================
        
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
        
        //TDC Methods - These are methods 301 to 400
        //=============================
        virtual int getTDCChanNumberTrig(){ return run.iTDC_Chan_Trig;};
        virtual int getTDCChanNumberDet(std::string strDet_Name){ return run.map_det[strDet_Name].iTDC_Chan; };
        
        virtual int getTDCHistoPks(std::string strDet_Name){ return run.map_det[strDet_Name].iTDC_Histo_nPks; };
        
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
        
        //Action Methods
        //=============================
        //template<class T>
        //virtual void getParameter(T &input, int methodIdx);
        //virtual void getParameter(int &input, int methodIdx);
        //virtual void getParameter(float &input, int methodIdx);
        //virtual void getParameter(std::string &input, int methodIdx);
        
        //Misc==================================================================
        //virtual void calcGain(float fDepVar);  //fDepVar variable gain formula is parameterized by
        //virtual void calcGain(float fDepVar, float &fGain, float &fGain_Err);
        
        //Setters===============================================================
        virtual void setRun(Run inputRun){ run = inputRun; return; };
        virtual void setDetector(std::string strDet_Name, Detector inputDet){ run.map_det[strDet_Name] = inputDet; return;};
        
        //Run Methods - These are methods 1 to 100
        //=============================
        virtual void setRunNumber(std::string strRun_Name, int iInput){run.iRun = iInput; return;};
        virtual void setBeam(std::string strRun_Name, std::string strInput);
        
        virtual void setTrigMode(std::string strRun_Name, std::string strInput);
        virtual void setTrigDelay(std::string strRun_Name, float fInput){run.fTrig_Delay = fInput; return; };
        
        //Set channel of the trigger
        virtual void setTDCChanTrig(std::string strRun_Name, int iInput){ run.iTDC_Chan_Trig = iInput; return; };
        
        //Set names of parameters - OR
        //virtual void setTDCFitParamNameOR(std::string strRun_Name, std::vector<std::string> vec_strParamName){
        //    run.map_det[strDet_Name].vec_strParamName_OR = vec_strParamName;
        //};
        virtual void setTDCFitParamNameOR(std::string strRun_Name, std::string strParamName){
            run.map_det[strDet_Name].vec_strParamName_OR.push_back(strParamName);
        };
        
        //Set names of parameters - AND
        //virtual void setTDCFitParamNameAND(std::string strRun_Name, std::vector<std::string> vec_strParamName){
        //    run.map_det[strDet_Name].vec_strParamName_AND = vec_strParamName;
        //};
        virtual void setTDCFitParamNameOR(std::string strRun_Name, std::string strParamName){
            run.map_det[strDet_Name].vec_strParamName_AND.push_back(strParamName);
        };
        
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
        
        //Detector Methods - These are methods 101 to 200
        //=============================
        //Detector Position in Filename
        virtual void setDetPos(std::string strDet_Name, int iInput){run.map_det[strDet_Name].iDet_Pos = iInput; return;};
        
        //Detector (iEta,iPhi) Position
        virtual void setDetEta(std::string strDet_Name, int iInput){run.map_det[strDet_Name].iDet_Eta = iInput; return;};
        virtual void setDetPhi(std::string strDet_Name, int iInput){run.map_det[strDet_Name].iDet_Phi = iInput; return;};
        
        //Detector Current
        virtual void setDetCurrent(std::string strDet_Name, float fInput);
        
        //Detector VDrift
        virtual void setDetDriftV(std::string strDet_Name, float fInput){run.map_det[strDet_Name].fDet_VDrift = fInput; return;};
        
        //Detector Name
        //virtual void setDetName(std::string input){run.map_det[strDet_Name].strDet_Name = input; return;};
        
        //Gas Fraction - Argon
        virtual void setDetGasFracAr(std::string strDet_Name, float fInput){run.map_det[strDet_Name].fDet_GasFrac_Ar = fInput; return;};
        virtual void setDetGasFracCO2(std::string strDet_Name, float fInput){run.map_det[strDet_Name].fDet_GasFrac_CO2 = fInput; return;};
        virtual void setDetGasFracCF4(std::string strDet_Name, float fInput){run.map_det[strDet_Name].fDet_GasFrac_CF4 = fInput; return;};
        
        //Gain
        virtual void setDetGain(std::string strDet_Name, float fDepVar);
        
        virtual void setDetGainConst(std::string strDet_Name, float fInput){ run.map_det[strDet_Name].fDet_Gain_Const = fInput; return;};
        virtual void setDetGainConstErr(std::string strDet_Name, float fInput){ run.map_det[strDet_Name].fDet_Gain_Const_Err = fInput; return;};
        
        virtual void setDetGainSlope(std::string strDet_Name, float fInput){ run.map_det[strDet_Name].fDet_Gain_Slope = fInput; return;};
        virtual void setDetGainSlopeErr(std::string strDet_Name, float fInput){ run.map_det[strDet_Name].fDet_Gain_Slope_Err = fInput; return;};
        
        //Independent Variable for Gain
        virtual void setDetGainIndepVarIsCurrent(std::string strDet_Name, bool bInput){ run.map_det[strDet_Name].bDet_Gain_IndepVar_Imon = bInput; return;};
        
        //Detector Voltages - GEM1
        virtual void setDetGEM1TopV(std::string strDet_Name, float fInput){ run.map_det[strDet_Name].fDet_VG1_Top = fInput; return;};
        virtual void setDetGEM1BotV(std::string strDet_Name, float fInput){ run.map_det[strDet_Name].fDet_VG1_Bot = fInput; return;};
        
        //Detector Voltages - GEM2
        virtual void setDetGEM2TopV(std::string strDet_Name, float fInput){ run.map_det[strDet_Name].fDet_VG2_Top = fInput; return;};
        virtual void setDetGEM2BotV(std::string strDet_Name, float fInput){ run.map_det[strDet_Name].fDet_VG2_Bot = fInput; return;};
        
        //Detector Voltages - GEM3
        virtual void setDetGEM3TopV(std::string strDet_Name, float fInput){ run.map_det[strDet_Name].fDet_VG3_Top = fInput; return;};
        virtual void setDetGEM3BotV(std::string strDet_Name, float fInput){ run.map_det[strDet_Name].fDet_VG3_Bot = fInput; return;};
        
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
        
        //VFAT Methods - These are methods 201 to 300
        //=============================
        //virtual void setTURBOID(int iInput){ run.iTURBO_ID = input; return;};
        
        //VFAT Position
        virtual void setVFATPos(std::string strDet_Name, int iInput){ run.map_det[strDet_Name].iVFAT_Pos = iInput; return;};
        
        //virtual void setVFATID(std::string input){ run.strVFAT_ID = input; return;};
        
        //VFAT IPreAmp
        virtual void setVFATIPreAmpIn(std::string strDet_Name, int iInput){ run.map_det[strDet_Name].iVFAT_IPreAmpIn = iInput; return;};
        virtual void setVFATIPreAmpFeed(std::string strDet_Name, int iInput){ run.map_det[strDet_Name].iVFAT_IPreAmpFeed = iInput; return;};
        virtual void setVFATIPreAmpOut(std::string strDet_Name, int iInput){ run.map_det[strDet_Name].iVFAT_IPreAmpOut = iInput; return;};
        
        //VFAT IShaper
        virtual void setVFATIShaper(std::string strDet_Name, int iInput){ run.map_det[strDet_Name].iVFAT_IShaper = iInput; return;};
        virtual void setVFATIShaperFeed(std::string strDet_Name, int iInput){ run.map_det[strDet_Name].iVFAT_IShaperFeed = iInput; return;};
        
        //VFAT IComp (comparator)
        virtual void setVFATIComp(std::string strDet_Name, int iInput){ run.map_det[strDet_Name].iVFAT_IComp = iInput; return;};
        
        //VFAT MSPL
        virtual void setVFATMSPL(std::string strDet_Name, int iInput){ run.map_det[strDet_Name].iVFAT_MSPL = iInput; return;};
        
        //virtual void setVFATLatency(int iInput){ run.map_det[strDet_Name].iVFAT_Latency = input; return;};
        
        virtual void setVFATThresh(std::string strDet_Name, int iInput){ run.map_det[strDet_Name].fVFAT_Thresh = fInput*0.08; return;};
        
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
        
        //TDC Methods - These are methods 301 to 400
        //=============================
        //Set channel of the detectors
        virtual void setTDCChanDet(std::string strDet_Name, int iInput){ run.map_det[strDet_Name].iTDC_Chan = iInput; return; };
        
        //Set names of parameters
        //virtual void setTDCFitParamName(std::string strDet_Name, std::vector<std::string> vec_strParamName){
            //run.map_det[strDet_Name].vec_strParamName = vec_strParamName;
        //};
        virtual void setTDCFitParamName(std::string strDet_Name, std::string strParamName){
            run.map_det[strDet_Name].vec_strParamName.push_back(strParamName);
        };
        
        //Set Detector Data
        virtual void setTDCData(std::string strDet_Name, std::vector<int> vec_iData){run.map_det[strDet_Name].vec_iTDC_Data = vec_iData1; return;};
        
        virtual void setTDCHistoDet(std::string strDet_Name, TH1F *hInput){
            run.map_det[strDet_Name].hTDC_Histo = hInput;
            run.map_det[strDet_Name].fTDC_Histo_Mean = hInput->GetMean();
            run.map_det[strDet_Name].fTDC_Histo_RMS = hInput->GetRMS();
            return;
        };
        
        virtual void setTDCHistoNPks(std::string strDet_Name, int iInput){
            run.map_det[strDet_Name].iTDC_Histo_nPks = iInput;
        }
        
        virtual void setTDCFit(std::string strDet_Name, TF1 *func_Input, std::vector<std::string> vec_strParamName){
            //run.map_det[strDet_Name].vec_fTDC_Fit_Param.clear();
            //run.map_det[strDet_Name].vec_fTDC_Fit_ParamErr.clear();
            
            run.map_det[strDet_Name].map_fTDC_Fit_Param.clear();
            run.map_det[strDet_Name].map_fTDC_Fit_ParamErr.clear();
            //run.map_det[strDet_Name].vec_strParamName.clear();
            
            run.map_det[strDet_Name].func_TDC_Fit = func_Input;
            
            //for (int i=0; i < func_Input->GetNpar(); ++i) {
                //run.map_det[strDet_Name].vec_fTDC_Fit_Param.push_back( func_Input->GetParameter(i) );
                //run.map_det[strDet_Name].vec_fTDC_Fit_ParamErr.push_back( func_Input->GetParError(i) );
            //}
            
            for (auto strIter = vec_strParamName.begin(); strIter != vec_strParamName.end(); ++strIter) {
                run.map_det[strDet_Name].map_fTDC_Fit_Param[*strIter] = func_Input->GetParameter(i);
                run.map_det[strDet_Name].map_fTDC_Fit_ParamErr[*strIter] = func_Input->GetParError(i);
            }
            
            run.map_det[strDet_Name].fTDC_Fit_Chi2  = func_Input->GetChisquare();
            run.map_det[strDet_Name].fTDC_Fit_NDF   = func_Input->GetNDF();
            
            return;
        };
        
        virtual void setTDCEff(std::string strDet_Name, float fInput){run.map_det[strDet_Name].fTDC_Eff = fInput; return;};
        
        //Sync Mode
        //------------------------------------------------------
        //virtual void setTDCHistoPksInt_1stMax(float fInput){ run.fTDC_Histo_PkInt_1stMax = input; return;};
        //virtual void setTDCHistoPksInt_2ndMax(float fInput){ run.fTDC_Histo_PkInt_2ndMax = input; return;};
        //virtual void setTDCHistoPksInt_3rdMax(float fInput){ run.fTDC_Histo_PkInt_3rdMax = input; return;};
        
        //virtual void setTDCHistoPksPos_1stMax(float fInput){ run.fTDC_Histo_PkPos_1stMax = input; return;};
        //virtual void setTDCHistoPksPos_2ndMax(float fInput){ run.fTDC_Histo_PkPos_2ndMax = input; return;};
        //virtual void setTDCHistoPksPos_3rdMax(float fInput){ run.fTDC_Histo_PkPos_3rdMax = input; return;};
        
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
        
        //Complex Analysis Histograms - DETECTOR OR
        //=============================
        virtual void setTDCHistoOR(TH1F *hInput){
            run.hTDC_OR = hInput;
            run.fTDC_Histo_OR_Mean = hInput->GetMean();
            run.fTDC_Histo_OR_RMS = hInput->GetRMS();
            return;
        };
        
        virtual void setTDCHistoORNPks(int iInput){
            run.iTDC_Histo_OR_nPks = iInput;
        };
        
        virtual void setTDCFitOR(TF1 *func_Input, std::vector<std::string> vec_strParamName){
            run.map_fTDC_Fit_OR_Param.clear();
            run.map_fTDC_Fit_OR_ParamErr.clear();
            //run.map_det[strDet_Name].vec_strParamName_OR.clear();
            
            run.func_TDC_Fit_OR = func_Input;
            
            //for (int i=0; i < func_Input->GetNpar(); ++i) {
            //run.map_det[strDet_Name].vec_fTDC_Fit_Param.push_back( func_Input->GetParameter(i) );
            //run.map_det[strDet_Name].vec_fTDC_Fit_ParamErr.push_back( func_Input->GetParError(i) );
            //}
            
            for (auto strIter = vec_strParamName.begin(); strIter != vec_strParamName.end(); ++strIter) {
                run.map_fTDC_Fit_OR_Param[*strIter] = func_Input->GetParameter(i);
                run.map_fTDC_Fit_OR_ParamErr[*strIter] = func_Input->GetParError(i);
            }
            
            run.fTDC_Fit_OR_Chi2  = func_Input->GetChisquare();
            run.fTDC_Fit_OR_NDF   = func_Input->GetNDF();
            
            return;
        };
        
        virtual void setTDCEffOR(float fInput){run.fTDC_Eff_OR = fInput; return;};
        
        //Complex Analysis Histograms - DETECTOR AND
        //=============================
        virtual void setTDCHistoAND(TH1F *hInput){
            run.hTDC_AND = hInput;
            run.fTDC_Histo_AND_Mean = hInput->GetMean();
            run.fTDC_Histo_AND_RMS = hInput->GetRMS();
            return;
        };
        
        virtual void setTDCHistoANDNPks(int iInput){
            run.iTDC_Histo_AND_nPks = iInput;
        };
        
        virtual void setTDCFitAND(TF1 *func_Input, std::vector<std::string> vec_strParamName){
            run.map_fTDC_Fit_AND_Param.clear();
            run.map_fTDC_Fit_AND_ParamErr.clear();
            //run.map_det[strDet_Name].vec_strParamName_AND.clear();
            
            run.func_TDC_Fit_AND = func_Input;
            
            //for (int i=0; i < func_Input->GetNpar(); ++i) {
            //run.map_det[strDet_Name].vec_fTDC_Fit_Param.push_back( func_Input->GetParameter(i) );
            //run.map_det[strDet_Name].vec_fTDC_Fit_ParamErr.push_back( func_Input->GetParError(i) );
            //}
            
            for (auto strIter = vec_strParamName.begin(); strIter != vec_strParamName.end(); ++strIter) {
                run.map_fTDC_Fit_AND_Param[*strIter] = func_Input->GetParameter(i);
                run.map_fTDC_Fit_AND_ParamErr[*strIter] = func_Input->GetParError(i);
            }
            
            run.fTDC_Fit_AND_Chi2  = func_Input->GetChisquare();
            run.fTDC_Fit_AND_NDF   = func_Input->GetNDF();
            
            return;
        };
        
        virtual void setTDCEffAND(float fInput){run.fTDC_Eff_AND = fInput; return;};
        
        //Complex Analysis Histograms - Delta Detector Signal & Correlation
        //=============================
        virtual void setTDCHistoDeltaT(TH1F *hInput){run.hTDC_DeltaT = hInput; return;};
        virtual void setTDCHistoCorr(TH2F *hInput2D){run.hTDC_Correlation = hInput2D; return;};
        
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
        
        //Action Methods
        //template<class T>
        //virtual void setParameter(T input, int methodIdx);
        virtual void setParameter(std::string strDetOrRunName, bool bInput, int iMthdIdx);
        virtual void setParameter(std::string strDetOrRunName, int iInput, int iMthdIdx);
        virtual void setParameter(std::string strDetOrRunName, float fInput, int iMthdIdx);
        virtual void setParameter(std::string strDetOrRunName, std::string strInput, int iMthdIdx);
        
    //private:
        //Run run;
    //};
} //End namespace Timing
#endif /* defined(____TRunParameters__) */