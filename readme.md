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

## Todo

- Need to set-up build system to find <pb.h> based on bracket notation. 