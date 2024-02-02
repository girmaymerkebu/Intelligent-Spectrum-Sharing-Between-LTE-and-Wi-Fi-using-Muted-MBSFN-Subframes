Intelligent Spectrum Sharing Between LTE and Wi-Fi using Muted MBSFN Subframes
==============================================================================

This repository includes modifications included for implementation of adaptive muted MBSFN subrframes based on WiFi channel occupancy time obtained from technology recognition system and LTE traffic queue on the LTE eNB scheduler. Modifications are also implemented for periodic SIB generation from the eNB side and periodic SIB decoding from the UE side. The periodic SIB generation is used to adptiveley set certain number of muted MBSFN subframes.

# Installation and Execution Procedures
Procedures to test the solution:
  * Clone this repository using "git clone https://gitlab.ilabt.imec.be/mgirmay/adaptive-mbsfn.git".
  * Checkout to 'DSS' branch and install in atleast two host PCs .
  * Run the srsepc, srsmbms, and srsenb (with the enb.conf) in the eNB host PC.  
  * Run srsue (with the configuration file ue.conf) in the UE host PC .
  * Create MBMS gateway. 
  * Setup end-to-end Wi-Fi network on the same band as LTE.
  * Generate dynamic LTE unicast traffic and Wi-Fi traffic loads using iperf.
  * Trace the obtained LTE and Wi-Fi throughput to observe how the resourse allocation works adaptiveley as the LTE and WiFi traffic loads vary.

# Support
merkebutekaw.girmay@ugent.be 

# Reference

If you utilize the provided source code, please kindly cite the following research paper:

M. Girmay, et al., "[Enabling Uncoordinated Dynamic Spectrum Sharing Between LTE and NR Networks](https://ieeexplore.ieee.org/document/10313113)," IEEE Transactions on Wireless Communications, 2023.










