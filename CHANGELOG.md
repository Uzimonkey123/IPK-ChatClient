# Changelog

## [1.0.0]

### Known issues
- UDP not always waiting for confirm before terminating connection
- TCP and UDP does not check the format of the incomming message
- Not checking for the length of the sent message

### 2024-03-30
#### Added
- **readme**: Adding TCP tests `7a89c31ef7`
- **readme**: Adding TCP class section `16a3999a87`
- **readme**: Adding TextFormatter part into the readme `0f6f14979f`

#### Fixed
- **fsm**: Fixing receiving REPLY OK/NOK `85ab3bf2dd`
- **fsm**: If server sends bye message, does not exit with error code `0a841374b2`

### 2024-03-29
#### Added
- **readme**: Adding more to structure of the project `c40c7cdb11`
- **readme**: Adding basics of readme `6989e95370`

#### Fixed
- **main**: Giving time for socket to close `e01ddd51f3`
- **formatter**: Fixing a single indent `e33ab63bd3`
- **tcp**: Fixing double close on tcp socket, causing RST flag `7e455ac411`
- **udp**: Fixing message ID order `515925ff4b`
- **tcp**: Fixing stdout flush for incoming MSG `d97fe875f1`

### 2024-03-28
#### Fixed
- **fsm**: If getting err in auth state, it sends back bye `a16978dba9`
- **formatter**: Handling invalid message from server `0430ab9e34`

### 2024-03-27
#### Added
- **udp**: Adding comments `ba0974b884`
- **tcp**: adding comments `ba3bcd0356`
- **lastMessage**: Adding comments `587091d7f1`
- **main**: Adding comments `c307858d70`
- **validator**: adding comments `36e9bf8cee`
- **strings**: adding comments `6ff6b803d4`
- **parser**: adding comments `11e5fe0666`
- **fsm**: adding comments `080520b703`

#### Fixed
- **udp**: Fixing amount of resending messages + adding file to makefile `8936937d1d`
- **udp**: refactoring last message, class creation and separation from main `21ffb8d40d`
- **udp**: timeout ending connection between the socket `ee3d3abda1`

### 2024-03-22
#### Added
- **fsm**: adding error handling and sending in OPEN state `aecda5bffe`

#### Fixed
- **formatter**: Separating class into header file `f56a7c90b2`
- **fsm**: separating FSM class into header file `5385ce1e94`

### 2024-03-21
#### Added
- **formatter**: splitting string related operations into separate class `cf370165bf`

#### Fixed
- **udp**: Fixing error codes in udp send and recv `8980eb6aa1`
- **tcp**: Fixing error return codes from tcp send and recv `ad097fc32f`
- **Main**: Refactoring code according to header files `a523e776b9`
- **Main**: ignoring now reply messages that aren't supposed to happen `0212b3fe2a`
- **fsm**: Changing so after auth, it waits for REPLY and MSG before continuing `f3acedcda2`
- **Main**: refactoring usleep at the end of program `6c4c7bbaf5`
- **Main**: usleep change `8d8f6a9b15`

### 2024-03-20
#### Added
- **Main**: Adding header files and updating Makefile `26129330b2`
- **Main**: Implementing `-help` and `/help` `be3417000d`

#### Fixed
- **udp**: memory free fix, messageID duplication fix `f23d4ba73c`

### 2024-03-19
#### Added
- **Main**: adding possibility to case-insensitive protocol for args `3a1d993ef0`

#### Fixed
- **udp**: changing back to big endian messageID `2d1a121b6f`
- **udp**: fixing messageID order for sent messages `382dc4cc82`
- **tcp**: fixing case-insensitivity according to the assignment rules `c92a5497e9`

### 2024-03-18
#### Added
- **udp**: adding basics for retransmission and delay handling `c6c1bd5f59`
- **Main**: adding comments to the code `db741242d5`

#### Fixed
- **Main**: fixing termination when server sends error and/or bye `34c7f29f00`
- **udp**: adding formatting for ERR messages `2781e53c30`
- **Main**: In START state cannot send bye and just terminates `118d8b161e`
- **Main**: Fixing reaction for CTRL+D, refactoring fsm for both protocols `d0ead9bfe4`
- **udp**: now udp can take files on stdin as input too, removing end state `19772faeb3`

### 2024-03-17
#### Added
- **Main**: adding basic udp fsm `eeb45e2b1d`
- **Main**: Adding message formatting for udp `2f95f15bd9`
- **Main**: Adding basics of joining for the udp, handling recv and sends, confirms etc.. `17b588f21c`

### 2024-03-15
#### Added
- **Main**: adding udp.cpp `85a4c86127`
- **Main**: Adding basic UDP send function and UDP class `661b45cb23`

#### Fixed
- **Main**: Removing sleep `6751daa13e`

### 2024-03-09
#### Added
- **Main**: implementing user bad command at given FSM states `4cd7ad2eca`

#### Fixed
- **Main**: error handling from user behaviour, reformating functions `fcbd525df2`
- **tcp**: Error handling when server terminates connection `6de2f3c0b4`

### 2024-03-08
#### Added
- **Main**: implementing basic Client-FSM `51b4743bf9`
- **Main**: adding fsm.cpp to the makefile `17231475f1`
- **Main**: proper usage of select and Client-FSM `81ca94d7f4`
- **Main**: Changing return codes from fsm, separation of parser `9e6d60d55a`

#### Fixed
- **Main**: changing fsm, to block while waiting for reply only `75a441912a`
- **tcp**: Adding null terminator at recv string `e23cfb768b`
- **Main**: Implementing the recv formater + exiting code for error `d0e5663397`
- **Main**: Adding function to format received communication `9dd829f2d5`
- **Main**: Adding basic text formatter for commands + Makefile `a5d7d1412f`
- **Main**: adding few commands for parsing, delay time for join `edba7af82c`
- **Main**: Implementing timeout for waiting the REPLY (not used yet) `4d65e2f69d`
- **Main**: allowing user to send BYE before REPLY arrives `224aeef8cb`
- **Main**: BYE is only sent by EOF or SIGINT from client side `e42b1119f8`

### 2024-03-07
#### Added
- **main.cpp**: Changing active waiting for select() `e718720c5f`
- **Main**: implementing tcp connection to the server `7753ba6fb5`
- **Main**: adding tcp.cpp to makefile `3e85ca1670`

### 2024-03-06
#### Added
- **Main**: basic makefile `40c5f29d57`
- **Main**: adding argument parsing at the start of the program `5093276d81`

### 2024-02-15
#### Added
- Initial commit `8e981138f4`
