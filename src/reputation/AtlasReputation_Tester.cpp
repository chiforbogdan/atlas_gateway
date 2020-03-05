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

    std::cout << "Sign: " << sign << " . Random value: " << tmpRand << std::endl;
    std::cout << "####################################################" << std::endl;

    return (targetVal + (sign * tmpRand * errorVal));
}

void AtlasReputationTester::simulateScenario_1(double targetVal, double errorVal)
{
    AtlasDeviceFeatureManager manager;
    manager.addFeature(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_GENERIC, (targetVal - errorVal + 1));
    
    FILE* fileOut = fopen("scenario_1.dat", "w");
    if (fileOut != nullptr)
    {
        double rez = 0, genFB = 0;
        for (int i=0; i<10; i++)
        {
            genFB = generateFeedback(targetVal, errorVal);
            rez = AtlasReputationNaiveBayes::computeForFeature(manager, AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_GENERIC, genFB);
            std::cout << "Rezultatul reputatiei: " << rez << std::endl;
            std::cout << "##################################################" << std::endl;
            fprintf(fileOut, "%f\n", rez);
        }
        fclose(fileOut);
    }
}

void AtlasReputationTester::simulateScenario_2(double targetVal, double errorVal)
{   
    AtlasDeviceFeatureManager manager;
    manager.addFeature(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_CO2, (targetVal - errorVal + 1));
    manager.addFeature(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_HUMIDITY, (targetVal - (errorVal * 0.5) + 1));

    FILE* fileOut = fopen("scenario_2.dat", "w");
    if (fileOut != nullptr)
    {
        double rez = 0, genFB = 0;
        for (int i=0; i<10; i++)
        {
            std::unordered_map<AtlasDeviceFeatureType, double> fbMatrix;
            for (size_t j=0; j < manager.getDeviceFeatures().size(); j++)//(auto it = manager.getDeviceFeatures().cbegin(); it != manager.getDeviceFeatures().cend(); it++) 
            {
                genFB = generateFeedback(targetVal, errorVal);
                fbMatrix[manager.getDeviceFeatures()[j].getFeatureType()] = genFB;
                //fbMatrix[(*it).getFeatureType()] = genFB;
            }

            rez = AtlasReputationNaiveBayes::computeForDevice(manager, fbMatrix);
            fprintf(fileOut, "%f\n", rez);
        }
        fclose(fileOut);
    } 
}
} //namespace atlas