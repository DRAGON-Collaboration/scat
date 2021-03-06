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

	const TString col[] = {"1", "2", "3", "5", "7"};
	const TString deg[] = {"22.5", "25", "35", "40", "45", "55", "60", "65", "75", "90", "120"};
	const TString sls = "mm/";
	const TString evn = "deg/Events0.root";

	for (int p=0;p<5;p++)
	{
		for (int q=0;q<11;q++)
		{
			TString str = "/local/astro/scat/SONIK_He3/1.5MeV/";
			str.Append(col[p]);
			str.Append(sls);
			str.Append(deg[q]);
			str.Append(evn);

			f = new TFile(str);
			t = (TTree*)f->Get("Ejectile Detector Events");

			y[q] = (Double_t) t->GetEntries() / 10.;
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

	TCanvas *c1 = new TCanvas("c1","Counts per 1000p-nA", 1);
	c1->SetFillColor(42);
	c1->SetGrid();
	//c1->GetFrame()->SetFillColor(21);
	//c1->GetFrame()->SetBorderSize(12);
	//c1.SetLogy();	
	
	gr[4]->SetLineWidth(2);
	gr[4]->SetLineColor(12);
	gr[4]->SetMarkerColor(9);
	gr[4]->SetMarkerStyle(21);
	gr[4]->Draw("ALP");
	for (int p=0;p<4;p++)
	{
		gr[p]->SetLineWidth(2);
		gr[p]->SetLineColor(12);
		gr[p]->SetMarkerColor(5+p);
		gr[p]->SetMarkerStyle(21);
		gr[p]->Draw("LP");
	}

	c1->Update();
}

	 	

