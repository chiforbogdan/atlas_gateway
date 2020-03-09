#include <iostream>
#include <string>
#include <vector>
#include "AtlasReputation_Tester.h"

namespace atlas 
{

double AtlasReputationTester::generateFeedback(double targetVal, double errorVal, double thresholdVal, double badFeedbackRatio)
{
    double tmpRand = rand() % 10;
    int sign = 1;
    if (tmpRand < (badFeedbackRatio * 10.0))
        sign = -1;
    tmpRand = ((double)rand() / RAND_MAX);

    double tmpFinal = targetVal + (sign * tmpRand * errorVal);   
    if (tmpFinal > targetVal) {
        double ratio = (tmpFinal - targetVal) / targetVal;
        return (thresholdVal + ratio * (0.99 - thresholdVal));
    }
    return (tmpFinal / targetVal);
}

void AtlasReputationTester::simulateScenario_1(int noOfClients, int noOfFeatures, double targetVal, double errorVal, double thresholdVal, int noOfIterations)
{
    double badFeedbackRatio = 0.4;    
    for (int i=1; i <= noOfClients; i++) {
        std::string fileName = "scenario_1_client_" + std::to_string(i) + ".dat";
        AtlasDeviceFeatureManager manager;
        manager.updateFeedbackThreshold(thresholdVal);

        for (int j=0; j < noOfFeatures; j++) {
            manager.addFeature((AtlasDeviceFeatureType)j, 1.0/(double)noOfFeatures);
        }        

        FILE* fileOut = fopen(fileName.c_str(), "w");
        if (fileOut != nullptr) {
            double rez = 0, genFB = 0;
            std::vector<std::pair<AtlasDeviceFeatureType, double>> fbMatrix;
            std::pair<AtlasDeviceFeatureType, double> tmpPair;
            
            for (int j = 0; j < noOfIterations; j++) {            
                for (size_t k=0; k < manager.getDeviceFeatures().size(); k++) {
                    genFB = generateFeedback(targetVal, errorVal, thresholdVal, badFeedbackRatio);
                    tmpPair.first = manager.getDeviceFeatures()[k].getFeatureType();
                    tmpPair.second = genFB;
                    fbMatrix.push_back(tmpPair);
                }

                rez = AtlasReputationNaiveBayes::computeReputation(manager, fbMatrix);
                fprintf(fileOut, "%f\n", rez);
                fbMatrix.clear();
            }
            fclose(fileOut);
        }
    }
}

void AtlasReputationTester::simulateScenario_2(int noOfClients, int noOfFeatures, double targetVal, double errorVal, double thresholdVal, int noOfIterations)
{
    double badFeedbackRatio = 0.3, dataError = errorVal;
    for (int i=1; i <= noOfClients; i++) {
        std::string fileName = "scenario_2_client_" + std::to_string(i) + ".dat";
        AtlasDeviceFeatureManager manager;
        manager.updateFeedbackThreshold(thresholdVal);

        for (int j=0; j < noOfFeatures; j++) {
            manager.addFeature((AtlasDeviceFeatureType)j, 1.0/(double)noOfFeatures);
        }

        FILE* fileOut = fopen(fileName.c_str(), "w");
        if (fileOut != nullptr) {
            double rez = 0, genFB = 0;
            std::vector<std::pair<AtlasDeviceFeatureType, double>> fbMatrix;
            std::pair<AtlasDeviceFeatureType, double> tmpPair;

            for (int j=0; j < noOfIterations; j++) {  
                if ((i > (noOfClients / 2)) && (j > (noOfIterations / 2))) {
                    badFeedbackRatio = 0.95;
                    dataError = 1.5 * errorVal;
                }         
                for (int k=0; k < noOfFeatures; k++) {
                    genFB = generateFeedback(targetVal, dataError, thresholdVal, badFeedbackRatio);
                    //std::cout << "Am generat feedback-ul: " << genFB << std::endl;
                    tmpPair.first = manager.getDeviceFeatures()[k].getFeatureType();
                    tmpPair.second = genFB;
                    fbMatrix.push_back(tmpPair);
                }

                rez = AtlasReputationNaiveBayes::computeReputation(manager, fbMatrix);
                fprintf(fileOut, "%f\n", rez);
                fbMatrix.clear();
            }
            fclose(fileOut);
        }
    }
}

void AtlasReputationTester::simulateScenario_3(int noOfClients, int noOfFeatures, double targetVal, double errorVal, double thresholdVal, int noOfIterations)
{
    double badFeedbackRatio;
    double dataError;
    for (int i=1; i <= noOfClients; i++) {
        std::string fileName = "scenario_3_client_" + std::to_string(i) + ".dat";
        AtlasDeviceFeatureManager manager;
        manager.updateFeedbackThreshold(thresholdVal);

        for (int j=0; j < noOfFeatures; j++) {
            manager.addFeature((AtlasDeviceFeatureType)j, 1.0/(double)noOfFeatures);
        }

        FILE* fileOut = fopen(fileName.c_str(), "w");
        if (fileOut != nullptr) {
            double rez = 0, genFB = 0;
            dataError = errorVal;
            badFeedbackRatio = 0.4;
            std::vector<std::pair<AtlasDeviceFeatureType, double>> fbMatrix;
            std::pair<AtlasDeviceFeatureType, double> tmpPair;   

            //First half of the scenario, with normal feedback
            for (int j = 0; j < (noOfIterations/2); j++) {            
                for (int k=0; k < noOfFeatures; k++) {
                    genFB = generateFeedback(targetVal, dataError, thresholdVal, badFeedbackRatio);
                    tmpPair.first = manager.getDeviceFeatures()[k].getFeatureType();
                    tmpPair.second = genFB;
                    fbMatrix.push_back(tmpPair);
                }

                rez = AtlasReputationNaiveBayes::computeReputation(manager, fbMatrix);
                fprintf(fileOut, "%f\n", rez);
                fbMatrix.clear();
            }

            //Second half of the scenario with worse feedback (error is 50% greater)
            badFeedbackRatio = 0.8;
            dataError = errorVal * 1.5;
            for (int j = 0; j < (noOfIterations/2); j++) {            
                for (int k=0; k < noOfFeatures; k++) {
                    genFB = generateFeedback(targetVal, dataError, thresholdVal, badFeedbackRatio);
                    tmpPair.first = manager.getDeviceFeatures()[k].getFeatureType();
                    tmpPair.second = genFB;
                    fbMatrix.push_back(tmpPair);
                }

                rez = AtlasReputationNaiveBayes::computeReputation(manager, fbMatrix);
                fprintf(fileOut, "%f\n", rez);
                fbMatrix.clear();
            }

            fclose(fileOut);
        }
    }
}

void AtlasReputationTester::simulateScenario_4(int noOfClients, int noOfFeatures, double targetVal, double errorVal, double thresholdVal, int noOfIterations)
{
    double badFeedbackRatio = 0.4;
    double weightOfControlPlane = 0.35;    
    for (int i=1; i <= noOfClients; i++) {
        std::string fileName = "scenario_4_client_" + std::to_string(i) + ".dat";
        
        /***** Manager for Control Plane features *****/
        AtlasDeviceFeatureManager managerData;
        managerData.updateFeedbackThreshold(thresholdVal);
        for (int j=0; j < noOfFeatures; j++) {
            managerData.addFeature((AtlasDeviceFeatureType)j, 1.0/(double)noOfFeatures);
        }
        managerData.addFeature(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_GENERIC, 1);

        /***** Manager for Data Plane features *****/
        AtlasDeviceFeatureManager managerCtrl;
        managerCtrl.updateFeedbackThreshold(thresholdVal);
        managerCtrl.addFeature(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_UPTIME, 0.3);
        managerCtrl.addFeature(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_MISSED_KEEP_ALIVE_PACKETS, 0.15);
        managerCtrl.addFeature(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_PACKETS_ACCEPTANCY_RATE, 0.35);
        managerCtrl.addFeature(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_REGISTERING_RATE, 0.2);

        FILE* fileOut = fopen(fileName.c_str(), "w");
        if (fileOut != nullptr) {
            double rezData = 0, rezCtrl = 0, rez, genFB = 0;
            std::vector<std::pair<AtlasDeviceFeatureType, double>> fbMatrix;
            std::pair<AtlasDeviceFeatureType, double> tmpPair;
            
            for (int j = 0; j < noOfIterations; j++) {
                /***** Generate feedback and compute reputation for Data Plane *****/            
                for (size_t k=0; k < managerData.getDeviceFeatures().size(); k++) {
                    genFB = generateFeedback(targetVal, errorVal, thresholdVal, badFeedbackRatio);
                    tmpPair.first = managerData.getDeviceFeatures()[k].getFeatureType();
                    tmpPair.second = genFB;
                    fbMatrix.push_back(tmpPair);
                }
                rezData = AtlasReputationNaiveBayes::computeReputation(managerData, fbMatrix);
                fbMatrix.clear();

                /***** Generate feedback and compute reputation for Control Plane *****/            
                for (size_t k=0; k < managerCtrl.getDeviceFeatures().size(); k++) {
                    genFB = generateFeedback(targetVal, errorVal, thresholdVal, badFeedbackRatio);
                    tmpPair.first = managerCtrl.getDeviceFeatures()[k].getFeatureType();
                    tmpPair.second = genFB;
                    fbMatrix.push_back(tmpPair);
                }
                rezCtrl = AtlasReputationNaiveBayes::computeReputation(managerCtrl, fbMatrix);
                fbMatrix.clear();

                /***** Generate feedback for device *****/
                rez = (rezCtrl * weightOfControlPlane) + (rezData * (1 - weightOfControlPlane));
                fprintf(fileOut, "%f\n", rez);
            }
            fclose(fileOut);
        }
    }
}
} //namespace atlas