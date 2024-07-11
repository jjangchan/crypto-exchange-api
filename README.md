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
docker build . -t jjangchan/cpp-tool

# Run Docker
docker run -it --name {cotainer_name} jjangchan/cpp-tool

# Built Path
/source/
|-- CMakeLists.txt
|-- Dockerfile
|-- include
|-- src
|-- unittest
|-- build
|	|-- crypto_exchange_api # execute file
|-- README.md
```

