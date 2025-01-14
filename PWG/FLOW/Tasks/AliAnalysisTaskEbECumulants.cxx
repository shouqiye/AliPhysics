/*************************************************************************
* Copyright(c) 1998-2008, ALICE Experiment at CERN, All rights reserved. *
*                                                                        *
* Author: The ALICE Off-line Project.                                    *
* Contributors are mentioned in the code where appropriate.              *
*                                                                        *
* Permission to use, copy, modify and distribute this software and its   *
* documentation strictly for non-commercial purposes is hereby granted   *
* without fee, provided that the above copyright notice appears in all   *
* copies and that both the copyright notice and this permission notice   *
* appear in the supporting documentation. The authors make no claims     *
* about the suitability of this software for any purpose. It is          *
* provided "as is" without express or implied warranty.                  * 
**************************************************************************/

/************************************** 
* template class for student projects * 
**************************************/ 
  
#include "Riostream.h"
#include "AliAnalysisTaskEbECumulants.h"
#include "AliLog.h"
#include "AliAODEvent.h"
#include "AliAODInputHandler.h"
#include "TFile.h"

using std::cout;
using std::endl;

ClassImp(AliAnalysisTaskEbECumulants)

//================================================================================================================

AliAnalysisTaskEbECumulants::AliAnalysisTaskEbECumulants(const char *name): 
 AliAnalysisTaskSE(name), 
 fHistList(NULL),
 // Control histograms:
 fControlHistogramsList(NULL),
 fPtHist(NULL),
 fNbinsPt(1000),
 fMinBinPt(0.),
 fMaxBinPt(10.),
 // Final results:
 fFinalResultsList(NULL)
 {
  // Constructor.
 
  AliDebug(2,"AliAnalysisTaskEbECumulants::AliAnalysisTaskEbECumulants(const char *name)");

  // Base list:
  fHistList = new TList();
  fHistList->SetName("outputEbECumulantsAnalysis");
  fHistList->SetOwner(kTRUE);

  // Initialize all arrays:
  this->InitializeArrays();

  // Define input and output slots here. 
  // Below is the old example, now obsolete.

  // Input slot #0 works with an AliFlowEventSimple
  //DefineInput(0, AliFlowEventSimple::Class());  
  // Input slot #1 is needed for the weights input file:
  //if(useParticleWeights)
  //{
  // DefineInput(1, TList::Class());   
  //}  
  // Output slot #0 is reserved              
  // Output slot #1 writes into a TList container

  DefineOutput(1, TList::Class());  

} // AliAnalysisTaskEbECumulants::AliAnalysisTaskEbECumulants(): 

//================================================================================================================

AliAnalysisTaskEbECumulants::AliAnalysisTaskEbECumulants(): 
 AliAnalysisTaskSE(),
 fHistList(NULL),
 // Control histograms:
 fControlHistogramsList(NULL),
 fPtHist(NULL),
 fNbinsPt(1000),
 fMinBinPt(0.),
 fMaxBinPt(10.),
 // Final results:
 fFinalResultsList(NULL)
{
  // Dummy constructor.
 
  AliDebug(2,"AliAnalysisTaskEbECumulants::AliAnalysisTaskEbECumulants()");

} // AliAnalysisTaskEbECumulants::AliAnalysisTaskEbECumulants():

//================================================================================================================

AliAnalysisTaskEbECumulants::~AliAnalysisTaskEbECumulants()
{
 // Destructor.

 if(fHistList) delete fHistList;
  
} // AliAnalysisTaskEbECumulants::~AliAnalysisTaskEbECumulants()

//================================================================================================================

void AliAnalysisTaskEbECumulants::UserCreateOutputObjects() 
{
 // Called at every worker node to initialize.

 // a) Trick to avoid name clashes, part 1;
 // b) Book and nest all lists;
 // c) Book all objects;
 // *) Trick to avoid name clashes, part 2.
  
 // a) Trick to avoid name clashes, part 1:
 Bool_t oldHistAddStatus = TH1::AddDirectoryStatus(); 
 TH1::AddDirectory(kFALSE);

 // b) Book and nest all lists:
 this->BookAndNestAllLists();

 // c) Book all objects:
 this->BookControlHistograms();
 this->BookFinalResultsHistograms();

 // *) Trick to avoid name clashes, part 2:
 TH1::AddDirectory(oldHistAddStatus);

 PostData(1,fHistList);

} // void AliAnalysisTaskEbECumulants::UserCreateOutputObjects() 

//================================================================================================================

void AliAnalysisTaskEbECumulants::UserExec(Option_t *) 
{
 // Main loop (called for each event).
 // a) Get pointer to AOD event:
 // b) Start analysis over AODs;
 // c) Reset event-by-event objects;
 // d) PostData.

 // a) Get pointer to AOD event:
 AliAODEvent *aAOD = dynamic_cast<AliAODEvent*>(InputEvent()); // from TaskSE
 if(!aAOD){return;}

 // b) Start analysis over AODs:
 Int_t nTracks = aAOD->GetNumberOfTracks(); // number of all tracks in current event 
 for(Int_t iTrack=0;iTrack<nTracks;iTrack++) // starting a loop over all tracks
 {
  AliAODTrack *aTrack = dynamic_cast<AliAODTrack*>(aAOD->GetTrack(iTrack)); // getting a pointer to a track
  if(!aTrack){continue;} // protection against NULL pointers
  if(!aTrack->TestFilterBit(128)){continue;} // filter bit 128 denotes TPC-only tracks

  // example variables for each track: (for more options, please see class STEER/AOD/AliAODTrack.h)  
  Double_t pt = aTrack->Pt(); // Pt
  //Double_t px = aTrack->Px(); // x-component of momenta
  //Double_t py = aTrack->Py(); // y-component of momenta
  //Double_t pz = aTrack->Pz(); // z-component of momenta
  //Double_t e = aTrack->E();  // energy
  //Double_t phi = aTrack->Phi(); // azimuthal angle
  //Double_t eta = aTrack->Eta(); // pseudorapidity
  //Double_t charge = aTrack->Charge(); // charge
 
  // apply some cuts: e.g. take for the analysis only particles in 0.2 < pT < 5.0 GeV
  if( (0.2 < pt) && (pt < 5.0) ) // example cuts
  {
   // fill some control histograms:
   fPtHist->Fill(pt); // filling pt distribution
 
   // do some analysis only with the particles which passed the cuts
   // ... your analysis code ... 

  } // if( (0.2 < pT) && (pT < 5.0) )

 } // for(Int_t iTrack=0;iTrack<nTracks;iTrack++) // starting a loop over all tracks
  
 // c) Reset event-by-event objects:
 // ...

 // d) PostData:
 PostData(1,fHistList);

} // void AliAnalysisTaskEbECumulants::UserExec(Option_t *)

//================================================================================================================

void AliAnalysisTaskEbECumulants::Terminate(Option_t *)
{
 // Accessing the merged output list.

 fHistList = (TList*)GetOutputData(1);
 if(!fHistList){exit(1);}

 // Do some calculation in offline mode here:

 // ... your code for offline calculations ...


} // end of void AliAnalysisTaskEbECumulants::Terminate(Option_t *)

//================================================================================================================

void AliAnalysisTaskEbECumulants::InitializeArrays()
{
 // Initialize all data members which are arrays in this method.

 // This is important, since these objects cannot be initialized directly in the constructor list. 

} // void AliAnalysisTaskEbECumulants::InitializeArrays()

//=======================================================================================================================

void AliAnalysisTaskEbECumulants::BookAndNestAllLists()
{
 // Book and nest all lists nested in the base list fHistList.

 // a) Book and nest lists for control histograms;
 // b) Book and nest lists for final results.

 TString sMethodName = "void AliAnalysisTaskEbECumulants::BookAndNestAllLists()";
 if(!fHistList){Fatal(sMethodName.Data(),"fHistList is NULL");}

 // a) Book and nest lists for control histograms:
 fControlHistogramsList = new TList();
 fControlHistogramsList->SetName("ControlHistograms");
 fControlHistogramsList->SetOwner(kTRUE);
 fHistList->Add(fControlHistogramsList);

 // b) Book and nest lists for final results:
 fFinalResultsList = new TList();
 fFinalResultsList->SetName("FinalResults");
 fFinalResultsList->SetOwner(kTRUE);
 fHistList->Add(fFinalResultsList);

} // void AliAnalysisTaskEbECumulants::BookAndNestAllLists()

//=======================================================================================================================

void AliAnalysisTaskEbECumulants::BookControlHistograms()
{
 // Book all control histograms.

 // a) Book histogram to hold pt spectra;
 // b) ...

 // a) Book histogram to hold pt spectra:
 fPtHist = new TH1F("fPtHist","atrack->Pt()",fNbinsPt,fMinBinPt,fMaxBinPt);
 fPtHist->SetStats(kFALSE);
 fPtHist->SetFillColor(kBlue-10);
 fPtHist->GetXaxis()->SetTitle("p_{t}");
 fControlHistogramsList->Add(fPtHist);
 
 // b) ...

} // void AliAnalysisTaskEbECumulants::BookControlHistograms()

//=======================================================================================================================

void AliAnalysisTaskEbECumulants::BookFinalResultsHistograms()
{
 // Book all histograms to hold the final results.

} // void AliAnalysisTaskEbECumulants::BookFinalResultsHistograms()

//=======================================================================================================================

