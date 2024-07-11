FROM jjangchan/cpp-tool:latest

CMD ["/bin/bash"]

COPY ./ ./source/

RUN rm -rf /var/lib/apt/lists/*
#RUN apt-get update -y
RUN cd ./source && mkdir build
RUN cd ./source/build && cmake .. && make
