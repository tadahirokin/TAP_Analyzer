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
    TH1I *histCh1 = new TH1I("histCh1", "Ch1_histogram_all", 1024, 0, 1025);
    histCh1->GetXaxis()->SetTitle("Ch1 Pulse Height");
    TH1I *histCh2 = new TH1I("histCh2", "Ch2_histogram_all", 1024, 0, 1025);
    histCh2->GetXaxis()->SetTitle("Ch2 Pulse Height");
    TH1I *histCh3 = new TH1I("histCh3", "Ch3_histogram_all", 1024, 0, 1025);
    histCh3->GetXaxis()->SetTitle("Ch3 Pulse Height");

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

    nHits = 0;
    for (int i = 0; i < 4; i++)
    {
        pha[i] = 0;
        time[i] = 0;
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
        }
        else
        {
            cout << nHits << " " << pha[0] << " " << pha[1] << " " << pha[2] << " " << pha[3] << " " << time[0] << " " << time[1] << " " << time[2] << " " << time[3] << endl;

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

    tree->Write();
}