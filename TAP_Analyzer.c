#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <regex>
#include <array>
#include <TStyle.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TCanvas.h>
#include <TText.h>
#include <TPaveText.h>
#include <TPaletteAxis.h>
#include <TPaveStats.h>
#include <TCut.h>
#include <TFile.h>
#include <TTree.h>

#define FILE_NAME "list000003.csv"
#define COIN_WINDOW 1000 // in unit of nano second

vector<string>
split(string &input, char delimiter)
{
    istringstream stream(input);
    string field;
    vector<string> result;
    while (getline(stream, field, delimiter))
    {
        result.push_back(field);
    }
    return result;
}

void TAP_Analyzer()
{
    TH1I *hist[4];
    hist[0] = new TH1I("histCh0", "Ch0_histogram_all", 1024, 0, 1025);
    hist[0]->GetXaxis()->SetTitle("Ch0 Pulse Height");
    hist[1] = new TH1I("histCh1", "Ch1_histogram_all", 1024, 0, 1025);
    hist[1]->GetXaxis()->SetTitle("Ch1 Pulse Height");
    hist[2] = new TH1I("histCh2", "Ch2_histogram_all", 1024, 0, 1025);
    hist[2]->GetXaxis()->SetTitle("Ch2 Pulse Height");
    hist[3] = new TH1I("histCh3", "Ch3_histogram_all", 1024, 0, 1025);
    hist[3]->GetXaxis()->SetTitle("Ch3 Pulse Height");

    TH3I *histCh123 = new TH3I("histCh123", "Only_Ch1_and_2_and_3_histogram", 1024, 0, 1025, 1024, 0, 1025, 1024, 0, 1025);
    histCh123->GetXaxis()->SetTitle("Ch1 Pulse Height");
    histCh123->GetYaxis()->SetTitle("Ch2 Pulse Height");
    histCh123->GetZaxis()->SetTitle("Ch3 Pulse Height");

    int pha[4];
    Long64_t time[4];
    int nHits;
    // int ch1TimeDiff, ch2TimeDiff, ch3TimeDiff;

    TFile *file = new TFile("output.root", "recreate");
    TTree *tree = new TTree("tree", "tree");

    tree->Branch("pha", &pha[4]);
    tree->Branch("time", &time[4]);
    tree->Branch("nHits", &nHits);

    ifstream ifs(FILE_NAME, std::ios::binary);
    if (!ifs)
    {
        cout << "File does not exist." << endl;
    }

    string line;
    Long64_t currTime;
    Long64_t prevTime = 0;
    int nCh;
    int tempPha;

    int noOfNHits[4];

    nHits = 0;
    for (int i = 0; i < 4; i++)
    {
        pha[i] = 0;
        time[i] = 0;
        noOfNHits[i] = 0;
    }

    while (getline(ifs, line))
    {
        vector<string> splitString = split(line, ',');
        currTime = stol(splitString[0]);
        nCh = stoi(splitString[1]);
        tempPha = stoi(splitString[2]);
        // cout << nHits << " " << currTime << " " << nCh << " " << tempPha << endl;

        if (currTime - prevTime < COIN_WINDOW)
        {
            nHits++;
            pha[nCh] = tempPha;
            time[nCh] = currTime;
            prevTime = currTime;
            hist[nCh]->Fill(tempPha);
        }
        else
        {
            // cout << nHits << " " << pha[0] << " " << pha[1] << " " << pha[2] << " " << pha[3] << " " << time[0] << " " << time[1] << " " << time[2] << " " << time[3] << endl;
            if (nHits == 3)
            {
                histCh123->Fill(pha[1], pha[2], pha[3]);
            }
            noOfNHits[nHits]++;
            tree->Fill();
            nHits = 1;
            for (int i = 0; i < 4; i++)
            {
                pha[i] = 0;
                time[i] = 0;
            }
            pha[nCh] = tempPha;
            time[nCh] = currTime;
            prevTime = currTime;
        }
    }

    int nEvent = tree->GetEntries();
    cout << "Number of events is " << nEvent << "." << endl;
    cout << "Single-hit event: " << noOfNHits[1] << " (" << noOfNHits[1] / (currTime * 1.E-9) * 60. << " cpm)" << endl;
    cout << "Double-hit event: " << noOfNHits[2] << " (" << noOfNHits[2] / (currTime * 1.E-9) * 60. << " cpm)" << endl;
    cout << "Triple-hit event: " << noOfNHits[3] << " (" << noOfNHits[3] / (currTime * 1.E-9) * 60. << " cpm)" << endl;

    TCanvas *cSingleHist = new TCanvas("cSingleHist", "Single_Ch_Histograms", 700, 1500);
    cSingleHist->Divide(1, 3);
    cSingleHist->cd(1);
    hist[1]->Draw("hist");
    cSingleHist->cd(2);
    hist[2]->Draw("hist");
    cSingleHist->cd(3);
    hist[3]->Draw("hist");

    TCanvas *cThreeHitsHist = new TCanvas("cThreeHitsHist", "Triple_Coincidence_Histgram", 700, 500);
    // histCh123->Draw("box");
    TH3I *rebinHistCh123 = (TH3I *)histCh123->Rebin3D(32, 32, 32, "rebinHistCh123");
    rebinHistCh123->Draw("box");

    TCanvas *c3HitsHistProjection = new TCanvas("c3HitsHistProjection", "Projection_of_Triple_Coincidence_Histogram", 2100, 450);
    c3HitsHistProjection->Divide(3, 1);
    c3HitsHistProjection->cd(1);
    TH2I *projectionXY = (TH2I *)histCh123->Project3D("xy");
    TH2I *projectionXYrebin = (TH2I *)projectionXY->Rebin2D(32, 32, "projectionXYrebin");
    projectionXYrebin->Draw("colz");
    c3HitsHistProjection->cd(2);
    TH2I *projectionYZ = (TH2I *)histCh123->Project3D("yz");
    TH2I *projectionYZrebin = (TH2I *)projectionYZ->Rebin2D(32, 32, "projectionYZrebin");
    projectionYZrebin->Draw("colz");
    c3HitsHistProjection->cd(3);
    TH2I *projectionZX = (TH2I *)histCh123->Project3D("zx");
    TH2I *projectionZXrebin = (TH2I *)projectionZX->Rebin2D(32, 32, "projectionZXrebin");
    projectionZXrebin->Draw("colz");

    tree->Write();
    file->Close();
}