#include <iostream>
#include <string>
#include <vector>
#include "AtlasReputation_Tester.h"

namespace atlas 
{

double AtlasReputationTester::generateFeedback(double targetVal, double errorVal)
{
    double tmpRand = rand() % 10;
    int sign = 1;
    if (tmpRand < 5)
        sign = -1;
    tmpRand = ((double)rand() / RAND_MAX);

    return (targetVal + (sign * tmpRand * errorVal));
}

void AtlasReputationTester::simulateScenario_1(double targetVal, double errorVal, double thresholdVal, int noOfIterations)
{
    AtlasDeviceFeatureManager manager;
    manager.addFeature(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_GENERIC, 1, thresholdVal);
    
    FILE* fileOut = fopen("scenario_1.dat", "w");
    if (fileOut != nullptr)
    {
        double rez = 0, genFB = 0;
        std::vector<std::pair<AtlasDeviceFeatureType, double>> fbMatrix;
        std::pair<AtlasDeviceFeatureType, double> tmpPair;
        
        for (int i = 0; i < noOfIterations; i++)
        {            
            for (size_t j=0; j < manager.getDeviceFeatures().size(); j++) 
            {
                genFB = generateFeedback(targetVal, errorVal);
                tmpPair.first = manager.getDeviceFeatures()[j].getFeatureType();
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

void AtlasReputationTester::simulateScenario_2(double targetVal, double errorVal, double thresholdVal, int noOfIterations)
{
    AtlasDeviceFeatureManager manager;
    manager.addFeature(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_GENERIC, 0.8, thresholdVal);
    manager.addFeature(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_TEMPERATURE, 0.2, thresholdVal);
    
    FILE* fileOut = fopen("scenario_2.dat", "w");
    if (fileOut != nullptr)
    {
        double rez = 0, genFB = 0;
        std::vector<std::pair<AtlasDeviceFeatureType, double>> fbMatrix;
        std::pair<AtlasDeviceFeatureType, double> tmpPair;
        
        for (int i = 0; i < noOfIterations; i++)
        {            
            for (size_t j=0; j < manager.getDeviceFeatures().size(); j++) 
            {
                genFB = generateFeedback(targetVal, errorVal);
                tmpPair.first = manager.getDeviceFeatures()[j].getFeatureType();
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

void AtlasReputationTester::simulateScenario_3(double targetVal, double errorVal, double thresholdVal, int noOfIterations)
{
    AtlasDeviceFeatureManager manager;
    manager.addFeature(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_GENERIC, 1, thresholdVal);
    
    FILE* fileOut = fopen("scenario_3_1.dat", "w");
    if (fileOut != nullptr)
    {
        double rez = 0, genFB = 0;
        std::vector<std::pair<AtlasDeviceFeatureType, double>> fbMatrix;
        std::pair<AtlasDeviceFeatureType, double> tmpPair;
        
        for (int i = 0; i < noOfIterations; i++)
        {            
            for (size_t j=0; j < manager.getDeviceFeatures().size(); j++) 
            {
                genFB = generateFeedback(targetVal, errorVal);
                tmpPair.first = manager.getDeviceFeatures()[j].getFeatureType();
                tmpPair.second = genFB;
                fbMatrix.push_back(tmpPair);
            }

            rez = AtlasReputationNaiveBayes::computeReputation(manager, fbMatrix);
            fprintf(fileOut, "%f\n", rez);

            fbMatrix.clear();
        }
        fclose(fileOut);
    }

    FILE* fileOut2 = fopen("scenario_3_2.dat", "w");
    double newErrorVal = errorVal * 1.5;
    if (fileOut2 != nullptr)
    {
        double rez = 0, genFB = 0;
        std::vector<std::pair<AtlasDeviceFeatureType, double>> fbMatrix;
        std::pair<AtlasDeviceFeatureType, double> tmpPair;
        
        for (int i = 0; i < noOfIterations; i++)
        {            
            for (size_t j=0; j < manager.getDeviceFeatures().size(); j++) 
            {
                genFB = generateFeedback(targetVal, newErrorVal);
                tmpPair.first = manager.getDeviceFeatures()[j].getFeatureType();
                tmpPair.second = genFB;
                fbMatrix.push_back(tmpPair);
            }

            rez = AtlasReputationNaiveBayes::computeReputation(manager, fbMatrix);
            fprintf(fileOut2, "%f\n", rez);

            fbMatrix.clear();
        }
        fclose(fileOut2);
    }
}

void AtlasReputationTester::simulateScenario_4(double targetVal, double errorVal, double thresholdVal, int noOfIterations)
{
    AtlasDeviceFeatureManager manager;
    manager.addFeature(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_GENERIC, 1, thresholdVal);
    
    //1st Naive Bayes network
    FILE* fileOut = fopen("scenario_4_1.dat", "w");
    if (fileOut != nullptr)
    {
        double rez = 0, genFB = 0;
        std::vector<std::pair<AtlasDeviceFeatureType, double>> fbMatrix;
        std::pair<AtlasDeviceFeatureType, double> tmpPair;
        double newErrorVal = errorVal * 1.5;
        
        //1st half with normal feedbacks
        for (int i = 0; i < (noOfIterations / 2); i++)
        {            
            for (size_t j=0; j < manager.getDeviceFeatures().size(); j++) 
            {
                genFB = generateFeedback(targetVal, errorVal);
                tmpPair.first = manager.getDeviceFeatures()[j].getFeatureType();
                tmpPair.second = genFB;
                fbMatrix.push_back(tmpPair);
            }

            rez = AtlasReputationNaiveBayes::computeReputation(manager, fbMatrix);
            fprintf(fileOut, "%f\n", rez);

            fbMatrix.clear();
        }

        //2nd half with worse feedbacks
        for (int i = 0; i < (noOfIterations / 2); i++)
        {            
            for (size_t j=0; j < manager.getDeviceFeatures().size(); j++) 
            {
                genFB = generateFeedback(targetVal, newErrorVal);
                tmpPair.first = manager.getDeviceFeatures()[j].getFeatureType();
                tmpPair.second = genFB;
                fbMatrix.push_back(tmpPair);
            }

            rez = AtlasReputationNaiveBayes::computeReputation(manager, fbMatrix);
            fprintf(fileOut, "%f\n", rez);

            fbMatrix.clear();
        }

        fclose(fileOut);
    }

    //2nd Naive Bayes network
    FILE* fileOut2 = fopen("scenario_4_2.dat", "w");
    double newErrorVal = errorVal * 1.5;
    if (fileOut2 != nullptr)
    {
        double rez = 0, genFB = 0;
        std::vector<std::pair<AtlasDeviceFeatureType, double>> fbMatrix;
        std::pair<AtlasDeviceFeatureType, double> tmpPair;
        
        //1st half with worse feedbacks
        for (int i = 0; i < (noOfIterations / 2); i++)
        {            
            for (size_t j=0; j < manager.getDeviceFeatures().size(); j++) 
            {
                genFB = generateFeedback(targetVal, newErrorVal);
                tmpPair.first = manager.getDeviceFeatures()[j].getFeatureType();
                tmpPair.second = genFB;
                fbMatrix.push_back(tmpPair);
            }

            rez = AtlasReputationNaiveBayes::computeReputation(manager, fbMatrix);
            fprintf(fileOut2, "%f\n", rez);

            fbMatrix.clear();
        }

        //2nd half with normal feedbacks
        for (int i = 0; i < (noOfIterations / 2); i++)
        {            
            for (size_t j=0; j < manager.getDeviceFeatures().size(); j++) 
            {
                genFB = generateFeedback(targetVal, errorVal);
                tmpPair.first = manager.getDeviceFeatures()[j].getFeatureType();
                tmpPair.second = genFB;
                fbMatrix.push_back(tmpPair);
            }

            rez = AtlasReputationNaiveBayes::computeReputation(manager, fbMatrix);
            fprintf(fileOut2, "%f\n", rez);

            fbMatrix.clear();
        }

        fclose(fileOut2);
    }
}
} //namespace atlas