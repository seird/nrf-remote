### Receiver

- pico
- nrf24l01
- a relay in parallel with the power button


### Transmitter (remote)

- pico
- nrf24l01
- a button


### Relay

To extend the range between the receiver and transmitter, set `USE_RELAY=0` to `1` in CMakeLists.txt.

- pico
- nrf24l01
