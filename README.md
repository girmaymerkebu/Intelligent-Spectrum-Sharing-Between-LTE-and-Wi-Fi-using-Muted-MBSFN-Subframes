Adaptive SF Allocaction for Multicast/Unicast Traffic
======================================================

This repository includes modifications included for periodic SIB generation from the eNB side and 
periodic SIB decoding from the UE side. The periodic SIB generation is used to adptiveley set MBSFN 
subframe allocation and periodicity values 


Procedures to test the Adaptive SF allocation solution:
  * Clone the embms_fix branch from this repository using "git clone -b embms_fix git@gitlab.ilabt.imec.be:mgirmay/adaptive-sf-allocation-multicast_unicast.git"
  * Install in two PCs (eNB and UE)
  * Run srsenb and srsue with enb.conf and ue.conf configuration files respectively
  * Generate iperf traffic and trace 
  * Change the "subframe_allocation" and "Allocation_period"  values in "srsran/srsenb/mnsfn_set.txt" file and observe the iperf traffic changes accordingly

