
## Organization of this Folder


### Operating System
[RIOT](https://github.com/inetrg/RIOT/tree/TDSC22) points to our fork of the operating system RIOT repository. We base our work on the [Release-2022.01](https://github.com/RIOT-OS/RIOT/releases/tag/2022.01) and apply changes on top of it.
Due to the OS integration there are many changes included, however, the most fundamental contributions are located in *RIOT/sys/*.

To view all changes, see [this diff](https://github.com/inetrg/RIOT/commit/51906f03d932e15f6bed37a7a5753cbc030aa6d8).


### Measurement Applications
Three measurement applications are located in [apps](apps/). Each of the applications contains a *main.c* file that implements the application logic, and a *Makefile* to set up the build dependencies. Note that these applications utilize the RIOT version introduced above.
Additionally, an application folder includes one or multiple **.elf* files (compiled for the [`M3`](https://www.iot-lab.info/docs/boards/iot-lab-m3/) platform) that allow for experiment deployment using default configurations without the need to compile yourself.

## Prerequisites

## Getting the Code

Clone this repository and navigate to the root directory. Note, you need to set up your GitHub properly in order to clone vis SSH.

```
git clone git@github.com:inetrg/IEEE-TDSC-PUF22.git && cd IEEE-TDSC-PUF22
```

Update the git submodule:
```
git submodule update --init --recursive
```

### Compile Applications in RIOT
This is **not required** when using the **.elf* files provided in the application folders.
In order to cross-compile applications for RIOT, a set of standard tools needs to be present on your machine. We recommend a Linux based environment when compiling RIOT. For further information about the build environment, we refer to the [RIOT Getting Started Guide](https://doc.riot-os.org/getting-started.html). The most convenient alternative among the options is the [Use Docker to build RIOT](https://doc.riot-os.org/getting-started.html#docker) option.

Once the tools are ready, navigate into an application folder and type `make`. This starts compilation and creates a *bin/* folder which contains the compiled *.elf* file on success.

For specific experiments you need to set compiler flags for configuring the system (e.g., the Fuzzy Extractor). Set the compiler flags before the make command, i.e. like:

`CFLAGS=-D<FLAGNAME>=1 make`

You can also set the flags in a respective Makefile.

### Execute Applications on the FIT IoT-LAB Testbed

The open access [FIT IoT-LAB Testbed](https://www.iot-lab.info/) provides a variety of hardware platforms that can be allocated for experimentation; the majority of which is represented by the [`M3`](https://www.iot-lab.info/docs/boards/iot-lab-m3/) nodes.

Every user must create a [user account](https://www.iot-lab.info/docs/getting-started/user-account/) initially. There is a wide collection of tools and learning material on how to deploy experiments on the testbed. We refer to the [testbed documentation](https://www.iot-lab.info/docs) and recommend to follow the [Getting Started Tutorial](https://www.iot-lab.info/legacy/tutorials/getting-started-tutorial/index.html) to familiarize with the testbed.

Each of our measurement applications prints to the console which can be displayed in the terminal of the testbed Webportal, or using [aggregation tools](https://www.iot-lab.info/legacy/tutorials/serial-aggregator/index.html) which facilitates the logging of outputs.

## Information on the Provided Examples

To reproduce the paper results, please note that the experiments need to be deployed on large quantities of nodes and for a very long time, e.g., to be certain about remaining bit errors. When doing this, note that
- Logging of all outputs easily produces gigabytes of data. This quickly exceeds a users disk quota. It is advisable to compress the outputs directly when writing into a file.
- The testbed is a shared resource, hence, be fair with resource allocation and familiarize yourself with the [terms of use](https://www.iot-lab.info/docs/getting-started/terms-of-use/) beforehand. Consider announcing large experiments on the FIT IoT-LAB mailing list.

### [1. Analysis of Uninitialized SRAM](apps/print_sram/)

The program prints the values of raw and uninitialized memory directly after startup.
It starts a wakeup timer of five seconds, and sets the device to standby mode (complete SRAM is powered off), to obtain fresh memory pattern iteratively. RIOT starts from the `reset_handler` again after wakeup by the timer.

For practical reasons, we split the execution into two 32kB blocks. Note that this is still a lot of output. When using the terminal of the Webportal, it sometimes quits and complains "nodes sending too fast". Use the *Serial Aggregator* instead.

#### **Configuration**

Change the sleep time of the device. Currently, we only implement a resolution of seconds. Default is 5 seconds here.
```
CFLAGS=-DSTANDBY_TIME_S=5
```

Print first half of memory.
```
CFLAGS=-DSRAM_PRINT_FIRST=1
```

Print second half of memory.

```
CFLAGS=-DSRAM_PRINT_LAST=1
```


#### **Expected Output**
```
main(): This is RIOT! (Version: 2021.10)
CPUID;0x32ffdc053348323630800343
MCUID;0x00000000
memory
93aae3fb
57953173
8474d3ff
...
```

### [2. Analysis of Seeds](apps/print_seeds/)
The program prints general purpose and crypto purpose seeds. Similar to the first application, it puts the device to sleep and wakes it up periodically, with a sleep time of one second.

Note that the first run after flashing a device informs about `PUF SEED not fresh`. This can also be triggered by a manual reset of the device during operation, and indicates the vitality of our soft-reset detection mechanism.


#### **Configuration**

Change the sleep time of the device. Currently, we only implement a resolution of seconds.
```
CFLAGS=-DSTANDBY_TIME_S=1
```

Change the length of SRAM memory considered for creating the general purpose seed. Default is 128 Bytes.
```
CFLAGS=-DPUF_SRAM_SEED_RAM_LEN=128
```

Change the length of SRAM memory considered for creating the crypto purpose seed. Default is 1024 Bytes.
```
CFLAGS=-DPUF_SRAM_SEED_RAM_LEN_SECURE=1024
```

#### **Expected Output**
```
random: PUF SEED not fresh
main(): This is RIOT! (Version: 2021.10)
STANDBY_TIME_S;1
PUF_SRAM_SEED_RAM_LEN;128
PUF_SRAM_SEED_RAM_LEN_SECURE;1024
seed;EE8B5C9F
seedsecure;eb1655723773a98c56dcc1f0efacadad7db134657d5d3a4e79d1f1dd401debd7
main(): This is RIOT! (Version: 2021.10)
STANDBY_TIME_S;1
PUF_SRAM_SEED_RAM_LEN;128
PUF_SRAM_SEED_RAM_LEN_SECURE;1024
seed;70B7FB62
seedsecure;33c8a34da5d21239618e6a7623700a6c98df1ff9429c8aca550eacd49e8b928a
main(): This is RIOT! (Version: 2021.10)
STANDBY_TIME_S;1
PUF_SRAM_SEED_RAM_LEN;128
PUF_SRAM_SEED_RAM_LEN_SECURE;1024
seed;8ACFD435
seedsecure;990e794ea741bb8ceac23af014e54072d72c2069cc89337b15f1866f4449ced6
...
```

### [3. Analysis of Secret IDs](apps/print_ids/)
The program(s) implement the enrollment and reconstruction to create a secret reliably, on repeated noisy SRAM measurements. To tun the experiment, you need to flash the node three times (see configuration below).
First, flash the firmware that clears the non-volatile memory on the `M3` node to ensure a clean start.
Second, flash the enrollment firmware that takes a reference measurement, calculates public helper data, and places it on the non-volatile memory. The involved intermediate data is printed (see Expected Output below).
Third, the flash the reconstruction firmware which utilizes the helper data to reproduce the PUF measurement, and the secret respectively. It prints the relevant data involved by the Fuzzy Extractor, and performs the same sleep/wakeup routine as described above.
Note that the first iteration of the reconstruction after flashing the MCU returns bad results. This is because the SRAM requires a proper power-off cycle which is just given after the first iteration of the program.

#### **Configuration**

Only **one** of the following flags **must** be set to 1 at a time. Either to clear the non-volatile memory, to enroll the helper data, or reconstruct the PUF response, assuming a valid set of helper data is in place.
```
CFLAGS+=-DSRAM_PUF_ID_CLEAR=1
CFLAGS+=-DSRAM_PUF_ID_ENROLL=1
CFLAGS+=-DSRAM_PUF_ID_RECONSTRUCT=1
```

Change the length of the random secret (code offset) at the input of the Fuzzy Extractor during enrollment. Default is 18 Bytes.
```
CFLAGS+=-DPUF_SRAM_CODEOFFSET_LEN=18
```

Change the number of repetitions by the repetition error correction code. Default is 7.
```
CFLAGS+=-DECC_REPETITION_COUNT=7
```

#### **Expected Output `SRAM_PUF_ID_CLEAR`**

```
random: PUF SEED not fresh
main(): This is RIOT! (Version: 2021.10)
erase_helper;
```
#### **Expected Output `SRAM_PUF_ID_ENROLL`**
```
random: PUF SEED not fresh
main(): This is RIOT! (Version: 2021.10)
read marker; ffffffff
PUF_SRAM_CODEOFFSET_LEN;18
PUF_SRAM_GOLAY_LEN;36
PUF_SRAM_REP_COUNT;7
PUF_SRAM_HELPER_LEN;252
STANDBY_TIME_S;1
erase_helper;
_setup_config;
Now trigger standby;
main(): This is RIOT! (Version: 2021.10)
read marker; affecafe
calculate and save new helperdata;
idEnroll;aa5a1b19c90d3f16e9732e5df0ad9f2e9aa02d11f6c040d28c13e34a1502f542
codeEnroll;2b35ea94fc489f95020ec9d5a4ae4a34fc61
helperEnroll;59282934d0ff57d0100dec3ba04e4b7292ecfed733278a8cf56dbf59c2eea6cb642954e0716f09efd8e7b6fded317ed821f35bc38ca02a4811274c006125d1ac991b3d42dc3c7e0a506d52a0c6fef4563d9cc7fec96e7496e9e801861a4434c87c5fb6d74dd2f63842443702ff334a2988d3b9df292201fe0b6173474b1a7c65d2c14e38e124dbf4694b9bd4e6d9894b9cdd9bcd607d37883265db148f3f32985db15a3aa548a8d5683f21a32b7c7ad70d60648babe8463d6f63a35c261bb6aacad1f8aa43c0d33e04098e77294e7a7ee029a142b9d9776d9e8a0a5a407d0dfd1c4d5d59655b0906f75a190c2d8e2ac9ebfe24dc88e4b3add4a83400
pufEnroll;eb7a9a4a0515bf595f9560739d97b25ec7ee0fe7dfdc43590c77f5bd6ca4fbc82b2b9881c33dba910d0d5e74a2a9f2901c2aa2efd9a2db78fddc85d5983f9b4837516041933eb26be23fe1de13141cdf72044bb6f4b78dbabceaf0b6f6bffd1d8545fc33e398ab3b0d46fb634d61f9575d39515666ba8db636b88a6b1e188d553e3a87ed183e9110c701c6d7a9db452a2e8f28b3b597df017dfd575cb2e6cbb408b3ab0a49b3610091256b47853627d44262a8ea19baf543ba894bd569833ae2f708018616c2220ee8f247a2d054309a4e63fc41f6dbbb0c2cd8b9249597e57453d5d1115882f02aa258e83cc175e31c12e46e3826aeeeae9baaf861
helper data already exists;
```
#### **Expected Output `SRAM_PUF_ID_RECONSTRUCT`**
```
random: PUF SEED not fresh
main(): This is RIOT! (Version: 2021.10)

idReconstruct;ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
codeReconstruct;000000000000000000000000000000000000
pufReconstruct;00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000232d302b2000686c4c0065666745464700303132333435363738394142434445460030313233343536373839616263646566000080100020010101016c0600200000000088300008a830000868300008000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000021000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
debug_measReconstruct;02460020c0430a4e0b4600240de0115d090648400025002802da4000704000e040000135082df6d101349c42efd100beb71dc10400205060304600be7c0000207c000020ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff0f0c01400e0c01400d0c01400b080140004000000000000000380140004000000a0801400908014001250000004400400000020003080140020801400026000072616e646f6d3a205055462053454544206e6f742066726573680a006c0600200000000000000000000000000000000000000000000000000000000000000000
main(): This is RIOT! (Version: 2021.10)

idReconstruct;aa5a1b19c90d3f16e9732e5df0ad9f2e9aa02d11f6c040d28c13e34a1502f542
codeReconstruct;2b35ea94fc489f95020ec9d5a4ae4a34fc61
pufReconstruct;eb7a9a4a0515bf595f9560739d97b25ec7ee0fe7dfdc43590c77f5bd6ca4fbc82b2b9881c33dba910d0d5e74a2a9f2901c2aa2efd9a2db78fddc85d5983f9b4837516041933eb26be23fe1de13141cdf72044bb6f4b78dbabceaf0b6f6bffd1d8545fc33e398ab3b0d46fb634d61f9575d39515666ba8db636b88a6b1e188d553e3a87ed183e9110c701c6d7a9db452a2e8f28b3b597df017dfd575cb2e6cbb408b3ab0a49b3610091256b47853627d44262a8ea19baf543ba894bd569833ae2f708018616c2220ee8f247a2d054309a4e63fc41f6dbbb0c2cd8b9249597e57453d5d1115882f02aa258e83cc175e31c12e46e3826aeeeae9baaf861
debug_measReconstruct;e9388a4a85153f595f1561739d97f25fc7ee0de7dfdc4b790c76f1bd64a4fbc83d2b9881c3bdba950d0c5e54a2a9f2905c2a22ebd9a29b70ed5c85d5983f93483f516441933eb36be23de1de133614df320c6bb6f4b68dbabcaef0b6f6bffd1d8d05ec33e39aabbb0d46f3034d61b9575939515666ba8db53eb88afb1e188d053e3a86ed183f9150c7014697a9d145aa2ecf20b3b5975b217dfd575cb2e6cbb408b3ab2a41b3250091256947853627d40266acea19bef563ab894bd56b8b3ae2f749018616c2220ee8f247a2d05634924e63fc41f6db3b5c2cd8b9249596e57453d5c111d082b02aa268e83cc175e30c93e04e3806aeeaae9b2a7873
main(): This is RIOT! (Version: 2021.10)

idReconstruct;aa5a1b19c90d3f16e9732e5df0ad9f2e9aa02d11f6c040d28c13e34a1502f542
codeReconstruct;2b35ea94fc489f95020ec9d5a4ae4a34fc61
pufReconstruct;eb7a9a4a0515bf595f9560739d97b25ec7ee0fe7dfdc43590c77f5bd6ca4fbc82b2b9881c33dba910d0d5e74a2a9f2901c2aa2efd9a2db78fddc85d5983f9b4837516041933eb26be23fe1de13141cdf72044bb6f4b78dbabceaf0b6f6bffd1d8545fc33e398ab3b0d46fb634d61f9575d39515666ba8db636b88a6b1e188d553e3a87ed183e9110c701c6d7a9db452a2e8f28b3b597df017dfd575cb2e6cbb408b3ab0a49b3610091256b47853627d44262a8ea19baf543ba894bd569833ae2f708018616c2220ee8f247a2d054309a4e63fc41f6dbbb0c2cd8b9249597e57453d5d1115882f02aa258e83cc175e31c12e46e3826aeeeae9baaf861
debug_measReconstruct;e9388a4a85153f595f1561739d97f25fc7ee0de7dfdc4b790c76f1bd64a4fbc83d2b9881c3bdba950d0c5e54a2a9f2905c2a22ebd9a29b70ed5c85d5983f93483f516441933eb36be23de1de133614df320c6bb6f4b68dbabcaef0b6f6bffd1d8d05ec33e39aabbb0d46f3034d61b9575939515666ba8db53eb88afb1e188d053e3a86ed183f9150c7014697a9d145aa2ecf20b3b5975b217dfd575cb2e6cbb408b3ab2a41b3250091256947853627d40266acea19bef563ab894bd56b8b3ae2f749018616c2220ee8f247a2d05634924e63fc41f6db3b5c2cd8b9249596e57453d5c111d082b02aa268e83cc175e30c93e04e3806aeeaae9b2a7873
...
```

## A Note on Local Experiment Execution on `ESP32` and `HiFive`

We recommend to follow the steps above and execute the experiments on the FIT IoT-LAB Testbed using M3 nodes. Otherwise, please note the following.

Our test application [1. Analysis of Uninitialized SRAM](apps/print_sram/) is currently tied to the M3 (`iotlab-m3`) platform. Applications [2. Analysis of Seeds](apps/print_seeds/) and [3. Analysis of Secret IDs](apps/print_ids/) can be executed locally on ESP32 (`esp32-wroom-32`) and HiFive (`hifive1b`) boards. There are, however, some caveats due to missing features.

### Local Compilation and Flashing
In agreement with [RIOT Getting Started Guide](https://doc.riot-os.org/getting-started.html), building firmware requires compilers and tools for flashing. In all generality, two steps are required to bring RIOT firmware to a board (locally).

1. Navigate to the application folder (e.g., `cd apps/print_seeds`).
2. Compile, flash, and monitor output: `BOARD=<boardname> make flash term`

The `<boardname>` for ESP32 is `esp32-wroom-32` and for HiFive is `hifive1b`.

### Analysis of Seeds on `HiFive`
The HiFives lack an automatic hard-reset trigger. Hence, it will not print seeds iteratively (contrasting other boards) and rely on an external power-off cycle. 
this can be achieved by operating it with an external power supply and disconnecting it intermittently. Note, disconnecting the USB cable used for flashing is not useful, since the serial connection is lost and require re-connection. We used an external FTDI here.

### Analysis of Secret IDs on ESP32 and HiFive

- Navigate to *RIOT/tests/puf_sram/*.
- Compile and flash the application.
  `CFLAGS="-DPUF_SRAM_GEN_HELPER=1 -DSRAM_PUF_ID_ENROLL=1" BOARD=<boardname> make flash`

- There is a build target which assist in off-device helper data generation. It requires a power-off cycle.
  - Boards that provide this (ESP32) simply do it and our tool awaits the reference PUF measurement: `make puf-helpergen`
  - Boards that do not provide this (HiFive) must provide the hard-reset otherwise (see Analysis of Seeds above). In this case, we rely on an FTDI which controls an external power supply for the HiFive: `REPOWER=ftdi make puf-helpergen`
- This procedure generates an individual header file (*sys/puf_sram/helperdata_generated.h*) which will be utilized in the next step.
- Regular firmware can reconstruct the PUF using this helper data, e.g., navigate to the test application: `cd apps/print_ids`
- Compile and flash the application (note differing flags): `CFLAGS="-DHELPER_FROM_FILE=1 -DSRAM_PUF_ID_RECONSTRUCT=1" BOARD=<boardname> make flash term`
- Boards without automatic hard-reset features require manual reset.



