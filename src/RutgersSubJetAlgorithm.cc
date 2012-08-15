#include "RutgersSandbox/RutgersSubJetAlgorithm/interface/RutgersSubJetAlgorithm.h"

//
// constructors and destructor
//_____________________________________________________________________________
RutgersSubJetAlgorithm::RutgersSubJetAlgorithm(const edm::ParameterSet& iConfig)
  : jetAlgorithm_            (iConfig.getParameter<std::string>  ("jetAlgorithm"))
  , jetReclusterAlgorithm_   (iConfig.getParameter<std::string>  ("jetReclusterAlgorithm"))
  , rParam_                  (iConfig.getParameter<double>       ("rParam"))
  , nSubjets_                (iConfig.getParameter<unsigned>     ("nSubjets"))
  , jetPtMin_                (iConfig.getParameter<double>       ("jetPtMin"))
  , doAreaFastjet_           (iConfig.getParameter<bool>         ("doAreaFastjet"))
  , useExplicitGhosts_       (false)
  , voronoiRfact_            (-9)
{
  if (jetAlgorithm_=="Kt")
    fjJetDefinition_= JetDefPtr( new fastjet::JetDefinition(fastjet::kt_algorithm, rParam_) );
  else if (jetAlgorithm_=="CambridgeAachen")
    fjJetDefinition_= JetDefPtr( new fastjet::JetDefinition(fastjet::cambridge_algorithm, rParam_) );
  else if (jetAlgorithm_=="AntiKt")
    fjJetDefinition_= JetDefPtr( new fastjet::JetDefinition(fastjet::antikt_algorithm, rParam_) );
  else
    throw cms::Exception("Invalid jetAlgorithm")
      <<"Jet algorithm for RutgersSubJetProducer is invalid: "
      <<jetAlgorithm_<<", use CambridgeAachen | Kt | AntiKt"<<std::endl;

  if (jetReclusterAlgorithm_=="Kt")
    fjJetReclusterDefinition_= JetDefPtr( new fastjet::JetDefinition(fastjet::kt_algorithm, rParam_) );
  else if (jetReclusterAlgorithm_=="CambridgeAachen")
    fjJetReclusterDefinition_= JetDefPtr( new fastjet::JetDefinition(fastjet::cambridge_algorithm, rParam_) );
  else if (jetReclusterAlgorithm_=="AntiKt")
    fjJetReclusterDefinition_= JetDefPtr( new fastjet::JetDefinition(fastjet::antikt_algorithm, rParam_) );
  else
    throw cms::Exception("Invalid jetAlgorithm")
      <<"Jet algorithm for RutgersSubJetProducer is invalid: "
      <<jetReclusterAlgorithm_<<", use CambridgeAachen | Kt | AntiKt"<<std::endl;

  // use explicit ghosts in the fastjet clustering sequence?
  if ( iConfig.exists("useExplicitGhosts") )
    useExplicitGhosts_ = iConfig.getParameter<bool>("useExplicitGhosts");

    // Voronoi-based area calculation allows for an empirical scale factor
  if ( iConfig.exists("voronoiRfact") )
    voronoiRfact_     = iConfig.getParameter<double>("voronoiRfact");

  // do fasjet area? => accept corresponding parameters
  if ( doAreaFastjet_ )
  {
    // default Ghost_EtaMax should be 5
    double ghostEtaMax = iConfig.getParameter<double>("Ghost_EtaMax");
    // default Active_Area_Repeats 1
    int    activeAreaRepeats = iConfig.getParameter<int> ("Active_Area_Repeats");
    // default GhostArea 0.01
    double ghostArea = iConfig.getParameter<double> ("GhostArea");
    if ( voronoiRfact_ <= 0 )
    {
      fjActiveArea_     = ActiveAreaSpecPtr(new fastjet::GhostedAreaSpec(ghostEtaMax,activeAreaRepeats,ghostArea));
      fjActiveArea_->set_fj2_placement(true);
      fjAreaDefinition_ = AreaDefinitionPtr( new fastjet::AreaDefinition(fastjet::active_area, *fjActiveArea_ ) );
      if ( !useExplicitGhosts_ )
        fjReclusterAreaDefinition_ = AreaDefinitionPtr( new fastjet::AreaDefinition(fastjet::active_area, *fjActiveArea_ ) );
      else
        fjReclusterAreaDefinition_ = AreaDefinitionPtr( new fastjet::AreaDefinition(fastjet::active_area_explicit_ghosts, *fjActiveArea_ ) );
    }
  }
}


//______________________________________________________________________________
RutgersSubJetAlgorithm::~RutgersSubJetAlgorithm()
{

}


//
// member functions
//

//______________________________________________________________________________
void RutgersSubJetAlgorithm::run(const std::vector<fastjet::PseudoJet>& fjInputs,
                                 std::vector<CompoundPseudoJet>& compoundJets)
{
  if ( !doAreaFastjet_ )
    fjClusterSeq_  = ClusterSequencePtr( new fastjet::ClusterSequence( fjInputs, *fjJetDefinition_ ) );
  else if (voronoiRfact_ <= 0)
    fjClusterSeq_   = ClusterSequencePtr( new fastjet::ClusterSequenceArea( fjInputs, *fjJetDefinition_, *fjAreaDefinition_ ) );
  else
    fjClusterSeq_   = ClusterSequencePtr( new fastjet::ClusterSequenceVoronoiArea( fjInputs, *fjJetDefinition_, fastjet::VoronoiAreaSpec(voronoiRfact_) ) );

  std::vector<fastjet::PseudoJet> inclusiveJets = fastjet::sorted_by_pt( fjClusterSeq_->inclusive_jets(jetPtMin_) );

  // Loop over inclusive jets, attempt to find substructure
  std::vector<fastjet::PseudoJet>::iterator jetIt = inclusiveJets.begin();
  for ( ; jetIt != inclusiveJets.end(); ++jetIt )
  {
    // Recluster jet constituents
    if ( !doAreaFastjet_ )
      fjReclusterSeq_ = ClusterSequencePtr( new fastjet::ClusterSequence( jetIt->constituents(), *fjJetReclusterDefinition_ ) );
    else if (voronoiRfact_ <= 0)
      fjReclusterSeq_ = ClusterSequencePtr( new fastjet::ClusterSequenceArea( jetIt->constituents(), *fjJetReclusterDefinition_, *fjReclusterAreaDefinition_ ) );
    else
      fjReclusterSeq_ = ClusterSequencePtr( new fastjet::ClusterSequenceVoronoiArea( jetIt->constituents(), *fjJetReclusterDefinition_, fastjet::VoronoiAreaSpec(voronoiRfact_) ) );

    std::vector<fastjet::PseudoJet> reclusteredJets = fastjet::sorted_by_pt( fjReclusterSeq_->inclusive_jets() );

    // Can't request more subjets than there are constituents
    unsigned nConst = jetIt->constituents().size();
    unsigned nSubjets = ( nConst>=nSubjets_ ?  nSubjets_ : nConst );

    // Decompose into requested number of subjets
    std::vector<fastjet::PseudoJet> subjets = fastjet::sorted_by_pt( reclusteredJets.front().exclusive_subjets(int(nSubjets)) );
    // Create the subjets objects to put into the "output" objects
    std::vector<CompoundPseudoSubJet>  subjetsOutput;
    std::vector<fastjet::PseudoJet>::const_iterator subJetIt = subjets.begin();
    for ( ; subJetIt != subjets.end(); ++subJetIt )
    {
      // Get the transient subjet constituents from fastjet
      std::vector<fastjet::PseudoJet> subjetConstituents = subJetIt->constituents();
      // Get the indices of the constituents:
      std::vector<int> constituents;
      std::vector<fastjet::PseudoJet>::const_iterator subConstIt = subjetConstituents.begin();
      for ( ; subConstIt != subjetConstituents.end(); ++subConstIt )
      {
        if (subConstIt->user_index() >= 0)
          constituents.push_back( subConstIt->user_index() );
      }

      double subJetArea = 0.0;
      if ( doAreaFastjet_ && subJetIt->has_area() )
        subJetArea = subJetIt->area();

      // Make a CompoundPseudoSubJet object to hold this subjet and the indices of its constituents
      subjetsOutput.push_back( CompoundPseudoSubJet( *subJetIt, subJetArea, constituents ) );
    }

    double jetArea = 0.0;
    if ( doAreaFastjet_ && jetIt->has_area() )
      jetArea = jetIt->area();

    // Make a CompoundPseudoJet object to hold this hard jet, and the subjets that make it up
    compoundJets.push_back( CompoundPseudoJet( *jetIt, jetArea, subjetsOutput));
  }

  return;
}
