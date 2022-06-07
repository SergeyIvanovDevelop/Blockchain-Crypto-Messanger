<h1 align="center">
  BCM Linux-Console
</h1>

## BCM Linux-Console &middot; [![GitHub license](https://img.shields.io/badge/license-CC%20BY--NC--SA%203.0-blue)](./LICENSE) [![Python](https://img.shields.io/badge/language-Python-critical)](https://www.python.org/) [![C](https://img.shields.io/badge/language-C-yellow)](https://www.iso.org/standard/74528.html) [![C++](https://img.shields.io/badge/language-C%2B%2B-yellowgreen)](https://isocpp.org/) [![SQLite](https://img.shields.io/badge/DB-SQLite-blue)](https://www.sqlite.org/index.html) [![Address Sanitizer](https://img.shields.io/badge/safety-Address%20Sanitizer-9cf)](https://github.com/google/sanitizers/wiki/AddressSanitizer) [![I2Pd](https://img.shields.io/badge/anonymity-i2pd-success)](https://i2pd.website/) [![Yggdrasil](https://img.shields.io/badge/mesh-yggdrasil-lightgrey)](https://yggdrasil-network.github.io/) [![LinkedIn](https://img.shields.io/badge/linkedin-Sergey%20Ivanov-blue)](https://www.linkedin.com/in/sergey-ivanov-33413823a/) [![Telegram](https://img.shields.io/badge/telegram-%40SergeyIvanov__dev-blueviolet)](https://t.me/SergeyIvanov_dev) ##

This branch of the repository contains the code and configuration for running a blockchain network node on a device with `Ubuntu` OS.<br>

The software of the blockchain network node has an integrated messenger console client that allows secure communication with other network members, as well as the use of a distributed data storage.

## :computer: Getting Started  ##

**Step 1**

1. Go to home directory and clone repository from github: `cd ~ && git clone https://SergeyIvanovDevelop@github.com/SergeyIvanovDevelop/Blockchain-Crypto-Messanger`

**Step 2**<br>

2. Open the `Linux-Console` application code from the `bcm-linux-console` branch

**Step 3**<br>

3. Install project dependencies:
- [boost_1_74_0](https://www.boost.org/users/history/version_1_74_0.html)
- [openssl](https://www.openssl.org/)
- [sqlite](https://www.sqlite.org/index.html)
- [crypto++](https://www.cryptopp.com/)
- [yggdrasil](https://yggdrasil-network.github.io/)
- [i2pd](https://i2pd.website/)

**Step 4**<br>

4. Configure `yggdrasil` as follows:
- You need to add the addresses of the `Yggdrasil` network nodes to the configuration file `/etc/yggdrasil.conf` (an example is in the file [yggdrasil.conf](./Support_Dir/yggdrasil.conf))

**Step 5**<br>

5. Configure `i2pd` as follows:
- Adjust the configuration file `/etc/i2pd/i2pd.conf` according to the example (file [i2pd.conf](./Support_Dir/i2pd.conf))

**Step 6**<br>

6. Build the project like this (requires [AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer) or use the `-f` flag and `Makefile_2`):
- Open the `Linux-Console` application code from the `bcm-linux-console` branch and run `make clean & make`

**Step 7**<br>

7. Run the project like this: `./Messenger <IP-address server> <server port> <another client port blockchain> <another client port message> <yggdrasil port client-server> <yggdrasil port server-client> <your user name> <use_I2P: [TRUE/FALSE]> <SAM_port>`
Description of parameters:<br>
- `<IP-address server>` - `IP`-address of the server (now not used, used in a more centralized implementation, but it is possible to run in this mode, so the parameter remains for compatibility). Basically, the server was used as a real-time storage of `IP` addresses currently present in the network of devices. At the moment, users exchange `i2p` and `yggdrasil` addresses in person.
- `<server port>` - port of the server described above (remained for compatibility).
- `<another client port blockchain>` - port on which clients accept requests to provide a hash of the last transaction and requests to provide an up-to-date copy of the blockchain
- `<another client port message>` - port on which clients receive messages claiming to be added to the blockchain
- `<yggdrasil port client-server>` - port on which the server part of the clients listens for requests for storing information (receiving data to distributed storage)
- `<yggdrasil port server-client>` - port, the port on which the server part of the clients listens for requests to retrieve information (sending data from distributed storage)
- `<your user name>` - username under which you are known to other network users with whom you want to communicate
- `<use_I2P: [TRUE/FALSE]>` - parameter that determines the operation mode of the blockchain node (via `i2p` or `clearnet`). The node will only be able to communicate with nodes that have the same mode of operation selected at startup
- `<SAM_port>` - port on which `i2pd` is running a service that supports `SAM` protocol. At the moment, the code uses static tunnels `i2p`

Example: `./Messenger 192.168.0.1 2000 2001 2002 2003 2004 user1 TRUE 7656`

However, it is possible to run code in program control mode using [AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer). To do this, you need to open the file [start_user_1.sh](./start_user_1.sh), fill in the parameters you need in the launch line and run this `bash` script.

**Step 8**<br>

8. Run the `Android` application, enter the `IP` address and port of the server with `Ubuntu` OS, where the node with the messenger core is deployed, and click the `Connect` button.<br>

You need to add users. For this you need to have:
-`File-key` of the user with whom you want to communicate (you can generate such a file using the [gen_key](./gen_key) utility) by typing the command in the terminal: `./gen_key <key_size> <output_filename>`. Only ONE of the users generates a `key file` and transfers it to another person in a personal meeting or via a trusted communication channel
-`i2p`-address of the interlocutor (receives it also during a personal meeting or via a trusted communication channel)
-`yggdrasil`-address of the interlocutor (receives it also during a personal meeting or via a trusted communication channel)

Next you need:
- Place the `key file` of the interlocutor in the code directory from the `bcm-linux-console` branch
- In the file `/etc/i2pd/tunnels.conf` register static tunnels for each user (including yourself). An example of how to do this can be found in the [tunnels.conf](./TUNNELS/tunnels.conf) file.

For a more detailed understanding, consider a specific example:<br>
```
[HASH_BLOCKCHAIN_1-CLIENT]
type = client
address = 127.0.0.1
port = 5000
destination = 6g6lrmiiyx4c7rvrbsqywugsiaix4ypem4y3wizwwd7hqyqhoswa.b32.i2p
destinationport = 6001
keys = HASH_BLOCKCHAIN_1-CLIENT.dat

[SEND_MESSAGE_1-CLIENT]
type = client
address = 127.0.0.1
port = 7000
destination = mhaseyc4bsugusuk5jdchide2na4ixffrhx2sg3jchohsv3qy5pq.b32.i2p
destinationport = 6002
keys = HASH_BLOCKCHAIN_1-CLIENT.dat
```

The configuration code shown above describes how `i2pd` will interact with a user with `i2p` address `6g6lrmiiyx4c7rvrbsqywugsiaix4ypem4y3wizwwd7hqyqhoswa.b32.i2p` and port `6001` to accept requests for hashes of the latest transaction or an up-to-date copy of the blockchain. From our node, a connection to the given `i2p` address will be established via the `proxy-server` provided by `i2pd` when a connection request is received at `127.0.0.1:5000`.<br>
To receive candidate messages for adding to the blockchain, the user has chosen `i2p` address `mhaseyc4bsugusuk5jdchide2na4ixffrhx2sg3jchohsv3qy5pq.b32.i2p` and port `6002`. From our host, a connection to the given `i2p` address will be established via the `proxy-server` provided by `i2pd` when a connection request is received at `127.0.0.1:7000`.<br>

- In the file [Yggdrasil_addresses.txt](./Yggdrasil_addresses.txt) you need to put the `yggdrasil` address of the interlocutor. (Adding this address is optional, it is only necessary to be able to use the interlocutor as a data storage node in a distributed information store.)<br>

When adding a new user through the console application, you must specify the path to the `key file` of the interlocutor and his name. After the actions described above, you can start using the messenger.

**Step 9**<br>

To use a node as one of the elements of a distributed data store, you need to run the following commands from the code directory of the `bcm-linux-console` branch:
- `cd servers_together/ && ./ssl_server_receive <yggdrasil port client-server>` to receive files for storage
- `cd servers_together/ && ./ssl_server_send <yggdrasil port server-client>` to send files from storage

**Step 10**<br>

In order to be able to control the host via the `Android` application, you need to run `Python-Server`: `sudo python3 Remote_controller_MSG_whole.py <IP-address server> <server port> > log.txt`.<br>
Description of parameters:
- `<IP-address server>` - `IP`-address of the blockchain network node (in fact, `IP`-address of the computer running this `Python` script)
- `<server port>` - the port on which the server will be launched in this script, which will accept connections from the `Android` device
- `> log.txt` - necessary to override the output of the script to a file for debugging and health monitoring

**Step 11**<br>

To ensure greater anonymity, you need to run another `Python` script with the following command: `python3 Peridically_send_random_message_in_blockcgain <pseudo_user_name>`<br>
_Notice: `<pseudo_user_name>` MUST be added in programm before running this `Python`-script !!!_<br>

This script sends messages to a non-existent user with a random set of characters at random intervals. Since an external observer (any user of the blockchain network) does not know WHO the message is being sent to (except the recipient, of course), this measure makes it possible to exclude the use of a timing attack.

:warning: _Note: Before using this messenger, it is strongly recommended that you familiarize yourself with the `i2pd` project and `yggdrasil`._

### :bookmark_tabs: Licence ###
BCM Linux-Console is [CC BY-NC-SA 3.0 licensed](./LICENSE).
