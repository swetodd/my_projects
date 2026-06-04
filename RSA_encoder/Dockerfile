FROM frolvlad/alpine-gxx
RUN apk add --no-cache cmake make
COPY . /project
WORKDIR /project
RUN mkdir build && cd build && cmake .. && make

WORKDIR /project/build

#check README for buiding image and starting container 


#need to add string of building project through CMAKE as without it alpine can raise an error cause of compatibility