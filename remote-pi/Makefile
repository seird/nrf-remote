build:
	arm-linux-gnueabihf-g++ -marm -march=armv6zk -mtune=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard -Ofast -Wall -pthread  -DRF24_NO_INTERRUPT -DUSE_RELAY -I/usr/local/include/RF24/.. -I.. -L/usr/local/lib src/crypto/aes.cpp src/crypto/crypto.cpp src/packet/packet.cpp src/radio/radio.cpp src/transmitter/main.cpp -lrf24 -o remote

run: build
	./remote
