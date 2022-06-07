<h1 align="center">
  BCM Android-Client
</h1>

## BCM Android-Client &middot; [![GitHub license](https://img.shields.io/badge/license-CC%20BY--NC--SA%203.0-blue)](./LICENSE) [![Python](https://img.shields.io/badge/python-3.6-red)](https://www.python.org/) [![Java](https://img.shields.io/badge/Java-SE8-blue)](https://www.java.com/) [![Android Studio](https://img.shields.io/badge/IDE-Android%20Studio-lightgrey)](https://developer.android.com/studio) [![Ecncryption](https://img.shields.io/badge/encryption-one--time%20pad-important)](https://en.wikipedia.org/wiki/One-time_pad#:~:text=In%20cryptography%2C%20the%20one%2Dtime,a%20one%2Dtime%20pad) [![LinkedIn](https://img.shields.io/badge/linkedin-Sergey%20Ivanov-blue)](https://www.linkedin.com/in/sergey-ivanov-33413823a/) [![Telegram](https://img.shields.io/badge/telegram-%40SergeyIvanov__dev-blueviolet)](https://t.me/SergeyIvanov_dev) ##

This branch of the repository contains the `Android` application code that allows you to connect to a node on a server running `Ubuntu` OS and communicate with it using encryption using one-time pads.

Through this `Android` application, you can remotely communicate with other participants in blockchain communications on behalf of a node running on a server with `Ubuntu` OS.

## :computer: Getting Started  ##

**Step 1**

1. Go to home directory and clone repository from github: `cd ~ && git clone https://SergeyIvanovDevelop@github.com/SergeyIvanovDevelop/Blockchain-Crypto-Messanger`

**Step 2**<br>

2. Open the `Android` application code from the `android-client` branch in `Android Studio` 

**Step 3**<br>

3. Build the project into an `apk` file

**Step 4**<br>

4. Place `apk` file in `Android` device

**Step 5**<br>

5. Install `Android` application by running `apk` file

**Step 6**<br>

6. Give the necessary permissions to the installed `Android` application (via settings)

**Step 7**<br>

7. Manually place the encryption key (file `Key.txt`) generated on the server with `Ubuntu` OS in the `Android` device in the directory `/storage/emulated/0/Pictures/Key.txt`

**Step 8**<br>

8. Run the `Android` application, enter the `IP` address and port of the server with `Ubuntu` OS, where the node with the messenger core is deployed, and click the `Connect` button

:warning: _To use the `Android` application, it is necessary periodically (as the current encryption key runs out) on the server side to generate a new encryption key `Key.txt` and copy it to the `Android` device in the directory `/storage/emulated/0/Pictures/Key.txt`_.

### :bookmark_tabs: Licence ###
BCM Android-Client is [CC BY-NC-SA 3.0 licensed](./LICENSE).
