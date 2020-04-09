[![License](https://img.shields.io/badge/license-GPL%20v3.0%20or%20later-brightgreen.svg)](https://github.com/chiforbogdan/atlas_gateway/blob/master/LICENSE)

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
ATLAS_GATEWAY Security Component was developed by:
* Bogdan-Cosmin Chifor
* Stefan-Ciprian Arseni
* Ioana Cismas
* Mihai Coca
* Mirabela Medvei

The ATLAS project is sponsored by [UEFISCDI].

----

#### License
GNU General Public License v3.0 or later.

See [LICENSE] file to read the full text.

[Atlas_client]: https://github.com/chiforbogdan/atlas_client
[UEFISCDI]: https://uefiscdi.gov.ro/
[LICENSE]: https://github.com/chiforbogdan/atlas_gateway/blob/master/LICENSE
[dep_script]: https://github.com/chiforbogdan/atlas_gateway/tree/mqtt_cloud_ssl/scripts/dependencies.sh
[script_dir]: https://github.com/chiforbogdan/atlas_gateway/tree/mqtt_cloud_ssl/scripts/
[build_script]: https://github.com/chiforbogdan/atlas_gateway/blob/mqtt_cloud_ssl/build.sh
[root_dir]: https://github.com/chiforbogdan/atlas_gateway/tree/mqtt_cloud_ssl/
