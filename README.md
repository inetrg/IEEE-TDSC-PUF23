# PUF for the Commons: Enhancing Embedded Security on the OS Level (submitted to TDSC 2022)

[![Paper][paper-badge]][paper-link]
[![Preprint][preprint-badge]][preprint-link]

This repository contains code and documentation to reproduce experimental results of the paper **"PUF for the Commons: Enhancing Embedded Security on the OS Level"** submitted to IEEE Transactions on Dependable and Secure Computing 2022.

 **Abstract**
 > Cryptographic procedures for authentication or encryption commonly rely on random input of high entropy and secure, tamper-resistant identities, which are difficult to obtain on embedded devices.
In this paper, we design a generic integration of physically unclonable functions (PUFs) for embedded operating systems.  Our
approach leverages uninitialized SRAM to act as digital fingerprint for
heterogeneous devices.
We implement and analyse our approach on the popular open-source IoT operating system RIOT, which supports more than 200 platforms.
Prior work identified the need for a high number of samples when the unpredictability of PUFs is tested.
Conducted on >700 nodes, our (reproducible)
experiments show that seeds and keys derived from PUFs provide security
guarantees in moderate attack scenarios. We quantify the resource
overhead and find a notable startup-delay, which we can mitigate for intermediate
sleep cycles of nodes.

Please follow our [Getting Started](getting_started.md) instructions for further information on how to compile and execute the code.

<!-- TODO: update URLs -->
[paper-link]:https://github.com/inetrg/IEEE-TDSC-PUF22
[preprint-link]:https://github.com/inetrg/IEEE-TDSC-PUF22
[paper-badge]:https://img.shields.io/badge/Paper-IEEE%20Xplore-gray
[preprint-badge]: https://img.shields.io/badge/Preprint-arXiv-gray