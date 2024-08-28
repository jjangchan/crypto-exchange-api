# crypto-exchange-api



## install packege

- gcc 9.4.0
- cmake 3.22.3
- Ninja 1.10.0
- boost 1.79.0
- OpenSSL 1.1.1
- uuid-dev



## Build with Docker

```shell
# Build Dockerfile
docker build . -t crypto-api

# Run Docker
docker run -it --name {cotainer_name} crypto-api

# Built Path
/source/
|---- CMakeLists.txt
|---- Dockerfile
|---- include
|---- src
|---- unittest
|---- build
| |---- unittest
| |   |-- unittest          #exexute file
| |---- crypto_exchange_api #execute file
|
|-- README.md
```



## Run unittest

4개의 암호화폐 거래소(바이낸스, 빗썸, 코인베이스, 업비트)에 1분봉 캔들 데이터를 받아오고, 1분봉 기준으로 1분동안 데이터 신호를 받아오는 테스트

```	shell
cd /source/build/unittest
./unittest
```





