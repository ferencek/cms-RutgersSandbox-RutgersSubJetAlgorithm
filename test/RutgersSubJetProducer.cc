#include "FWCore/Framework/interface/MakerMacros.h"
#include "RecoJets/JetProducers/plugins/RutgersSubJetProducer.h"

using namespace cms;
using namespace reco;

//
// constructors
//
RutgersSubJetProducer::RutgersSubJetProducer(const edm::ParameterSet& iConfig)
  : CompoundJetProducer(iConfig)
  , alg_(iConfig)
{

}

//
// member functions
//
void RutgersSubJetProducer::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  CompoundJetProducer::produce(iEvent, iSetup);
}

void RutgersSubJetProducer::runAlgorithm( edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  alg_.run( fjInputs_, fjCompoundJets_ );
}

//define this as a plug-in
DEFINE_FWK_MODULE(RutgersSubJetProducer);
