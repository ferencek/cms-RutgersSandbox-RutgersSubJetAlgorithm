#ifndef RUTGERSSUBJETPRODUCER_H
#define RUTGERSSUBJETPRODUCER_H

// Package:    JetProducers
// Class:      RutgersSubJetProducer
// 
/**\class RutgersSubJetProducer RutgersSubJetProducer.h RecoJets/JetProducers/plugins/RutgersSubJetProducer.h

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/

#include "RecoJets/JetProducers/plugins/CompoundJetProducer.h"
#include "RutgersSandbox/RutgersSubJetAlgorithm/interface/RutgersSubJetAlgorithm.h"

namespace cms
{
  class RutgersSubJetProducer : public CompoundJetProducer
  {
    public:

      RutgersSubJetProducer(const edm::ParameterSet& iConfig);
      virtual ~RutgersSubJetProducer() {}

      virtual void produce( edm::Event& iEvent, const edm::EventSetup& iSetup );
      virtual void runAlgorithm( edm::Event& iEvent, const edm::EventSetup& iSetup );

    private:
      RutgersSubJetAlgorithm  alg_;
  };
}

#endif