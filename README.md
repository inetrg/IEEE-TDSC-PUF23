# PUF for the Commons: Enhancing Embedded Security on the OS Level (submitted to TDSC 2023)

[![Paper][paper-badge]][paper-link]
[![Preprint][preprint-badge]][preprint-link]
[![Data][data-badge]][data-link]

This repository contains code and documentation to reproduce experimental results of the paper **"PUF for the Commons: Enhancing Embedded Security on the OS Level"** accepted for publication in IEEE Transactions on Dependable and Secure Computing 2023.

 **Abstract**
 > Security is essential for the Internet of Things (IoT). Cryptographic operations for authentication and encryption commonly rely on random input of high entropy and secure, tamper-resistant identities, which are difficult to obtain on constrained embedded devices.
In this paper, we design and analyze a generic integration of physically unclonable functions (PUFs) into the IoT operating system RIOT that supports about 250 platforms.  Our approach leverages uninitialized SRAM to act as the digital fingerprint for heterogeneous devices.
We ground our design on an extensive study of PUF performance in the wild, which involves SRAM measurements on more than 700 IoT nodes that aged naturally in the real-world. We quantify static SRAM bias, as well as the aging effects of devices and incorporate the results in our system. This work closes a previously identified gap of missing statistically significant sample sizes for testing the unpredictability of PUFs. Our experiments on COTS devices of 64 kB SRAM indicate that secure random seeds derived from the SRAM PUF provide 256 Bits-, and device unique keys provide more than 128 Bits of security.
In a practical security assessment we show that SRAM PUFs resist moderate attack scenarios, which greatly improves the security of low-end IoT devices.

Please follow our [Getting Started](getting_started.md) instructions for further information on how to compile and execute the code.

<!-- TODO: update URLs -->
[paper-link]:https://www.doi.org/10.1109/TDSC.2023.3300368
[preprint-link]:https://arxiv.org/pdf/2301.07048
[paper-badge]:https://img.shields.io/badge/Paper-IEEE%20Xplore-green
[preprint-badge]: https://img.shields.io/badge/Preprint-arXiv-green
[data-link]: https://doi.org/10.21227/axff-kc36
[data-badge]: https://img.shields.io/badge/Data-IEEE%20DataPort-blue
