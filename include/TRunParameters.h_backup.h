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
//#include <map>
//#include <math>
#include <stdio.h>
#include <string>
#include <utility>

//ROOT Includes
#include "TF1.h"
#include "TH1F.h"
#include "TObject.h"

/*  METHOD INDEX GUIDE
 
 //Run Methods                              //Expected Input Data Type
 case 1: setRun(input); break;              //int
 case 2: setBeam(input); break;             //string
 case 3: setTrigMode(input); break;         //string
 case 4: setTrigDelay(input); break;        //float
 
 //Detector Methods
 case 101: setDetPos(input); break;         //int
 case 102: setDetEta(input); break;         //int
 case 103: setDetPhi(input); break;         //int
 case 104: setDetCurrent(input); break;     //float
 case 105: setDetDriftV(input); break;      //float
 case 106: setDetName(input); break;        //      string
 case 107: setDetGasFracAr(input); break;   //float
 case 108: setDetGasFracCO2(input); break;  //float
 case 109: setDetGasFracCF4(input); break;  //float
 case 110: setDetGainConst(input); break;   //float
 case 111: setDetGainConstErr(input); break;//float
 case 112: setDetGainSlope(input); break;   //float
 case 113: setDetGainSlopeErr(input); break;//float
 case 114: setDetGainIndepVarIsCurrent(input); break;   //boolean
 case 115: setDetGEM1TopV(input); break;    //float
 case 116: setDetGEM1BotV(input); break;    //float
 case 117: setDetGEM2TopV(input); break;    //float
 case 118: setDetGEM2BotV(input); break;    //float
 case 119: setDetGEM3TopV(input); break;    //float
 case 120: setDetGEM3BotV(input); break;    //float
 
 //VFAT Methods
 case 201: setTURBOID(input); break;         //int
 case 202: setVFATPos(input); break;         //int
 case 203: setVFATID(input); break;          //     string
 case 204: setVFATIPreAmpIn(input); break;   //int
 case 205: setVFATIPreAmpFeed(input); break; //int
 case 206: setVFATIPreAmpOut(input); break;  //int
 case 207: setVFATIShaper(input); break;     //int
 case 208: setVFATIShaperFeed(input); break; //int
 case 209: setVFATIComp(input); break;       //int
 case 210: setVFATMSPL(input); break;        //int
 case 211: setVFATLatency(input); break;     //int
 case 212: setVFATThresh(input); break;      //int
 
 //TDC Methods
 case 301: setTDCChanNumber(input); break;          //int
 case 302: setTDCHisto(input); break;               //      TH1F*
 case 303: setTDCHistoMean(input); break;           //float
 case 304: setTDCHistoRMS(input); break;            //float
 case 305: setTDCHistoPks(input); break;            //float
 case 306: setTDCFit(input); break;                 //      TF1*
 case 307: setTDCFitAmp(input); break;              //float
 case 308: setTDCFitAmpErr(input); break;           //float
 case 309: setTDCFitMean(input); break;             //float
 case 310: setTDCFitMeanErr(input); break;          //float
 case 311: setTDCFitSigma(input); break;            //float
 case 312: setTDCFitSigmaErr(input); break;         //float
 case 313: setTDCFitChi2(input); break;             //float
 case 314: setTDCFitNDF(input); break;              //float
 case 315: setTDCFitConvo(input); break;            //      TF1*
 case 316: setTDCFitConvoAmp(input); break;         //float
 case 317: setTDCFitConvoAmpErr(input); break;      //float
 case 318: setTDCFitConvoMean(input); break;        //float
 case 319: setTDCFitConvoMeanErr(input); break;     //float
 case 320: setTDCFitConvoSigma(input); break;       //float
 case 321: setTDCFitConvoSigmaErr(input); break;    //float
 case 322: setTDCFitConvoChi2(input); break;        //float
 case 323: setTDCFitConvoNDF(input); break;         //float
 case 324: setTDCNumDeconvoTimeResp(input); break;  //float
 case 325: setTDCHistoPksInt_1stMax(input); break;  //float
 case 326: setTDCHistoPksInt_2ndMax(input); break;  //float
 case 327: setTDCHistoPksInt_3rdMax(input); break;  //float
 case 328: setTDCHistoPksPos_1stMax(input); break;  //float
 case 329: setTDCHistoPksPos_2ndMax(input); break;  //float
 case 330: setTDCHistoPksPos_3rdMax(input); break;  //float
 
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
        
        std::string strDet_Name; //Detector Name
        
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
        float fTDC_Histo_nPks;
        
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
        
        vector<float> vec_fTDC_Fit_Param;     //Fit parameter; assuming user labels first parameter in fit as [0] the indices match identically
        vector<float> vec_fTDC_Fit_ParamErr;  //Error on Fit parameter
        
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
            fTDC_Histo_Mean = fTDC_Histo_RMS = fTDC_Histo_nPks = -1;
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
        Detector Det1;
        Detector Det2;
        
        //TDC Info
        //=============================
        float fTDC_Fit_Chi2;
        float fTDC_Fit_NDF;
        
        int iTDC_Chan_Trig;         //Channel of the Trigger in the TDC
        
        ROOT::TF1 *func_TDC_Fit_OR;     //User Specified
        ROOT::TF1 *func_TDC_Fit_AND;     //User Specified
        
        ROOT::TH1F *hTDC_OR;
        ROOT::TH1F *hTDC_AND;
        ROOT::TH1F *hTDC_DeltaT;
        
        ROOT::TH2F *hTDC_Correlation;

        float fTDC_Fit_OR_Chi2;
        float fTDC_Fit_OR_NDF;
        
        float fTDC_Fit_AND_Chi2;
        float fTDC_Fit_AND_NDF;
        
        vector<float> vec_fTDC_Fit_OR_Param;     //Fit parameter; assuming user labels first parameter in fit as [0] the indices match identically
        vector<float> vec_fTDC_Fit_OR_ParamErr;  //Error on Fit parameter
        
        vector<float> vec_fTDC_Fit_AND_Param;     //Fit parameter; assuming user labels first parameter in fit as [0] the indices match identically
        vector<float> vec_fTDC_Fit_AND_ParamErr;  //Error on Fit parameter
        
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
        
        //Getters===============================================================
        virtual Run getRun(){ return run;};
        virtual Detector get1stDetector(){return run.Det1;};
        virtual Detector get2ndDetector(){return run.Det2;};
        
        //Run Methods - These are methods 1 to 100
        //=============================
        virtual int getRunNumber(){ return run.iRun;};
        
        virtual int getBeam(){ return run.iBeam_Type;};
        
        virtual int getTrigMode(){ return run.iTrig_Mode;};
        virtual float getTrigDelay(){ return run.fTrig_Delay;};
        
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
        
        //Detector Methods - These are methods 101 to 200
        //=============================
        //virtual int getDetPos(){ return run.iDet_Pos;};
        std::pair<int,int> getDetPos(){ return std::make_pair(run.Det1.iDet_Pos, run.Det2.iDet_Pos); };
        //virtual float getDetGainConst(){ return run.fDet_Gain_Const;};    //Gain assumed to be exp(P0+P1*x)
        //virtual float getDetGainSlope(){ return run.fDet_Gain_Slope;};    //Gain assumed to be exp(P0+P1*x)
        
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
        
        //VFAT Methods - These are methods 201 to 300
        //=============================
        
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
        
        //TDC Methods - These are methods 301 to 400
        //=============================
        //virtual int getTDCChanNumber(){ return run.iTDC_Chan;};
        int getTDCChanNumberTrig(){ return run.iTDC_Chan_Trig;};
        std::pair<int, int> getTDCChanNumberDet(){ return std::make_pair(run.Det1.iTDC_Chan, run.Det2.iTDC_Chan ); };
        
        //virtual int getTDCHistoPks(){ return (int) run.fTDC_Histo_nPks;};
        std::pair<int, int> getTDCHistoPks(){ return std::make_pair(run.Det1.fTDC_Histo_nPks, run.Det2.fTDC_Histo_nPks); };
        
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
        
        //Run Methods - These are methods 1 to 100
        //=============================
        virtual void setRunNumber(int input){run.iRun = input; return;};
        virtual void setBeam(std::string input);
        
        virtual void setTrigMode(std::string input);
        virtual void setTrigDelay(float input){run.fTrig_Delay = input; return; };
        
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
        
        //Detector Methods - These are methods 101 to 200
        //=============================
        //Detector Position in Filename
        virtual void setDetPos(int input){run.Det1.iDet_Pos = input; return;};
        virtual void setDetPos(std::pair<int,int> pairDet){
            setDetPos(pairDet.first);
            run.Det2.iDet_Pos = pairDet.second;
            return;
        }
        
        //Detector iEta Position
        virtual void setDetEta(int input){run.Det1.iDet_Eta = input; return;};
        virtual void setDetEta(std::pair<int,int> pairDet){
            setDetEta(pairDet.first);
            run.Det2.iDet_Eta = pairDet.second;
            return;
        }
        
        //Detector iPhi Position
        virtual void setDetPhi(int input){run.Det1.iDet_Phi = input; return;};
        virtual void setDetPhi(std::pair<int,int> pairDet){
            setDetPhi(pairDet.first);
            run.Det2.iDet_Phi = pairDet.second;
            return;
        }
        
        //Detector Current
        virtual void setDetCurrent(float input);
        virtual void setDetCurrent(std::pair<float,float> pairDet);
        
        //Detector VDrift
        virtual void setDetDriftV(float input){run.Det1.fDet_VDrift = input; return;};
        virtual void setDetDriftV(std::pair<float,float> pairDet){
            setDetDriftV(pairDet.first);
            run.Det2.fDet_VDrift = pairDet.second;
            return;
        };
        
        //Detector Name
        virtual void setDetName(std::string input){run.Det1.strDet_Name = input; return;};
        virtual void setDetName(std::pair<std::string,std::string> pairDet){
            setDetName(pairDet.first);
            run.Det2.strDet_Name = pairDet.second;
            return;
        };
        
        //Gas Fraction - Argon
        virtual void setDetGasFracAr(float input){run.Det1.fDet_GasFrac_Ar = input; return;};
        virtual void setDetGasFracAr(std::pair<float,float> pairDet){
            setDetGasFracAr(pairDet.first);
            run.Det2.fDet_GasFrac_Ar = pairDet.second;
            return;
        };
        
        //Gas Fraction - CO2
        virtual void setDetGasFracCO2(float input){run.Det1.fDet_GasFrac_CO2 = input; return;};
        virtual void setDetGasFracCO2(std::pair<float,float> pairDet){
            setDetGasFracCO2(pairDet.first);
            run.Det2.fDet_GasFrac_CO2 = pairDet.second;
            return;
        };
        
        //Gas Fraction - CF4
        virtual void setDetGasFracCF4(float input){run.Det1.fDet_GasFrac_CF4 = input; return;};
        virtual void setDetGasFracCF4(std::pair<float,float> pairDet){
            setDetGasFracCF4(pairDet.first);
            run.Det2.fDet_GasFrac_CF4 = pairDet.second;
            return;
        };
        
        //Gain
        virtual void setDetGain(float fDepVar);
        virtual void setDetGain(std::pair<float,float> pairDet);
        
        virtual void setDetGainConst(float input){ run.Det1.fDet_Gain_Const = input; return;};
        virtual void setDetGainConst(std::pair<float,float> pairDet){
            setDetGainConst(pairDet.first);
            run.Det2.fDet_Gain_Const = pairDet.second;
            return;
        };
        
        virtual void setDetGainConstErr(float input){ run.Det1.fDet_Gain_Const_Err = input; return;};
        virtual void setDetGainConstErr(std::pair<float,float> pairDet){
            setDetGainConstErr(pairDet.first);
            run.Det2.fDet_Gain_Const_Err = pairDet.second;
            return;
        };
        
        virtual void setDetGainSlope(float input){ run.Det1.fDet_Gain_Slope = input; return;};
        virtual void setDetGainSlope(std::pair<float,float> pairDet){
            setDetGainSlope(pairDet.first);
            run.Det2.fDet_Gain_Slope = pairDet.second;
            return;
        };
        
        virtual void setDetGainSlopeErr(float input){ run.Det1.fDet_Gain_Slope_Err = input; return;};
        virtual void setDetGainSlopeErr(std::pair<float,float> pairDet){
            setDetGainSlopeErr(pairDet.first);
            run.Det2.fDet_Gain_Slope_Err = pairDet.second;
            return;
        };
        
        //Independent Variable for Gain
        virtual void setDetGainIndepVarIsCurrent(bool input){ run.Det1.bDet_Gain_IndepVar_Imon = input; return;};
        virtual void setDetGainIndepVarIsCurrent(std::pair<bool,bool> pairDet){
            setDetGainIndepVarIsCurrent(pairDet.first);
            run.Det2.bDet_Gain_IndepVar_Imon = pairDet.second;
            return;
        };
        
        //Detector Voltages - GEM1 Top
        virtual void setDetGEM1TopV(float input){ run.Det1.fDet_VG1_Top = input; return;};
        virtual void setDetGEM1TopV(std::pair<float,float> pairDet){
            setDetGEM1TopV(pairDet.first);
            run.Det2.fDet_VG1_Top = pairDet.second;
            return;
        };
        
        //Detector Voltages - GEM1 Bot
        virtual void setDetGEM1BotV(float input){ run.Det1.fDet_VG1_Bot = input; return;};
        virtual void setDetGEM1BotV(std::pair<float,float> pairDet){
            setDetGEM1TopV(pairDet.first);
            run.Det2.fDet_VG1_Bot = pairDet.second;
            return;
        };
        
        //Detector Voltages - GEM2 Top
        virtual void setDetGEM2TopV(float input){ run.Det1.fDet_VG2_Top = input; return;};
        virtual void setDetGEM2TopV(std::pair<float,float> pairDet){
            setDetGEM2TopV(pairDet.first);
            run.Det2.fDet_VG2_Top = pairDet.second;
            return;
        };
        
        //Detector Voltages - GEM2 Bot
        virtual void setDetGEM2BotV(float input){ run.Det1.fDet_VG2_Bot = input; return;};
        virtual void setDetGEM2BotV(std::pair<float,float> pairDet){
            setDetGEM2BotV(pairDet.first);
            run.Det2.fDet_VG2_Bot = pairDet.second;
            return;
        };
        
        //Detector Voltages - GEM3 Top
        virtual void setDetGEM3TopV(float input){ run.Det1.fDet_VG3_Top = input; return;};
        virtual void setDetGEM3TopV(std::pair<float,float> pairDet){
            setDetGEM3TopV(pairDet.first);
            run.Det2.fDet_VG3_Top = pairDet.second;
            return;
        };
        
        //Detector Voltages - GEM3 Bot
        virtual void setDetGEM3BotV(float input){ run.Det1.fDet_VG3_Bot = input; return;};
        virtual void setDetGEM3BotV(std::pair<float,float> pairDet){
            setDetGEM3BotV(pairDet.first);
            run.Det2.fDet_VG3_Bot = pairDet.second;
            return;
        };
        
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
        
        //VFAT Methods - These are methods 201 to 300
        //=============================
        //virtual void setTURBOID(int input){ run.iTURBO_ID = input; return;};
        
        //VFAT Position
        virtual void setVFATPos(int input){ run.Det1.iVFAT_Pos = input; return;};
        virtual void setVFATPos(std::pair<int,int> pairDet){
            setVFATPos(pairDet.first);
            run.Det2.iVFAT_Pos = pairDet.second;
            return;
        };
        
        //virtual void setVFATID(std::string input){ run.strVFAT_ID = input; return;};
        
        //VFAT IPreAmpIn
        virtual void setVFATIPreAmpIn(int input){ run.Det1.iVFAT_IPreAmpIn = input; return;};
        virtual void setVFATIPreAmpIn(std::pair<int,int> pairDet){
            setVFATIPreAmpIn(pairDet.first);
            run.Det2.iVFAT_IPreAmpIn = pairDet.second;
            return;
        };
        
        //VFAT IPreAmpFeed
        virtual void setVFATIPreAmpFeed(int input){ run.Det1.iVFAT_IPreAmpFeed = input; return;};
        virtual void setVFATIPreAmpFeed(std::pair<int,int> pairDet){
            setVFATIPreAmpFeed(pairDet.first);
            run.Det2.iVFAT_IPreAmpFeed = pairDet.second;
            return;
        };
        
        //VFAT IPreAmpOut
        virtual void setVFATIPreAmpOut(int input){ run.Det1.iVFAT_IPreAmpOut = input; return;};
        virtual void setVFATIPreAmpOut(std::pair<int,int> pairDet){
            setVFATIPreAmpOut(pairDet.first);
            run.Det2.iVFAT_IPreAmpOut = pairDet.second;
            return;
        };
        
        //VFAT IShaper
        virtual void setVFATIShaper(int input){ run.Det1.iVFAT_IShaper = input; return;};
        virtual void setVFATIShaper(std::pair<int,int> pairDet){
            setVFATIShaper(pairDet.first);
            run.Det2.iVFAT_IShaper = pairDet.second;
            return;
        };
        
        //VFAT IShaperFeed
        virtual void setVFATIShaperFeed(int input){ run.Det1.iVFAT_IShaperFeed = input; return;};
        virtual void setVFATIShaperFeed(std::pair<int,int> pairDet){
            setVFATIShaperFeed(pairDet.first);
            run.Det2.iVFAT_IShaperFeed = pairDet.second;
            return;
        };
        
        //VFAT IComp (comparator)
        virtual void setVFATIComp(int input){ run.Det1.iVFAT_IComp = input; return;};
        virtual void setVFATIComp(std::pair<int,int> pairDet){
            setVFATIComp(pairDet.first);
            run.Det2.iVFAT_IComp = pairDet.second;
            return;
        };
        
        //VFAT MSPL
        virtual void setVFATMSPL(int input){ run.Det1.iVFAT_MSPL = input; return;};
        virtual void setVFATMSPL(std::pair<int,int> pairDet){
            setVFATMSPL(pairDet.first);
            run.Det2.iVFAT_MSPL = pairDet.second;
            return;
        };
        
        //virtual void setVFATLatency(int input){ run.Det1.iVFAT_Latency = input; return;};
        
        virtual void setVFATThresh(int input){ run.Det1.fVFAT_Thresh = input*0.08; return;};
        virtual void setVFATThresh(std::pair<int,int> pairDet){
            setVFATThresh(pairDet.first);
            run.Det2.fVFAT_Thresh = pairDet.second * 0.08;
            return;
        };
        
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
        
        //TDC Methods - These are methods 301 to 400
        //=============================
        //Set channel of the trigger
        virtual void setTDCChanTrig(int input){ run.iTDC_Chan_Trig = input; return; };
        
        //Set channel of the detectors
        virtual void setTDCChanDet(int input){ run.Det1.iTDC_Chan = input; return; };
        virtual void setTDCChanDet(std::pair<int,int> pairDet){
            setTDCChanDet(pairDet.first);
            run.Det2.iTDC_Chan = pairDet.second;
            return;
        }
        
        //Set Detector Data
        virtual void setTDCData(std::vector<int> vec_iData1){
            run.Det1.vec_iTDC_Data = vec_iData1;
        }
        
        virtual void setTDCHistoDet(TH1F *input){
            run.Det1.hTDC_Histo = input;
        }
        
        
        //virtual void setTDCHisto(TH1F *input){ run.hTDC_Histo = input; return; };
        //virtual void setTDCHistoMean(float input){ run.fTDC_Histo_Mean = input; return;};
        //virtual void setTDCHistoRMS(float input){ run.fTDC_Histo_RMS = input; return;};
        //virtual void setTDCHistoPks(float input){ run.fTDC_Histo_nPks = input; return;};
        
        //TDC Fit
        //------------------------------------------------------
        //virtual void setTDCFit(TF1 *input){ run.func_Gaus = input; return; };
        //virtual void setTDCFitAmp(float input){ run.fTDC_Fit_Amp = input; return; };
        //virtual void setTDCFitAmpErr(float input){ run.fTDC_Fit_Amp_Err = input; return; };
        //virtual void setTDCFitMean(float input){ run.fTDC_Fit_Mean = input; return; };
        //virtual void setTDCFitMeanErr(float input){ run.fTDC_Fit_Mean_Err = input; return; };
        //virtual void setTDCFitSigma(float input){ run.fTDC_Fit_Sigma = input; return; };
        //virtual void setTDCFitSigmaErr(float input){ run.fTDC_Fit_Sigma_Err = input; return; };
        //virtual void setTDCFitChi2(float input){ run.fTDC_Fit_Chi2 = input; return; };
        //virtual void setTDCFitNDF(float input){ run.fTDC_Fit_NDF = input; return; };
        
        
        //Sync Mode
        //------------------------------------------------------
        virtual void setTDCHistoPksInt_1stMax(float input){ run.fTDC_Histo_PkInt_1stMax = input; return;};
        virtual void setTDCHistoPksInt_2ndMax(float input){ run.fTDC_Histo_PkInt_2ndMax = input; return;};
        virtual void setTDCHistoPksInt_3rdMax(float input){ run.fTDC_Histo_PkInt_3rdMax = input; return;};
        
        virtual void setTDCHistoPksPos_1stMax(float input){ run.fTDC_Histo_PkPos_1stMax = input; return;};
        virtual void setTDCHistoPksPos_2ndMax(float input){ run.fTDC_Histo_PkPos_2ndMax = input; return;};
        virtual void setTDCHistoPksPos_3rdMax(float input){ run.fTDC_Histo_PkPos_3rdMax = input; return;};
        
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
        
        //Action Methods
        //template<class T>
        //virtual void setParameter(T input, int methodIdx);
        virtual void setParameter(bool input, int methodIdx);
        virtual void setParameter(int input, int methodIdx);
        virtual void setParameter(float input, int methodIdx);
        virtual void setParameter(std::string input, int methodIdx);
        
    //private:
        //Run run;
    //};
} //End namespace Timing
#endif /* defined(____TRunParameters__) */