# bentool(Being Network Tool)

간단한 명령어로 쉽게 사용 가능한 Linux Network tool

<aside>
💡 Bentool은 호스트 및 액세스 포인트(AP) 정보를 수집하기 위해 ARP 스캔, ICMP 스캔, TCP 스캔, Wi-Fi 스캔 등의 다양한 네트워크 스캐닝 기능을 제공합니다. 또한 ARP 스푸핑, SYN 플러딩, Wi-Fi 재밍 등의 네트워크 공격 기법을 실행할 수 있는 Network tool로 개발되었습니다. 이 tool은 네트워크의 기본 원리, OSI 7계층의 헤더 구조 및 네트워크 보안에 대한 심층적인 지식을 습득하고자 하는 목적으로 시작하였습니다.
</aside>

### 시연 영상

[https://www.youtube.com/watch?v=LfIhu9Ai0K8&t=5s](https://www.youtube.com/watch?v=LfIhu9Ai0K8&t=5s)

<br/>

## 🛠️ 사용 기술 및 라이브러리

- C
- Kali Linux
- pcap, pthread

<br/>

## ⚙️ 주요 기능

![슬라이드8](https://github.com/dev1ck/bentool/assets/96347313/15c69642-d3b0-4cb7-851a-fe8ad73af3a8)

- ARP Request 패킷을 통한 로컬 네트워크 내 살아있는 host scan
- ICMP echo request 패킷을 통한 지정한 범위 내 살아있는 host scan
- TCP half open scan을 통한 호스트 내 열려있는 포트 scan
- ARP spoofing을 통한 victim과 지정한 호스트 간의 통신 스니핑
- Syn flood 공격을 통한 DoS 공격
- 주변 Wi-Fi (AP) 스캔
- 지정한 채널, 혹은 지정한 SSID로의 Wi-Fi 재밍 공격

<br/>

## ⭐차별성

![슬라이드21](https://github.com/dev1ck/bentool/assets/96347313/3addd075-5c4a-4efc-8707-d53c54dae055)

![슬라이드25](https://github.com/dev1ck/bentool/assets/96347313/fa03ad16-1c8c-4e26-a624-2772a2ae0567)

<br/>

---

<br/>

# How To Use

## Install

```
sudo apt update
sudo apt install scons
sudo apt install libpcap-dev
cd bentool/src
scons
```

## Usage

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
./bentool -aS <target IP or Domain/port>
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
