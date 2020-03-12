#include <iostream>
#include <boost/program_options.hpp>
#include "../../logger/AtlasLogger.h"
#include "AtlasReputation_Tester.h"

namespace {

const std::string ATLAS_GATEWAY_DESC = "Atlas gateway - reputation tester";

/* Scenario number */
int scenarioNo;

/* Number of clients to be simulated */
int noOfClients;

/* Number of features for each client */
int noOfFeatures;

/* Reference value for generating data */
double badFeedbackProbab;

/* Threshold value for feedback */
double thresholdValue;

/* Number of iterations for simulation */
int noOfIterations;

} // anonymous namespace

void parse_options(int argc, char **argv)
{
    boost::program_options::options_description desc(ATLAS_GATEWAY_DESC);
    boost::program_options::variables_map vm;
    
    desc.add_options()
    ("help", "Display help message")
    ("scenario_number,s", boost::program_options::value<int>(&scenarioNo), "Scenario number: \n\t - 1 ('c' clients with 'f' feature); \n\t - 2 ('c' clients with 'f' features each: normal vs bad feedback); \n\t - 3 ('c' clients with 'f' features each: switching feedback score at half of simulation); \n\t - 4 ('c' clients with 'f' features and 4 control plane features and normal feedback)")
    ("no_of_clients,c", boost::program_options::value<int>(&noOfClients), "Number of clients that will be simulated (between 1 and 10)")
    ("no_of_features,f", boost::program_options::value<int>(&noOfFeatures), "Number of dataplane features simulated for each client (between 1 and 5)")
    ("bad_fb_probability,p", boost::program_options::value<double>(&badFeedbackProbab), "Bad feedback probability when generating random feedback values for dataplane (between 1 and 99)")
    ("threshold_value,t", boost::program_options::value<double>(&thresholdValue), "Feedback threshold value (between 0.1 and 0.95)")
    ("no_of_iterations,i", boost::program_options::value<int>(&noOfIterations), "Number of iterations the simulation should be run (between 1 and 50000)");


    try {
        boost::program_options::store(boost::program_options::command_line_parser(argc, argv)
                                      .options(desc).run(), vm);
        
        if (vm.count("help")) {
            std::cout << desc << std::endl;
            exit(1);
        }

        boost::program_options::notify(vm);
    
        /* Scenario number validation */
        if (scenarioNo < 1 || scenarioNo > 5) {
            std::cout << "ERROR: Invalid scenario number" << std::endl;
            std::cout << desc << std::endl;
            exit(1);
        }    

        /* Client number validation */
        if (noOfClients < 1 || noOfClients > 10) {
            std::cout << "ERROR: Invalid number of clients" << std::endl;
            std::cout << desc << std::endl;
            exit(1);
        }    

        /* Features number validation */
        if (noOfFeatures < 1 || noOfFeatures > 5) {
            std::cout << "ERROR: Invalid number of features" << std::endl;
            std::cout << desc << std::endl;
            exit(1);
        } 

        /* Data refence value validation */
        if (badFeedbackProbab < 1 || badFeedbackProbab > 99) {
            std::cout << "ERROR: Invalid value for data reference" << std::endl;
            std::cout << desc << std::endl;
            exit(1);
        } 

        /* Threshold value validation */
        if (thresholdValue < 0.1 || thresholdValue > 0.95) {
            std::cout << "ERROR: Invalid value for feedback threshold" << std::endl;
            std::cout << desc << std::endl;
            exit(1);
        } 

        /* Client number validation */
        if (noOfIterations < 1 || noOfIterations > 50000) {
            std::cout << "ERROR: Invalid number of iterations" << std::endl;
            std::cout << desc << std::endl;
            exit(1);
        } 

    } catch(boost::program_options::error& e) {
        std::cout << desc << std::endl;
        exit(1);
    }
}

int main(int argc, char **argv)
{
    parse_options(argc, argv);

    atlas::initLog();

    switch (scenarioNo)
    {
        case 1:
            std::cout << "Scenario 1 with " << noOfClients << " clients, each with " << noOfFeatures << " features. Bad_feedback_prob = " << badFeedbackProbab << ". Threshold = " << thresholdValue << ". Iterations = " << noOfIterations << std::endl;
            atlas::AtlasReputationTester::simulateScenario_1(noOfClients, noOfFeatures, badFeedbackProbab, thresholdValue, noOfIterations);
            break;
        case 2:
            std::cout << "Scenario 2 with " << noOfClients << " clients, each with " << noOfFeatures << " features. Bad_feedback_prob = " << badFeedbackProbab << ". Threshold = " << thresholdValue << ". Iterations = " << noOfIterations << std::endl;
            atlas::AtlasReputationTester::simulateScenario_2(noOfClients, noOfFeatures, badFeedbackProbab, thresholdValue, noOfIterations);
            break;
        case 3:
            std::cout << "Scenario 3 with " << noOfClients << " clients, each with " << noOfFeatures << " features. Bad_feedback_prob = " << badFeedbackProbab << ". Threshold = " << thresholdValue << ". Iterations = " << noOfIterations << std::endl;
            atlas::AtlasReputationTester::simulateScenario_3(noOfClients, noOfFeatures, badFeedbackProbab, thresholdValue, noOfIterations);
            break;
        case 4:
            std::cout << "Scenario 4 with " << noOfClients << " clients, each with " << noOfFeatures << " features. Bad_feedback_prob = " << badFeedbackProbab << ". Threshold = " << thresholdValue << ". Iterations = " << noOfIterations << std::endl;
            atlas::AtlasReputationTester::simulateScenario_4(noOfClients, noOfFeatures, badFeedbackProbab, thresholdValue, noOfIterations);
            break;
        case 5:
            std::cout << "Scenario 5 with " << noOfClients << " clients, each with " << noOfFeatures << " features. Bad_feedback_prob = " << badFeedbackProbab << ". Threshold = " << thresholdValue << ". Iterations = " << noOfIterations << std::endl;
            atlas::AtlasReputationTester::simulateScenario_5(noOfClients, noOfFeatures, badFeedbackProbab, thresholdValue, noOfIterations);
            break;
    }

    return 0;
};