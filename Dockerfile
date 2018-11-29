FROM nvidia/cuda
ADD . /prog
WORKDIR /prog
RUN nvcc -o prog matrixCuda.cu Linear.cpp main.cpp
CMD ["./prog"]