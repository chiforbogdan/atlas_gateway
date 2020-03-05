#ifndef __ATLAS_REPUTATION_TESTER_H__
#define __ATLAS_REPUTATION_TESTER_H__

#include "AtlasReputation_NaiveBayes.h"

namespace atlas
{
class AtlasReputationTester
{
public:
    static double generateFeedback(double, double);
    static void simulateScenario_1(double targetVal, double errorVal);
    static void simulateScenario_2(double targetVal, double errorVal);
};

} //namespace atlas

#endif /*__ATLAS_REPUTATION_TESTER_H__*/