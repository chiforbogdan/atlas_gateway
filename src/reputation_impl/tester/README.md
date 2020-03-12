# atlas_gateway_reputation_test
ATLAS IoT Reputation tester

Usage: 
```sh
./atlas_gateway_reputation_test -s 3 -c 2 -f 2 -p 60 -e 3 -t 0.85 -i 10000
```

Atlas gateway - reputation tester:

- --help ---> Display help message
- -s [ --scenario_number ] --> Scenario number: 
                                 - 1 ('c' clients with 'f' feature); 
                                 - 2 ('c' clients with 'f' features each: normal vs bad feedback); 
                                 - 3 ('c' clients with 'f' features each: switching feedback score at half of simulation); 
                                 - 4 ('c' clients with 'f' features for data plane and 4 control plane features and normal feedback)
- -c [ --no_of_clients ] --> Number of clients that will be simulated (between 1 and 10)
- -f [ --no_of_features ] --> Number of dataplane features simulated for each client (between 1 and 5)
- -p [ --ref_data_value ] --> Bad feedback probability when generating random feedback values for dataplane (between 1 and 99)
- -t [ --threshold_value ] --> Feedback threshold value (between 0.1 and 0.95)
- -i [ --no_of_iterations ] --> Number of iterations the simulation should be run (between 1 and 50000)


Depending on the number of selected 'c' clients, each scenario will output a number of 'c' files, named "scenario_#_client_#_.dat".

E.g.: for 2 clients in scenario 1, the resulsts will be written in files "scenario_1_client_1.dat" and "scenario_1_client_2.dat"

For viewing the results as a graph, we recommend using Gnuplot.
Usage:
- from a Terminal and run:
```sh
gnuplot
```
- from gnuplot application, execute the plot command. E.g: for the files obtained in the example above, the command wil be: 
```sh
plot "scenario_1_client_1.dat", "scenario_1_client_2.dat"
```
