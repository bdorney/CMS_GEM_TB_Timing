//
//  singleRunPlotter.cpp
//  
//
//  Created by Brian L Dorney on 25/11/15.
//
//
//C++ Includes
#include <cmath>
#include <string>
#include <sstream>

//CMS Includes
#include "src/tdrstyle.C"
#include "src/CMS_lumi.C"

//My Includes
#include "include/TimingUtilityFunctions.h"

//ROOT Includes
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TLine.h"

using namespace Timing;

//returns a formatted function
//TFile - Root file containing the desired histogram
//strInputName is expected to be the full logical name (e.g. "dir1/dir2/.../dirN/name")
TF1 *getFunction(string strInputName, TFile *file_Input){
    TF1 *ret_func = (TF1*)file_Input->Get(strInputName.c_str() );
    
    ret_func->SetLineColor(kRed+1);
    ret_func->SetLineWidth(3);
    ret_func->SetLineStyle(3);
    
    return ret_func;
} //end getFunction()

//Returns a formatted histogram
//TFile - Root file containing the desired histogram
//strInputName is expected to be the full logical name (e.g. "dir1/dir2/.../dirN/name")
TH1F * getHistogram(string strInputName, TFile *file_Input){
    //Variable Declaration
    TH1F *ret_histo = (TH1F*)file_Input->Get(strInputName.c_str() );
    
    int iNBins = ret_histo->GetNbinsX();
    float fMin = ret_histo->GetBinLowEdge(1);
    float fMax = ret_histo->GetBinLowEdge(iNBins+1);
    float fBinSize = (fMax - fMin) / iNBins;
    
    //Format X-Axis()
    ret_histo->GetXaxis()->SetNdivisions(8,5,0);
    ret_histo->GetXaxis()->SetRangeUser(950,1100);
    ret_histo->GetXaxis()->SetTitle("time #left(ns#right)");
    
    //Format Y-Axis
    ret_histo->GetYaxis()->SetNdivisions(8,5,0);
    ret_histo->GetYaxis()->SetRangeUser(0,400);
    ret_histo->GetYaxis()->SetTitle( ("Events / " + getString(fBinSize) + " ns" ).c_str() );
    ret_histo->GetYaxis()->SetTitleOffset(1);
    
    //Format Style
    ret_histo->SetLineColor(kBlack);
    ret_histo->SetLineWidth(3);
    ret_histo->SetMarkerColor(kBlack);
    ret_histo->SetMarkerStyle(21);
    
    //Set directory
    ret_histo->SetDirectory(gROOT);
    
    return ret_histo;
} //end getHistogram()

//For some number converted to a string, this adjusts the number of sig figs
/*void adjustSigFigs(string &strInput){
    //Variable Declaration
    size_t iPos = strInput.find(".");
    
    if (iPos != std::string::npos) {
        if ( (strInput.length() - iPos) > 3 ) {
            strInput.erase(iPos + 3, strInput.length() - (iPos + 3) );
        }
    }
    
    return;
}*/ //end adjustSigFigs

//Ensures numbers use 2 sig figs and rounding is done correctly
string adjustSigFigs(float fInput, bool bAbsVal){
    fInput = std::round(fInput * 100);
    fInput = fInput/100;
    
    if (bAbsVal) {
        fInput = fabs(fInput);
    }
    
    return getString(fInput);
} //end adjustSigFigs

//Need name
//Need title
TCanvas * getCanvas(string strInputName_Canv, string strInputName_Func, string strInputName_Histo, TFile *file_Input){
    //Variable Declaration
    int iWidth = 800;
    //int iWidth = 600;
    int iHeight= 600;
    //int iHeight= 800;
    
    // references for Top, Bot, Left, Right
    float fTop  = 0.08 * iHeight;
    float fBot  = 0.12 * iHeight;
    float fLf   = 0.12 * iWidth;
    float fRt   = 0.04 * iWidth;
    
    //Set Canvas
    TCanvas * ret_canvas = new TCanvas(strInputName_Canv.c_str(), strInputName_Canv.c_str(), iWidth, iHeight);
    ret_canvas->SetFillColor(0);
    ret_canvas->SetBorderMode(0);
    ret_canvas->SetFrameFillStyle(0);
    ret_canvas->SetFrameBorderMode(0);
    ret_canvas->SetLeftMargin( fLf / iWidth );
    ret_canvas->SetRightMargin( fRt / iWidth );
    ret_canvas->SetTopMargin( fTop / iHeight );
    ret_canvas->SetBottomMargin( fBot / iHeight );
    ret_canvas->SetTickx(0);
    ret_canvas->SetTicky(0);
    
    //Get & Draw Histogram
    TH1F *hTimingHisto = getHistogram(strInputName_Histo,file_Input);
    
    ret_canvas->cd();
    hTimingHisto->Draw("ex0");
    
    //Get & Draw Function
    TF1 *func_TimingFunc = getFunction(strInputName_Func,file_Input);
    
    ret_canvas->cd();
    func_TimingFunc->Draw("same");
    
    //Legend
    float fX0, fX1, fdX;    fX1 = 0.92;  fdX = 0.3;  fX0 = fX1 - fdX;
    float fY0, fY1, fdY;    fY1 = 0.6;  fdY = 0.18;  fY0 = fY1 - fdY;
    TLegend *leg_timing = new TLegend(fX0,fY0,fX1,fY1);
    leg_timing->AddEntry(hTimingHisto, "Data", "lp");
    leg_timing->AddEntry(func_TimingFunc,"Fit", "l" );
    
    ret_canvas->cd();
    leg_timing->Draw("same");
    
    //TLatex
    float fTextSize = 0.04;
    
    string strMean      = adjustSigFigs( func_TimingFunc->GetParameter(1), true );
    string strMean_Err  = adjustSigFigs( func_TimingFunc->GetParError(1), true );
    string strSigma     = adjustSigFigs( func_TimingFunc->GetParameter(2), true );
    string strSigma_Err = adjustSigFigs( func_TimingFunc->GetParError(2), true );
    
    TLatex *latex_Det = new TLatex();       latex_Det->SetTextSize(fTextSize);
    TLatex *latex_Beam = new TLatex();      latex_Beam->SetTextSize(fTextSize);
    TLatex *latex_Thresh = new TLatex();    latex_Thresh->SetTextSize(fTextSize);
    TLatex *latex_Gas = new TLatex();       latex_Gas->SetTextSize(fTextSize);
    TLatex *latex_HVSetPt = new TLatex();   latex_HVSetPt->SetTextSize(fTextSize);
    TLatex *latex_TimeRes = new TLatex();   latex_TimeRes->SetTextSize(fTextSize);
    TLatex *latex_Sigma = new TLatex();     latex_Sigma->SetTextSize(fTextSize);
    
    latex_Det->DrawLatexNDC(fX0, fY1 + 7 * fTextSize, strInputName_Canv.c_str() );
    latex_Beam->DrawLatexNDC(fX0, fY1 + 6 * fTextSize, "#mu-Beam" );
    latex_Thresh->DrawLatexNDC(fX0, fY1 + 5 * fTextSize, "Theshold: 2 fC");
    latex_Gas->DrawLatexNDC(fX0, fY1 + 4 * fTextSize, "Gas: Ar/CO2/CF4 (45/15/40");
    latex_HVSetPt->DrawLatexNDC(fX0, fY1 + 3 * fTextSize, "SC HV Setpoint: 810#muA");
    latex_TimeRes->DrawLatexNDC(fX0,fY1+2 * fTextSize, "Time Resolution");
    latex_Sigma->DrawLatexNDC(fX0,fY1+ 1 * fTextSize, ("#sigma_{t} = " + strSigma + "#pm" + strSigma_Err + " ns").c_str() );
    
    //Write the lumi info and the CMS "logo"
    CMS_lumi( ret_canvas, 0, 11);
    
    //Update
    ret_canvas->Update();
    ret_canvas->RedrawAxis();
    //ret_canvas->GetFrame()->Draw();
    
    //Print
    ret_canvas->Print( ("figures/" + strInputName_Canv + ".pdf").c_str(),".pdf");
    ret_canvas->Print( ("figures/" + strInputName_Canv + ".png").c_str(),".png");
    
    return ret_canvas;
} //End getCanvas()

void singleRunPlotter(){
    //Load the style
    setTDRStyle();
    
    //Variable Declaration
    writeExtraText  = true;
    extraText       = "Preliminary";

    TFile * file_InputRun = new TFile("singleRunAnalyzer_Output_WithSel.root","READ",0,1);
    
    //Individual Plots
    TCanvas *canvas_AND = getCanvas("Detector And","Detector_AND/func_Timing_AND_R0","Detector_AND/timingHisto_AND_R0", file_InputRun);
    TCanvas *canvas_GE11_1 = getCanvas("GE11-VI-L-CERN-0001", "GE11-VI-L-CERN-0001/func_Timing_GE11_1_R0", "GE11-VI-L-CERN-0001/timingHisto_GE11_1_R0", file_InputRun);
    TCanvas *canvas_GE11_2 = getCanvas("GE11-VI-L-CERN-0002", "GE11-VI-L-CERN-0002/func_Timing_GE11_2_R0", "GE11-VI-L-CERN-0002/timingHisto_GE11_2_R0", file_InputRun);
    TCanvas *canvas_OR = getCanvas("Detector Or","Detector_OR/func_Timing_OR_R0","Detector_OR/timingHisto_OR_R0", file_InputRun);
    
    //Summary Plot
    int iHeight = 600;
    int iWidth = 800;
    
    float fTop  = 0.08 * iHeight;
    float fBot  = 0.12 * iHeight;
    float fLf   = 0.12 * iWidth;
    float fRt   = 0.04 * iWidth;
    
    //Set Canvas
    TCanvas *canvas_Summary = new TCanvas("canvas_Summary","Summary",800,600);
    canvas_Summary->SetFillColor(0);
    canvas_Summary->SetBorderMode(0);
    canvas_Summary->SetFrameFillStyle(0);
    canvas_Summary->SetFrameBorderMode(0);
    canvas_Summary->SetLeftMargin( fLf / iWidth );
    canvas_Summary->SetRightMargin( fRt / iWidth );
    canvas_Summary->SetTopMargin( fTop / iHeight );
    canvas_Summary->SetBottomMargin( fBot / iHeight );
    //canvas_Summary->SetTickx(0);
    //canvas_Summary->SetTicky(0);
    
    TF1 *func_AND = getFunction("Detector_AND/func_Timing_AND_R0", file_InputRun);              func_AND->SetName("AND");
    TF1 *func_GE11_1 = getFunction("GE11-VI-L-CERN-0001/func_Timing_GE11_1_R0", file_InputRun); func_GE11_1->SetName("GE11_1");
    TF1 *func_GE11_2 = getFunction("GE11-VI-L-CERN-0002/func_Timing_GE11_2_R0", file_InputRun); func_GE11_2->SetName("GE11_2");
    TF1 *func_OR = getFunction("Detector_OR/func_Timing_OR_R0", file_InputRun);                 func_OR->SetName("OR");
    
    TH1F *hXAxis = new TH1F("hXAxis","",6,4,10);
    hXAxis->GetXaxis()->SetNdivisions(10,10,0);
    hXAxis->GetXaxis()->SetTitle("time resolution #left(ns#right)");
    hXAxis->GetYaxis()->SetLabelColor(kWhite);
    hXAxis->GetYaxis()->SetRangeUser(0.,5);
    hXAxis->GetYaxis()->SetTickLength(0.);
    hXAxis->SetDirectory(gROOT);
    
    TGraphErrors *graph_Summary = new TGraphErrors(4);
    graph_Summary->SetPoint(0,fabs( func_GE11_1->GetParameter(2) ), 4); graph_Summary->SetPointError(0, func_GE11_1->GetParError(2), 0 );
    graph_Summary->SetPoint(1,fabs( func_GE11_2->GetParameter(2) ), 3); graph_Summary->SetPointError(1, func_GE11_2->GetParError(2), 0 );
    graph_Summary->SetPoint(2,fabs( func_OR->GetParameter(2) ), 2);     graph_Summary->SetPointError(2, func_OR->GetParError(2), 0 );
    graph_Summary->SetPoint(3,fabs( func_AND->GetParameter(2) ), 1);    graph_Summary->SetPointError(3, func_AND->GetParError(2), 0 );
    
    graph_Summary->SetLineColor(kBlack);
    graph_Summary->SetLineWidth(3);
    graph_Summary->SetMarkerColor(kBlack);
    graph_Summary->SetMarkerStyle(21);
    
    hXAxis->Draw();
    graph_Summary->Draw("samePE1");
    
    float fTextSize = 0.05;
    TLatex *latex_GE11_1 = new TLatex();  latex_GE11_1->SetTextSize(fTextSize);
    TLatex *latex_GE11_2 = new TLatex();  latex_GE11_2->SetTextSize(fTextSize);
    TLatex *latex_AND = new TLatex();  latex_AND->SetTextSize(fTextSize);
    TLatex *latex_OR = new TLatex();  latex_OR->SetTextSize(fTextSize);
    
    latex_GE11_1->DrawLatex(4.1,3.9,"GE1/1-VI-L-CERN-0001:");
    latex_GE11_2->DrawLatex(4.1,2.9,"GE1/1-VI-L-CERN-0002:");
    latex_OR->DrawLatex(4.1,1.9,"Detector Or:");
    latex_AND->DrawLatex(4.1,0.9,"Detector And:");
    
    TLine *line_1 = new TLine(hXAxis->GetBinLowEdge(1), 1.5, hXAxis->GetBinLowEdge(hXAxis->GetNbinsX()+1), 1.5);
    TLine *line_2 = new TLine(hXAxis->GetBinLowEdge(1), 2.5, hXAxis->GetBinLowEdge(hXAxis->GetNbinsX()+1), 2.5);
    TLine *line_3 = new TLine(hXAxis->GetBinLowEdge(1), 3.5, hXAxis->GetBinLowEdge(hXAxis->GetNbinsX()+1), 3.5);
    
    line_1->Draw("same");
    line_2->Draw("same");
    line_3->Draw("same");
    
    CMS_lumi( canvas_Summary, 0, 0);
    
    //Update
    canvas_Summary->Update();
    canvas_Summary->RedrawAxis();
    
    file_InputRun->Close();
    
    return;
} //End singleRunPlotter