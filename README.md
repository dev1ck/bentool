# bentool(Being Network Tool)

---

# Install
```
sudo apt update
sudo apt install scons
sudo apt install libpcap-dev
cd bentool/src
scons
```

---

# Usage
### [Syntax]
```
./bentool <option> [parameter]
```

### [Help Message]
```
./bentool -h
```

### [Print network interface information]
```
./bentool -i [<interface_name>]
```
    
### [ARP Scan]
```
./bentool -sA [-if <interface_name>]
./bentool -sA <target IP> [-if <interface_name>]
./bentool -sA <start IP> <end IP> [-if <interface_name>]
```

### [ICMP Scan]
```
./bentool -sP [-if interface_name]
./bentool -sP <target IP or Domain> [-if <interface_name>]
./bentool -sP <Network ID>:<prefix> [-if <interface_name>]
./bentool -sP <start IP> <end IP> [-if <intrface_name>]
```

### [TCP half open scan]
```
./bentool -sH [-p <port>] [-if <interface_name>]
./bentool -sH [-p <port> <end_port>] [-if <interface_name>]
./bentool -sH <target IP> [-p <port> [end_port]] [-if <inerface_name>]
./bentool -sH <start IP> <end IP> [-p <port> [end_port]] [-if <intrface_name>]
```

### [ARP Spoofing]
```
./bentool -pA <target IP> <host IP> [-if <inerface_name>]
```

### [Syn flood attack]
```
./bentool -aS <target IP or Domain/port>");
./bentool -aS <target IP> <port> [-l <attack_level>] [-if <interface_name>]
```

### [Wi-Fi AP scan]
```
./bentool -sW <interface_name> <[-c <Wi-Fi channel>]>
```

### [Wi-Fi deauth attack]
```
./bentool -aW <interface_name> <[-c <Wi-Fi channel>] or [-e <Wi-Fi SSID>]> 
```
