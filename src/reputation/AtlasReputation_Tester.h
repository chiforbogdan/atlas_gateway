#ifndef __ATLAS_REPUTATION_TESTER_H__
#define __ATLAS_REPUTATION_TESTER_H__

#include "AtlasReputation_NaiveBayes.h"

namespace atlas
{
class AtlasReputationTester
{
public:
    static double generateFeedback(double, double);
        
    /**
     * @brief Simulation 1 containing: 1 feature, 1 Naive Bayes network, no attacks
     * @param[in] Ideal value that the delivered data should reach
     * @param[in] Error value used when generating random feedbacks
     * @param[in] Threshold value for feedback
     * @param[in] Number of iterations for the simulation
     * @return Results are written in "scenario_1.dat" file
    */
    static void simulateScenario_1(double, double, double, int);

    /**
     * @brief Simulation 2 containing: 2 features, 1 Naive Bayes network, no attacks
     * @param[in] Ideal value that the delivered data should reach
     * @param[in] Error value used when generating random feedbacks
     * @param[in] Threshold value for feedback
     * @param[in] Number of iterations for the simulation
     * @return Results are written in "scenario_2.dat" file
    */
    static void simulateScenario_2(double, double, double, int);    

    /**
     * @brief Simulation 3 containing: 1 feature, 2 Naive Bayes networks, wrong data attack
     * @param[in] Ideal value that the delivered data should reach
     * @param[in] Error value used when generating random feedbacks
     * @param[in] Threshold value for feedback
     * @param[in] Number of iterations for the simulation
     * @return Results are written in "scenario_3_1.dat" and "scenario_3_2.dat" files
    */
    static void simulateScenario_3(double, double, double, int);

    /**
     * @brief Simulation 4 containing: 1 feature, 2 Naive Bayes networks, change of feedback at half of simulation
     * @param[in] Ideal value that the delivered data should reach
     * @param[in] Error value used when generating random feedbacks
     * @param[in] Threshold value for feedback
     * @param[in] Number of iterations for the simulation
     * @return Results are written in "scenario_4_1.dat" and "scenario_4_2.dat" files
    */
    static void simulateScenario_4(double, double, double, int);
};

} //namespace atlas

#endif /*__ATLAS_REPUTATION_TESTER_H__*/