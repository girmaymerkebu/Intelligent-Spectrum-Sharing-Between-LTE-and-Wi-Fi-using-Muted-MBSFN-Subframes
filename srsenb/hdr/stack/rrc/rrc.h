/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSENB_RRC_H
#define SRSENB_RRC_H

#include "rrc_bearer_cfg.h"
#include "rrc_cell_cfg.h"
#include "rrc_metrics.h"
#include "srsenb/hdr/common/common_enb.h"
#include "srsenb/hdr/common/rnti_pool.h"
#include "srsran/adt/circular_buffer.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/stack_procedure.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/common/timeout.h"
#include "srsran/interfaces/enb_rrc_interfaces.h"
#include "srsran/srslog/srslog.h"
#include <map>

namespace srsenb {

class s1ap_interface_rrc;
class pdcp_interface_rrc;
class rlc_interface_rrc;
class mac_interface_rrc;
class phy_interface_rrc_lte;

static const char rrc_state_text[RRC_STATE_N_ITEMS][100] = {"IDLE",
                                                            "WAIT FOR CON SETUP COMPLETE",
                                                            "WAIT FOR SECURITY MODE COMPLETE",
                                                            "WAIT FOR UE CAPABILITIY INFORMATION",
                                                            "WAIT FOR CON RECONF COMPLETE",
                                                            "RRC CONNECTED",
                                                            "RELEASE REQUEST"};

class rrc final : public rrc_interface_pdcp,
                  public rrc_interface_mac,
                  public rrc_interface_rlc,
                  public rrc_interface_s1ap
{
public:
  explicit rrc(srsran::task_sched_handle task_sched_);
  ~rrc();

  int32_t init(const rrc_cfg_t&       cfg_,
               phy_interface_rrc_lte* phy,
               mac_interface_rrc*     mac,
               rlc_interface_rrc*     rlc,
               pdcp_interface_rrc*    pdcp,
               s1ap_interface_rrc*    s1ap,
               gtpu_interface_rrc*    gtpu);

  void stop();
  void get_metrics(rrc_metrics_t& m);
  void tti_clock();

  // rrc_interface_mac
  int      add_user(uint16_t rnti, const sched_interface::ue_cfg_t& init_ue_cfg) override;
  void     upd_user(uint16_t new_rnti, uint16_t old_rnti) override;
  void     set_activity_user(uint16_t rnti) override;
  void     set_radiolink_dl_state(uint16_t rnti, bool crc_res) override;
  void     set_radiolink_ul_state(uint16_t rnti, bool crc_res) override;
  bool     is_paging_opportunity(uint32_t tti, uint32_t* payload_len) override;
  uint8_t* read_pdu_bcch_dlsch(const uint8_t cc_idx, const uint32_t sib_index) override;

  // rrc_interface_rlc
  void read_pdu_pcch(uint8_t* payload, uint32_t buffer_size) override;
  void max_retx_attempted(uint16_t rnti) override;

  // rrc_interface_s1ap
  void     write_dl_info(uint16_t rnti, srsran::unique_byte_buffer_t sdu) override;
  void     release_ue(uint16_t rnti) override;
  bool     setup_ue_ctxt(uint16_t rnti, const asn1::s1ap::init_context_setup_request_s& msg) override;
  bool     modify_ue_ctxt(uint16_t rnti, const asn1::s1ap::ue_context_mod_request_s& msg) override;
  bool     has_erab(uint16_t rnti, uint32_t erab_id) const override;
  int      get_erab_addr_in(uint16_t rnti, uint16_t erab_id, transp_addr_t& addr_in, uint32_t& teid_in) const override;
  void     set_aggregate_max_bitrate(uint16_t rnti, const asn1::s1ap::ue_aggregate_maximum_bitrate_s& bitrate) override;
  int      setup_erab(uint16_t                                           rnti,
                      uint16_t                                           erab_id,
                      const asn1::s1ap::erab_level_qos_params_s&         qos_params,
                      srsran::const_span<uint8_t>                        nas_pdu,
                      const asn1::bounded_bitstring<1, 160, true, true>& addr,
                      uint32_t                                           gtpu_teid_out,
                      asn1::s1ap::cause_c&                               cause) override;
  int      modify_erab(uint16_t                                   rnti,
                       uint16_t                                   erab_id,
                       const asn1::s1ap::erab_level_qos_params_s& qos_params,
                       srsran::const_span<uint8_t>                nas_pdu,
                       asn1::s1ap::cause_c&                       cause) override;
  bool     release_erabs(uint32_t rnti) override;
  int      release_erab(uint16_t rnti, uint16_t erab_id) override;
  void     add_paging_id(uint32_t ueid, const asn1::s1ap::ue_paging_id_c& UEPagingID) override;
  void     ho_preparation_complete(uint16_t                     rnti,
                                   rrc::ho_prep_result          result,
                                   const asn1::s1ap::ho_cmd_s&  msg,
                                   srsran::unique_byte_buffer_t rrc_container) override;
  uint16_t start_ho_ue_resource_alloc(const asn1::s1ap::ho_request_s&                                   msg,
                                      const asn1::s1ap::sourceenb_to_targetenb_transparent_container_s& container,
                                      asn1::s1ap::cause_c& failure_cause) override;
  void     set_erab_status(uint16_t rnti, const asn1::s1ap::bearers_subject_to_status_transfer_list_l& erabs) override;

  int notify_ue_erab_updates(uint16_t rnti, srsran::const_byte_span nas_pdu) override;

  // rrc_interface_pdcp
  void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu) override;

  uint32_t get_nof_users();

  // logging
  typedef enum { Rx = 0, Tx, toS1AP, fromS1AP } direction_t;
  template <class T>
  void log_rrc_message(const std::string&           source,
                       const direction_t            dir,
                       const srsran::byte_buffer_t* pdu,
                       const T&                     msg,
                       const std::string&           msg_type)
  {
    log_rrc_message(source, dir, srsran::make_span(*pdu), msg, msg_type);
  }
  template <class T>
  void log_rrc_message(const std::string&      source,
                       const direction_t       dir,
                       srsran::const_byte_span pdu,
                       const T&                msg,
                       const std::string&      msg_type)
  {
    static const char* dir_str[] = {"Rx", "Tx", "S1AP Tx", "S1AP Rx"};
    if (logger.debug.enabled()) {
      asn1::json_writer json_writer;
      msg.to_json(json_writer);
      logger.debug(
          pdu.data(), pdu.size(), "%s - %s %s (%zd B)", source.c_str(), dir_str[dir], msg_type.c_str(), pdu.size());
      logger.debug("Content:\n%s", json_writer.to_string().c_str());
    } else if (logger.info.enabled()) {
      logger.info("%s - %s %s (%zd B)", source.c_str(), dir_str[dir], msg_type.c_str(), pdu.size());
    }
  }

private:
  class ue;
  // args
  srsran::task_sched_handle task_sched;
  phy_interface_rrc_lte*    phy  = nullptr;
  mac_interface_rrc*        mac  = nullptr;
  rlc_interface_rrc*        rlc  = nullptr;
  pdcp_interface_rrc*       pdcp = nullptr;
  gtpu_interface_rrc*       gtpu = nullptr;
  s1ap_interface_rrc*       s1ap = nullptr;
  srslog::basic_logger&     logger;

  // derived params
  std::unique_ptr<enb_cell_common_list> cell_common_list;

  // state
  std::unique_ptr<freq_res_common_list>          cell_res_list;
  std::map<uint16_t, unique_rnti_ptr<ue> >       users; // NOTE: has to have fixed addr
  std::map<uint32_t, asn1::rrc::paging_record_s> pending_paging;

  void     process_release_complete(uint16_t rnti);
  void     rem_user(uint16_t rnti);
  uint32_t generate_sibs(int sfalloc, int sfperiod);
  uint32_t generate_sibsn();
  void     configure_mbsfn_sibs(int sfalloc, int sfperiod);
  void     configure_mbsfn_sibsn();
  int      pack_mcch();

  void config_mac();
  void parse_ul_dcch(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu);
  void parse_ul_ccch(uint16_t rnti, srsran::unique_byte_buffer_t pdu);
  void send_rrc_connection_reject(uint16_t rnti);

  uint32_t              paging_tti = INVALID_TTI;
  srsran::byte_buffer_t byte_buf_paging;
  const static int      mcch_payload_len                      = 3000;
  int                   current_mcch_length                   = 0;
  uint8_t               mcch_payload_buffer[mcch_payload_len] = {};
  typedef struct {
    uint16_t                     rnti;
    uint32_t                     lcid;
    uint32_t                     arg;
    srsran::unique_byte_buffer_t pdu;
  } rrc_pdu;

  const static uint32_t LCID_EXIT        = 0xffff0000;
  const static uint32_t LCID_REM_USER    = 0xffff0001;
  const static uint32_t LCID_REL_USER    = 0xffff0002;
  const static uint32_t LCID_ACT_USER    = 0xffff0004;
  const static uint32_t LCID_RTX_USER    = 0xffff0005;
  const static uint32_t LCID_RADLINK_DL  = 0xffff0006;
  const static uint32_t LCID_RADLINK_UL  = 0xffff0007;

  bool                                running = false;
  srsran::dyn_blocking_queue<rrc_pdu> rx_pdu_queue;

  asn1::rrc::mcch_msg_s  mcch;
  bool                   enable_mbms     = false;
  rrc_cfg_t              cfg             = {};
  uint32_t               nof_si_messages = 0;
  asn1::rrc::sib_type7_s sib7;

  void rem_user_thread(uint16_t rnti);

  std::mutex paging_mutex;
};

} // namespace srsenb

#endif // SRSENB_RRC_H
