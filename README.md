# ICS-emulator

- ## Background

  This project is derived from <u>NJU's programming assignment</u>. In which I implemented a simple but complete x86-arch emulator and a micro system,  aimming at getting fully understanding about the subtle details with computer systems in a programmer's perspective.

- ## Introduction

- ### Overall Architect

  - | Overall Architect |                                 |
    | ----------------- | :-----------------------------: |
    | navy-apps         |          applications           |
    | nanos-lite        |     micro operation system      |
    | nexus-am          |        abstract machine         |
    | nemu              | x86-arch machine implementation |
    | init.sh           |          Set env args           |

- ### NEMU

  Nemu is a implementation of actual x86 computer. Technically, it is also a Turing Machine(TRM). Which consists of Program Counter(PC), registers(regs), memory and Compute units. It works by continuatively fetch instructions stored in memory and pointed by PC, decode and execute the instruction and update PC.

  Futhermore, as it was built with our own hands, we can easily embed a monitor to inspect every aspect of nemu's status. The monitor's commonds is defined in src/monitor/debug/ui.c as below.

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

  
