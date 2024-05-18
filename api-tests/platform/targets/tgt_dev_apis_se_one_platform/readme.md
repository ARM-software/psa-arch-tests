Building:

cd api-tests
mkdir <build_dir>
cd <build_dir>
cmake ../ -G"Unix Makefiles" -DTARGET="tgt_dev_apis_se_one_platform" -DCPU_ARCH=armv8m_ml -DSUITE=CRYPTO -DPSA_INCLUDE_PATHS="/home/user/zephyrproject/modules/crypto/mbedtls/include"
cmake --build .
