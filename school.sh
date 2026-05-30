export CUDA_HOME=/usr/local/cuda
export PATH=$CUDA_HOME/bin:$PATH
export LD_LIBRARY_PATH=$CUDA_HOME/lib64:$LD_LIBRARY_PATH

sed -i 's|-L/opt/cuda/lib64|-L/usr/local/cuda/lib64|g' Makefile