# x86_32emulator

- ## Background

  This project is derived from <u>NJU's programming assignment</u>. In which I implemented a simple but complete x86-arch emulator and a micro system,  aiming at getting fully understanding about the subtle details with computer systems in a programmer's perspective.

- ## Introduction

  - ### Overall Architect

    | Overall Architect |                                 |
    | ----------------- | :-----------------------------: |
    | navy-apps         |          applications           |
    | nanos-lite        |     micro operation system      |
    | nexus-am          |        abstract machine         |
    | nemu              | x86-arch machine implementation |
    | init.sh           |          Set env args           |

- ### NEMU

  - #### Introduction

    Nemu is a rudimental implementation of x86 computer. Its basic function is constantly repeat the process of fetching, decoding, executing instructions and update PC.

    In addition, we implanted an embedded debug system which may come in handy in future days.

  - #### Features

    **Decoupled processing of instructions.**

     Take an essential instructions set, mov, as an example. In x86-isa, mov includes a lot of specific instructions.![image-readme1](http://github.com/huxy11/x86_32emulator/raw/master/readmerc/readme1.png)

    As above, there are a variety of specific instructions. Different mov instruction has different operands but the same execution(move data from one operand to another). Thus we could abstract two steps and decouple them respectively into:

    decoding: I2r, E2G, G2E, etc.

    execution: mov

    **Embedded gdb-like debug tool.**

    One advantage of emulator is that we are able to monitor every details within the "machine". Therefore it is not of difficulty to build a tool facilitating inspection of machine status during the runtime.
    
    | Commonds |                                                   |
    | -------- | ------------------------------------------------- |
    | help     | Display informations about all supported commands |
    | c        | Continue to execute the program                   |
    | si       | Execute once                                      |
    | i        | Display info about(regs, watch points etc.)       |
    | x        | Scan memory                                       |
    | reg      | pring regs' name                                  |
    | expr     | Calculate expression                              |
    | wp       | Set up a watch point                              |
    | dwp      | Remove the designated watch point                 |
    | sf       | Print stack frame                                 |
    | pd       | print page direction % tables                     |
    | test     | For test only                                     |
    | q        | Exit nemu                                         |

  

  - NEXUS-AM
  - NANOS-LITE
  - NAVY-APPS

- ## Install

- ```bash
  bash init.sh
  source ~/.bashrc
  ```

  
