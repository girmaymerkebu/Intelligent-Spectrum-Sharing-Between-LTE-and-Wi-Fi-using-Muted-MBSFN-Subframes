# An Adaptive MBSFN Resource Allocation Algorithm Between Multicast and Unicast Traffic
======================================================

This repository includes modifications included for periodic SIB generation from the eNB side and 
periodic SIB decoding from the UE side. The periodic SIB generation is used to adptiveley set MBSFN 
subframe allocation and periodicity values 


Procedures to test the Adaptive SF allocation solution:
  * Clone this repository using "git clone https://gitlab.ilabt.imec.be/mgirmay/adaptive-mbsfn.git".
  * Install in two host PCs (eNB and UE).
  * Run the srsepc and srsmbms 
  * Run srsenb and srsue with enb.conf and ue.conf configuration files respectively.
  * Create MBMS gateway and generate multicast and unicast traffic using iperf.
  * Trace the obtained unicast and multicast throughput to observe how the resourse allocation works adaptiveley.

=======





