[![License](https://img.shields.io/badge/license-GPL%20v3.0%20or%20later-brightgreen.svg)](https://github.com/chiforbogdan/atlas_gateway/blob/master/LICENSE)

# ATLAS IoT Security Platform - general information
ATLAS consists in a 3-tier IoT security platform which offers the following modules:
* A lightweight software client which runs on the IoT device ([Atlas_client])
* A gateway software which runs on the network edge and manages all the clients from the network ([Atlas_gateway])
* A cloud platform which allows managing the gateways and the clients ([Atlas_cloud])

ATLAS provides security management for a fleet of IoT devices and enables a reputation based Sensing-as-a-service platform. It also offers the capability to inspect the IoT device telemetry values and supports the CoAP lightweight protocol for the communication between the IoT device and the gateway.
On the IoT data plane layer, ATLAS provides an API which can be integrated with a user application and offers the following capabilities:
* Install a firewall rule on the gateway side
* Send packet statistics to the gateway and cloud
* Get the device with the most trusted reputation within a category and provide reputation feedback

## ATLAS IoT Security Gateway
ATLAS IoT Security Gateway is a gateway software which runs on the network edge and manages all the clients from within the network. It enables the collection of telemetry data from connected IoT nodes (that have [ATLAS IoT Security Client][Atlas_client] software) and offeres a method of filtering malicious nodes through an integrated reputation-based scoring architecture.

----

#### How to build it
Generally, there are two steps are involved:
* Step 1. Install required dependencies by executing the [dependencies.sh][dep_script] script from the [scripts][script_dir] folder
* Step 2. Execute the [build.sh][build_script] script from [root][root_dir] folder

Depending on the platform you are using, minor adjustments might be necessary to be made. See the output messages shown during execution of the above mentioned scripts in case of errors.

----

#### How to use it
```
./atlas_gateway --cloud <ATLAS_CLOUD_IP> --listen <ATLAS_CLIENT_LISTEN_PORT>
```

Arguments:
* __--help__ &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; _Display help message_
* __-c [ --cloud ]__ &nbsp;&nbsp; _IP address for the ATLAS_CLOUD broker - used to connect the ATLAS_GATEWAY with the ATLAS_CLOUD back-end_
* __-l [ --listen ]__ &nbsp;&nbsp;&nbsp; _Listen port number for the ATLAS_GATEWAY CoAP server - userd to connect the ATLAS_GATEWAY with the ATLAS_CLIENT instances_

Example: 
```
./atlas_gateway --cloud 127.0.0.1 -l 10100
```

----

#### Authors
ATLAS Gateway was developed by:
* Bogdan-Cosmin Chifor
* Stefan-Ciprian Arseni
* Ioana Cismas
* Mihai Coca
* Mirabela Medvei

ATLAS project is sponsored by [UEFISCDI].

----

#### License
GNU General Public License v3.0 or later.

See LICENSE file to read the full text.

[Atlas_client]: https://github.com/chiforbogdan/atlas_client
[Atlas_gateway]: https://github.com/chiforbogdan/atlas_gateway
[Atlas_cloud]: https://github.com/chiforbogdan/atlas_cloud
[UEFISCDI]: https://uefiscdi.gov.ro/
[dep_script]: https://github.com/chiforbogdan/atlas_gateway/tree/mqtt_cloud_ssl/scripts/dependencies.sh
[script_dir]: https://github.com/chiforbogdan/atlas_gateway/tree/mqtt_cloud_ssl/scripts/
[build_script]: https://github.com/chiforbogdan/atlas_gateway/blob/mqtt_cloud_ssl/build.sh
[root_dir]: https://github.com/chiforbogdan/atlas_gateway/tree/mqtt_cloud_ssl/
