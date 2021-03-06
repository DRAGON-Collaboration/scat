#include <iostream>

#include <TObject.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TH1F.h>
#include <TH1.h>
#include <TH3F.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TString.h>

void run()
{
	TFile *f;
	TTree *t;

	TGraphErrors *gr[5];	

	Int_t n = 11;
	Double_t x[11] = {22.5, 25., 35., 40., 45., 55., 60., 65., 75., 90., 120.};
	Double_t ex[11] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., };
	Double_t y[11];
	Double_t ey[11];

	const TString col[] = {"1", "2", "3", "5", "5"};
	const TString deg[] = {"22.5", "25", "35", "40", "45", "55", "60", "65", "75", "90", "120"};
	const TString sls = "mm_10k_pnA/";
	const TString evn = "deg/Events0.root";

	for (int p=0;p<5;p++)
	{
		for (int q=0;q<11;q++)
		{
			TString str = "/local/astro/scat/SONIK_He3/0.5MeV/";
			str.Append(col[p]);
			str.Append(sls);

			//if (p == 0) { str = "/local/astro/scat/SONIK_He3/0.5MeV/1mm_50k_pnA/";}

			str.Append(deg[q]);
			str.Append(evn);

			f = new TFile(str);
			t = (TTree*)f->Get("Ejectile Detector Events");

			y[q] = (Double_t) t->GetEntries();
			//if (p == 0) {y[q] = y[q] /5.;}
			if (y[q]==0.)
			{
				ey[q] = 1;
			}
			else
			{
				ey[q] = sqrt(y[q]);
			}
		}
		gr[p] = new TGraphErrors(n,x,y,ex,ey);
	}

	TCanvas *c2 = new TCanvas("c2","Counts per 10000p-nA", 1);
	c2->SetFillColor(10);
	c2->SetGrid();
	//c2->GetFrame()->SetFillColor(21);
	//c2->GetFrame()->SetBorderSize(12);
	c2->SetLogy();	
	
	TLegend *leg = new TLegend(0.4,0.6,0.89,0.89);	
	
	gr[4]->SetLineWidth(2);
	gr[4]->SetLineColor(4);
	gr[4]->SetLineStyle(2);
	gr[4]->SetMarkerColor(4);
	gr[4]->SetMarkerStyle(21);
	gr[4]->Draw("ALP");
	gr[4]->SetTitle("Counts per 10000p-nA");
	gr[4]->GetXaxis()->SetRangeUser(0.,180.);
	gr[4]->GetXaxis()->SetTitle("#theta_{cm} (deg)");
	gr[4]->GetYaxis()->SetTitle("");//("d#sigma/d#Omega (mb/sr)");
	gr[4]->GetXaxis()->CenterTitle();
	gr[4]->GetYaxis()->CenterTitle();
	for (int p=0;p<4;p++)
	{
		gr[p]->SetLineWidth(2);
		gr[p]->SetLineColor(6+p);
		gr[p]->SetLineStyle(2);
		gr[p]->SetMarkerColor(6+p);
		gr[p]->SetMarkerStyle(21);
		gr[p]->Draw("LP");
	}	
	leg->AddEntry(gr[0],"1mm Collimator","lpf");
	leg->AddEntry(gr[1],"2mm Collimator","lpf");
	leg->AddEntry(gr[2],"3mm Collimator","lpf");
	leg->AddEntry(gr[3],"5mm Collimator","lpf");
	leg->AddEntry(gr[4],"7mm Collimator","lpf");
	leg->Draw(); 

	c2->Update();
}

	 	

