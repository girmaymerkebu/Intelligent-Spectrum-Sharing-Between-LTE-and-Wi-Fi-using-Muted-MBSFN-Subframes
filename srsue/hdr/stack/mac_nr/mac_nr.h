/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSUE_MAC_NR_H
#define SRSUE_MAC_NR_H

#include "mac_nr_interfaces.h"
#include "proc_bsr_nr.h"
#include "proc_ra_nr.h"
#include "proc_sr_nr.h"
#include "srsran/common/block_queue.h"
#include "srsran/common/mac_pcap.h"
#include "srsran/interfaces/mac_interface_types.h"
#include "srsran/interfaces/ue_nr_interfaces.h"
#include "srsran/srslog/srslog.h"
#include "srsue/hdr/stack/mac_nr/mux_nr.h"
#include "srsue/hdr/stack/ue_stack_base.h"

namespace srsue {

class rlc_interface_mac;

struct mac_nr_args_t {
};

class mac_nr final : public mac_interface_phy_nr,
                     public mac_interface_rrc_nr,
                     public mac_interface_proc_ra_nr,
                     public mac_interface_mux_nr
{
public:
  mac_nr(srsran::ext_task_sched_handle task_sched_);
  ~mac_nr();

  int  init(const mac_nr_args_t& args_, phy_interface_mac_nr* phy, rlc_interface_mac* rlc, rrc_interface_mac* rrc_);
  void stop();

  void reset();
  void run_tti(const uint32_t tti);

  void start_pcap(srsran::mac_pcap* pcap_);

  void bch_decoded_ok(uint32_t tti, srsran::unique_byte_buffer_t payload);

  /// Interface for PHY
  sched_rnti_t get_dl_sched_rnti_nr(const uint32_t tti);
  sched_rnti_t get_ul_sched_rnti_nr(const uint32_t tti);

  int  sf_indication(const uint32_t tti);
  void tb_decoded(const uint32_t cc_idx, mac_nr_grant_dl_t& grant);
  void new_grant_ul(const uint32_t cc_idx, const mac_nr_grant_ul_t& grant, tb_action_ul_t* action);
  void prach_sent(const uint32_t tti,
                  const uint32_t s_id,
                  const uint32_t t_id,
                  const uint32_t f_id,
                  const uint32_t ul_carrier_id);

  /// Stack interface
  void timer_expired(uint32_t timer_id);
  void get_metrics(mac_metrics_t* metrics);

  /// Interface for RRC (RRC -> MAC)
  int  setup_lcid(const srsran::logical_channel_config_t& config);
  int  set_config(const srsran::bsr_cfg_nr_t& bsr_cfg);
  int  set_config(const srsran::sr_cfg_nr_t& sr_cfg);
  void set_config(const srsran::rach_nr_cfg_t& rach_cfg);
  void set_contention_id(const uint64_t ue_identity);
  bool set_crnti(const uint16_t crnti);
  void start_ra_procedure();

  /// procedure ra nr interface + mux
  uint64_t get_contention_id();
  uint16_t get_crnti();

  void msg3_flush() { mux.msg3_flush(); }
  bool msg3_is_transmitted() { return mux.msg3_is_transmitted(); }
  void msg3_prepare() { mux.msg3_prepare(); }
  bool msg3_is_pending() { return mux.msg3_is_pending(); }
  bool msg3_is_empty() { return mux.msg3_is_empty(); }

  /// stack interface
  void process_pdus();

  static bool is_in_window(uint32_t tti, int* start, int* len);

  // PHY Interface
  void prach_sent(const uint32_t tti);
  void tb_decoded_ok(const uint8_t cc_idx, const uint32_t tti);

private:
  void write_pcap(const uint32_t cc_idx, mac_nr_grant_dl_t& grant); // If PCAPs are enabled for this MAC
  void handle_pdu(srsran::unique_byte_buffer_t pdu);
  void get_ul_data(const mac_nr_grant_ul_t& grant, srsran::byte_buffer_t* tx_pdu);

  // temporary helper
  void handle_rar_pdu(mac_nr_grant_dl_t& grant);

  bool is_si_opportunity();
  bool is_paging_opportunity();

  bool     has_crnti();
  bool     is_valid_crnti(const uint16_t crnti);

  /// Interaction with rest of the stack
  phy_interface_mac_nr*         phy = nullptr;
  rlc_interface_mac*            rlc = nullptr;
  rrc_interface_mac*            rrc = nullptr;
  srsran::ext_task_sched_handle task_sched;

  srsran::mac_pcap*     pcap = nullptr;
  srslog::basic_logger& logger;
  mac_nr_args_t         args = {};

  bool started = false;

  uint16_t c_rnti        = SRSRAN_INVALID_RNTI;
  uint64_t contention_id = 0;

  srsran::block_queue<srsran::unique_byte_buffer_t>
      pdu_queue; ///< currently only DCH PDUs supported (add BCH, PCH, etc)

  mac_metrics_t metrics[SRSRAN_MAX_CARRIERS] = {};

  /// Rx buffer
  srsran::mac_sch_pdu_nr rx_pdu;

  /// Tx buffer
  srsran::unique_byte_buffer_t ul_harq_buffer = nullptr; // store PDU generated from MUX
  srsran::unique_byte_buffer_t rlc_buffer    = nullptr;
  srsran_softbuffer_tx_t       softbuffer_tx = {}; /// UL HARQ (temporal)

  srsran::task_multiqueue::queue_handle stack_task_dispatch_queue;

  // MAC Uplink-related procedures
  proc_ra_nr  proc_ra;
  proc_sr_nr  proc_sr;
  proc_bsr_nr proc_bsr;
  mux_nr      mux;
};

} // namespace srsue

#endif // SRSUE_MAC_NR_H