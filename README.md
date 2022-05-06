# An Adaptive MBSFN Resource Allocation Algorithm Between Multicast and Unicast Traffic
======================================================
# Procedures

This repository includes modifications included for periodic SIB generation from the eNB side and 
periodic SIB decoding from the UE side. The periodic SIB generation is used to adptiveley set MBSFN 
subframe allocation and periodicity values 


Procedures to test the Adaptive SF allocation solution:
  * Clone this repository using "git clone https://gitlab.ilabt.imec.be/mgirmay/adaptive-mbsfn.git".
  * Install in atleast two host PCs .
  * Run the srsepc, srsmbms, and srsenb (with the enb.conf) in the eNB host PC.  
  * Run srsue (with the configuration file ue.conf) in the UE host PC .
  * Create MBMS gateway 
  * Generate dynamic multicast and unicast traffic load using iperf.
  * Trace the obtained unicast and multicast throughput to observe how the resourse allocation works adaptiveley as the traffic load varies.
# Support
merkebutekaw.girmay@ugent.be , Ihtisham.Khalid@imec.be

=======================================================





