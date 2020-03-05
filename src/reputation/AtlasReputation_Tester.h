#ifndef __ATLAS_REPUTATION_TESTER_H__
#define __ATLAS_REPUTATION_TESTER_H__

#include "AtlasReputation_NaiveBayes.h"

namespace atlas
{
class AtlasReputationTester
{
public:
    static double generateFeedback(double, double);
    static void simulateScenario_1(double, double, int);
    static void simulateScenario_2(double, double, int);
};

} //namespace atlas

#endif /*__ATLAS_REPUTATION_TESTER_H__*/