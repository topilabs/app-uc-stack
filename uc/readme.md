## Guidebook

Assumes all code libraries can be found at
/Users/ikellymo/code/libraries/
and this folder contains (And configured in .bash_profile)
- nanopb-0.4.7-macosx-x86 (NANOPB_PATH)
- pico-sdk (PICO_SDK_PATH)

Protobuf message contained in ProjectFiles/nanopb/message.proto

Install nanopb, then cd into this project directory (where this readme file is) and run

`% /Users/ikellymo/pico/nanopb-0.4.7-macosx-x86/generator/nanopb_generator.py ProjectFiles/nanopb/message.proto`

`% Writing to ProjectFiles/nanopb/message.pb.h and ProjectFiles/nanopb/message.pb.c`

move the blink.uf2 file over to the pico. 

## Todo

- Get the debugger/flasher working on the pico, with another pico
- implement cobs encoding/decoding that actually works.