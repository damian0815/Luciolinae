For each slave:

1. Burn FUSE to set clock speed high:

$ make burn-fuse

2. Burn program to FLASH:

$ make program-rs485_protocol_dev

3. Set address in EEPROM:

$ ./burn_eeprom 0x10
(or 0x20, 0x30, ... )
