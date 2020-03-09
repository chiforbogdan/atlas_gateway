#ifndef __ATLAS_REPUTATION_TESTER_H__
#define __ATLAS_REPUTATION_TESTER_H__

#include "../AtlasReputation_NaiveBayes.h"

namespace atlas
{
class AtlasReputationTester
{
public:
    static double generateFeedback(double, double, double, double);
        
    /**
     * @brief Simulation containing: 'c' clients with 'f' features, no attacks
     * @param[in] Number of clients
     * @param[in] Number of features for clients
     * @param[in] Ideal value that the delivered data should reach
     * @param[in] Error value used when generating random feedbacks
     * @param[in] Threshold value for feedback
     * @param[in] Number of iterations for the simulation
     * @return Results are written in separate files depending on number of clients
    */
    static void simulateScenario_1(int, int, double, double, double, int);

    /**
     * @brief Simulation containing: 'c' clients with 'f' features, wrong data attack
     * @param[in] Number of clients
     * @param[in] Number of features for clients
     * @param[in] Ideal value that the delivered data should reach
     * @param[in] Error value used when generating random feedbacks
     * @param[in] Threshold value for feedback
     * @param[in] Number of iterations for the simulation
     * @return Results are written in separate files depending on number of clients
    */
    static void simulateScenario_2(int, int, double, double, double, int);    

    /**
     * @brief Simulation containing: 'c' clients with 'f' features, change of feedback at half of simulation
     * @param[in] Number of clients
     * @param[in] Number of features for clients
     * @param[in] Ideal value that the delivered data should reach
     * @param[in] Error value used when generating random feedbacks
     * @param[in] Threshold value for feedback
     * @param[in] Number of iterations for the simulation
     * @return Results are written in separate files depending on number of clients
    */
    static void simulateScenario_3(int, int, double, double, double, int);

    /**
     * @brief Simulation containing: 'c' clients with 'f' dataplane features and 4 controlplane features, no attacks 
     * @param[in] Number of clients
     * @param[in] Number of features for clients
     * @param[in] Ideal value that the delivered data should reach
     * @param[in] Error value used when generating random feedbacks
     * @param[in] Threshold value for feedback
     * @param[in] Number of iterations for the simulation
     * @return Results are written in separate files depending on number of clients
    */
    static void simulateScenario_4(int, int, double, double, double, int);
};

} //namespace atlas

#endif /*__ATLAS_REPUTATION_TESTER_H__*/