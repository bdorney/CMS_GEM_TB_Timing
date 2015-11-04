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

//My Includes
#include "TimingUtilityTypes.h"

//ROOT Includes
#include "TFile.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TObject.h"
#include "TTree.h"

/*  METHOD INDEX GUIDE
 
 //Run Methods                                                              //Expected Input Data Type
 case 1: setRunNumber(std::string strRunName, int iInput); break;                       //int
 case 2: setBeam(std::string strRunName, std::string strInput); break;                  //string
 case 3: setTrigMode(std::string strRunName, std::string strInput); break;              //string
 case 4: setTrigDelay(std::string strRunName, float fInput); break;                     //float
 case 5: setTDCChanTrig(std::string strRunName, int iInput); break;                     //int
 case 6: setTDCFitParamNameOR(std::string strRunName, string strParamName)              //string
 case 7: setTDCFitParamNameAND(std::string strRunName, string strParamName)             //string
 case 8: setSupermoduleHVSetPoint(std::string strRunName, float fInput)                 //float
 case 9: setTreeNameRun(std::string strRunName, std::string strTreeName)                //string
 case 10:setTreeNameRunParamDUT(std::string strRunName, std::string strTreeName)        //string
 
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
 case 121: setDetMultiChanHVCase(std::string strDet_Name, std::string strMultiChan)     //string
 case 122: setDetGEM1TopV(std::string strDet_Name, std::string strBranchName); break;   //string
 case 123: setDetGEM1BotV(std::string strDet_Name, std::string strBranchName); break;   //string
 case 124: setDetGEM2TopV(std::string strDet_Name, std::string strBranchName); break;   //string
 case 125: setDetGEM2BotV(std::string strDet_Name, std::string strBranchName); break;   //string
 case 126: setDetGEM3TopV(std::string strDet_Name, std::string strBranchName); break;   //string
 case 127: setDetGEM3BotV(std::string strDet_Name, std::string strBranchName); break;   //string
 
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
 case 303: setTDCData(std::string strDet_Name, std::vector<float> vec_iData)  //vector<float>
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
    //Container Class; All values are initialized to "-1"
    class TRunParameters : public TObject {
        
    public:
        TRunParameters();
        
        //Data Members==========================================================
        Timing::Run run; //This time let's make it public to feature more intuitive access
        
        //std::vector<int> vec_iTDC_Chan; //std::vector of stored TDC Channels;
        
        //Getters===============================================================
        virtual Run getRun(){ return run;};
        
        virtual int getNumDetectors(){ return run.map_det.size(); };
        virtual Detector getDetector(std::string strDet_Name){ return run.map_det[strDet_Name]; };
        virtual std::map<std::string, Detector> getDetectors(){ return run.map_det; };
        
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
        
        virtual int getTDCHistoPks(std::string strDet_Name){ return run.map_det[strDet_Name].timingResults.iTDC_Histo_nPks; };
        
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
        virtual void setRunName(std::string strRunName){ run.strRunName = strRunName; return;};
        
        virtual void setDetector(std::string strDet_Name, Detector inputDet){ run.map_det[strDet_Name] = inputDet; return;};
        
        //Run Methods - These are methods 1 to 100
        //=============================
        virtual void setRunNumber(std::string strRunName, int iInput){run.iRun = iInput; return;};
        virtual void setBeam(std::string strRunName, std::string strInput);
        
        virtual void setTrigMode(std::string strRunName, std::string strInput);
        virtual void setTrigDelay(std::string strRunName, float fInput){run.fTrig_Delay = fInput; return; };
        
        //Set channel of the trigger
        virtual void setTDCChanTrig(std::string strRunName, int iInput){ run.iTDC_Chan_Trig = iInput; return; };
        
        //Set names of parameters - OR
        //virtual void setTDCFitParamNameOR(std::string strRunName, std::vector<std::string> vec_strParamName){
        //    run.map_det[strDet_Name].vec_strParamName_OR = vec_strParamName;
        //};
        virtual void setTDCFitParamNameOR(std::string strRunName, std::string strParamName){
            run.timingResultsOR.vec_strParamName.push_back(strParamName);
        };
        
        //Set names of parameters - AND
        //virtual void setTDCFitParamNameAND(std::string strRunName, std::vector<std::string> vec_strParamName){
        //    run.map_det[strDet_Name].vec_strParamName_AND = vec_strParamName;
        //};
        virtual void setTDCFitParamNameAND(std::string strRunName, std::string strParamName){
            run.timingResultsAND.vec_strParamName.push_back(strParamName);
        };
        
        //Take the HV Setpoint of the supermodule from the dedicated TTree in the filename
        //  NOTE: this means the HV info in the filename does not necessarily correspond to the individual detectors
        virtual void setSupermoduleHVSetPoint(std::string strRunName, float fInput){
            //run.bTakeHVFromRunParamTree = true;
            run.fSupermoduleHVSetpoint = fInput;
            return;
        };
        
        //Set the name of the Tree that contains the data
        virtual void setTreeNameRun(std::string strRunName, std::string strTreeName){
            run.strTreeName_Run = strTreeName;
            return;
        };

        //Take the HV of each detector from the dedicated TTree in the ROOT file?
        //  NOTE: here the DET_V* automatic identifiers should be the name of the TBranch you want to use
        virtual void setTreeNameRunParamDUT(std::string strRunName, std::string strTreeName){
            run.bTakeHVFromRunParamTree = true;
            run.strTreeName_RunParam_DUT = strTreeName;
            return;
        };
        
        
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
        
        //Detector Methods - These are methods 101 to 200
        //=============================
        virtual void setDetParameterFromTree(std::string strDet_Name, std::string strBranchName, float &fInput);    //Untracked Method
        
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
        
        //Multichannel option?
        virtual void setDetMultiChanHVCase(std::string strDet_Name, std::string strMultiChan);
        
        //Set Detector Voltages from a Tree File
        virtual void setDetDriftV(std::string strDet_Name, std::string strBranchName){
            setDetParameterFromTree(strDet_Name, strBranchName, run.map_det[strDet_Name].fDet_VDrift);
            return;
        };
        virtual void setDetGEM1TopV(std::string strDet_Name, std::string strBranchName){
            setDetParameterFromTree(strDet_Name, strBranchName, run.map_det[strDet_Name].fDet_VG1_Top);
            return;
        };
        virtual void setDetGEM1BotV(std::string strDet_Name, std::string strBranchName){
            setDetParameterFromTree(strDet_Name, strBranchName, run.map_det[strDet_Name].fDet_VG1_Bot);
            return;
        };
        virtual void setDetGEM2TopV(std::string strDet_Name, std::string strBranchName){
            setDetParameterFromTree(strDet_Name, strBranchName, run.map_det[strDet_Name].fDet_VG2_Top);
            return;
        };
        virtual void setDetGEM2BotV(std::string strDet_Name, std::string strBranchName){
            setDetParameterFromTree(strDet_Name, strBranchName, run.map_det[strDet_Name].fDet_VG2_Bot);
            return;
        };
        virtual void setDetGEM3TopV(std::string strDet_Name, std::string strBranchName){
            setDetParameterFromTree(strDet_Name, strBranchName, run.map_det[strDet_Name].fDet_VG3_Top);
            return;
        };
        virtual void setDetGEM3BotV(std::string strDet_Name, std::string strBranchName){
            setDetParameterFromTree(strDet_Name, strBranchName, run.map_det[strDet_Name].fDet_VG3_Bot);
            return;
        };
        virtual void setDetCurrent(std::string strDet_Name, std::string strBranchName){
            setDetParameterFromTree(strDet_Name, strBranchName, run.map_det[strDet_Name].fDet_Imon);
            return;
        };
        
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
        
        virtual void setVFATThresh(std::string strDet_Name, int iInput){ run.map_det[strDet_Name].fVFAT_Thresh = iInput*0.08; return;};
        
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
            run.map_det[strDet_Name].timingResults.vec_strParamName.push_back(strParamName);
        };
        
        //Set Detector Data
        virtual void setTDCData(std::string strDet_Name, std::vector<int> vec_iData){run.map_det[strDet_Name].vec_iTDC_Data = vec_iData; return;};
        
        virtual void setTDCHistoDet(std::string strDet_Name, TH1F *hInput){
            run.map_det[strDet_Name].timingResults.hTDC_Histo = hInput;
            run.map_det[strDet_Name].timingResults.fTDC_Histo_Mean = hInput->GetMean();
            run.map_det[strDet_Name].timingResults.fTDC_Histo_RMS = hInput->GetRMS();
            return;
        };
        
        virtual void setTDCHistoNPks(std::string strDet_Name, int iInput){
            run.map_det[strDet_Name].timingResults.iTDC_Histo_nPks = iInput;
        }
        
        virtual void setTDCFit(std::string strDet_Name, TF1 *func_Input, std::vector<std::string> vec_strParamName);
        
        virtual void setTDCEff(std::string strDet_Name, float fInput){run.map_det[strDet_Name].timingResults.fTDC_Eff = fInput; return;};
        
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
            run.timingResultsOR.hTDC_Histo = hInput;
            run.timingResultsOR.fTDC_Histo_Mean = hInput->GetMean();
            run.timingResultsOR.fTDC_Histo_RMS = hInput->GetRMS();
            return;
        };
        
        virtual void setTDCHistoORNPks(int iInput){
            run.timingResultsOR.iTDC_Histo_nPks = iInput;
        };
        
        virtual void setTDCFitOR(TF1 *func_Input, std::vector<std::string> vec_strParamName);
        
        virtual void setTDCEffOR(float fInput){run.timingResultsOR.fTDC_Eff = fInput; return;};
        
        //Complex Analysis Histograms - DETECTOR AND
        //=============================
        virtual void setTDCHistoAND(TH1F *hInput){
            run.timingResultsAND.hTDC_Histo = hInput;
            run.timingResultsAND.fTDC_Histo_Mean = hInput->GetMean();
            run.timingResultsAND.fTDC_Histo_RMS = hInput->GetRMS();
            return;
        };
        
        virtual void setTDCHistoANDNPks(int iInput){
            run.timingResultsAND.iTDC_Histo_nPks = iInput;
        };
        
        virtual void setTDCFitAND(TF1 *func_Input, std::vector<std::string> vec_strParamName);
        
        virtual void setTDCEffAND(float fInput){run.timingResultsAND.fTDC_Eff = fInput; return;};
        
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
    }; //End Class TRunParameters
} //End namespace Timing
#endif /* defined(____TRunParameters__) */