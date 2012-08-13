#ifndef RUTGERSSUBJETALGORITHM_H
#define RUTGERSSUBJETALGORITHM_H

// Package:    RutgersSubJetAlgorithm
// Class:      RutgersSubJetAlgorithm
//
/**\class RutgersSubJetAlgorithm RutgersSubJetAlgorithm.h RutgersSandbox/RutgersSubJetAlgorithm/interface/RutgersSubJetAlgorithm.h

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Dinko Ferencek
//         Created:  Thu Aug  9 16:02:58 CDT 2012
// $Id: RutgersSubJetAlgorithm.h,v 1.1 2012/08/11 03:48:36 ferencek Exp $
//
// 

#include <vector>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "RecoJets/JetAlgorithms/interface/CompoundPseudoJet.h"

#include "fastjet/JetDefinition.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/GhostedAreaSpec.hh"

class RutgersSubJetAlgorithm
{
      //
      // construction / destruction
      //
   public:
      RutgersSubJetAlgorithm(const edm::ParameterSet& iConfig);
      virtual ~RutgersSubJetAlgorithm();

      // typedefs
      typedef boost::shared_ptr<fastjet::ClusterSequence>        ClusterSequencePtr;
      typedef boost::shared_ptr<fastjet::JetDefinition>          JetDefPtr;
      typedef boost::shared_ptr<fastjet::GhostedAreaSpec>        ActiveAreaSpecPtr;
      typedef boost::shared_ptr<fastjet::AreaDefinition>         AreaDefinitionPtr;
      typedef boost::shared_ptr<fastjet::RangeDefinition>        RangeDefPtr;

      //
      // member functions
      //
      void run(const std::vector<fastjet::PseudoJet>& fjInputs,
	       std::vector<CompoundPseudoJet>& compoundJets);

      //
      // member data
      //
   private:
      const std::string        jetAlgorithm_;
      const std::string        jetReclusterAlgorithm_;
      const double             rParam_;
      const unsigned           nSubjets_;
      const double             jetPtMin_;
      const bool               doAreaFastjet_;
      bool                     useExplicitGhosts_;
      double                   voronoiRfact_;

      ClusterSequencePtr       fjClusterSeq_;
      ClusterSequencePtr       fjReclusterSeq_;
      JetDefPtr                fjJetDefinition_;
      JetDefPtr                fjJetReclusterDefinition_;
      ActiveAreaSpecPtr        fjActiveArea_;
      AreaDefinitionPtr        fjAreaDefinition_;
      AreaDefinitionPtr        fjReclusterAreaDefinition_;
};

#endif
