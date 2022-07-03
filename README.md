# PC Remote

A simple remote to turn on a pc (or anything with physical access to the power button leads) with 2 raspberry pi picos or a pi + pico.


## [Pico Remote](remote-pico)

A pico transmitter and receiver


## [Pi Remote](remote-pi)

The transmitter part to connect to a pico receiver


## Requirements

The power on command is authenticated with a CBC-MAC. Add `src/key.h` with a 16 byte PRESHARED_KEY_ENCRYPTION and PRESHARED_KEY_AUTH, e.g.:

```
static const uint8_t PRESHARED_KEY_ENCRYPTION[] = {
    0xF3, 0x75, 0x52, 0x64, 0x9E, 0x9D, 0xD1, 0x51,
    0x97, 0x49, 0x0D, 0x80, 0x9A, 0x8F, 0x0B, 0xA0
};


static const uint8_t PRESHARED_KEY_AUTH[] = {
    0x76, 0x4E, 0x79, 0x50, 0x1E, 0x68, 0x61, 0xCF,
    0x3F, 0x64, 0xDD, 0xEE, 0x5B, 0x41, 0xC3, 0x19
};
```


## Libraries / code used

- https://github.com/kokke/tiny-AES-c
- https://ghubcoder.github.io/posts/awaking-the-pico/
- https://nrf24.github.io/RF24/md_docs_pico_sdk.html
