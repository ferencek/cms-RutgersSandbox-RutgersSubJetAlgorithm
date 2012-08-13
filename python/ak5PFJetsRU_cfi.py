import FWCore.ParameterSet.Config as cms

from RecoJets.JetProducers.PFJetParameters_cfi import *
from RecoJets.JetProducers.AnomalousCellParameters_cfi import *

ak5PFJetsRU = cms.EDProducer(
    'RutgersSubJetProducer',
    PFJetParameters,
    AnomalousCellParameters,
    jetAlgorithm = cms.string("AntiKt"),
    jetReclusterAlgorithm = cms.string("Kt"),
    rParam       = cms.double(0.5),
    nSubjets     = cms.uint32(2),
    jetCollInstanceName = cms.string("SubJets"),
    useExplicitGhosts = cms.bool(True)
)