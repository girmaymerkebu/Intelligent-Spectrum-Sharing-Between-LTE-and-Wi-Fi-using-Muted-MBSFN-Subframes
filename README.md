# Adaptive Empty MBSFN Subframe Allocation to Enable LTE-WiFi Coexistence
==========================================================================
# Installation and Execution Procedures

This repository includes modifications included for implementation of adaptive empty MBSFN subrframes based on WiFi channel occupancy time obtained from technology recognition system and LTE traffic load. Modifications are also implemented for periodic SIB generation from the eNB side and 
periodic SIB decoding from the UE side. The periodic SIB generation is used to adptiveley set MBSFN 
subframe allocation and periodicity values.


Procedures to test the solution:
  * Clone this repository using "git clone https://gitlab.ilabt.imec.be/mgirmay/adaptive-mbsfn.git".
  * Checkout to 'DSS' branch and install in atleast two host PCs .
  * Run the srsepc, srsmbms, and srsenb (with the enb.conf) in the eNB host PC.  
  * Run srsue (with the configuration file ue.conf) in the UE host PC .
  * Create MBMS gateway 
  * Generate dynamic LTE unicast traffic load using iperf.
  * Trace the obtained LTE throughput to observe how the resourse allocation works adaptiveley as the LTE traffic load and WiFi transmission vary.
# Support
merkebutekaw.girmay@ugent.be 

=======================================================





