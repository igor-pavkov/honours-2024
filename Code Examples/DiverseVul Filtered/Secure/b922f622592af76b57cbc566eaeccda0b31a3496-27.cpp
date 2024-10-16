static int aq_fw1x_set_wake_magic(struct aq_hw_s *self, bool wol_enabled,
				  const u8 *mac)
{
	struct hw_atl_utils_fw_rpc *prpc = NULL;
	unsigned int rpc_size = 0U;
	int err = 0;

	err = hw_atl_utils_fw_rpc_wait(self, &prpc);
	if (err < 0)
		goto err_exit;

	memset(prpc, 0, sizeof(*prpc));

	if (wol_enabled) {
		rpc_size = offsetof(struct hw_atl_utils_fw_rpc, msg_wol_add) +
			   sizeof(prpc->msg_wol_add);


		prpc->msg_id = HAL_ATLANTIC_UTILS_FW_MSG_WOL_ADD;
		prpc->msg_wol_add.priority =
				HAL_ATLANTIC_UTILS_FW_MSG_WOL_PRIOR;
		prpc->msg_wol_add.pattern_id =
				HAL_ATLANTIC_UTILS_FW_MSG_WOL_PATTERN;
		prpc->msg_wol_add.packet_type =
				HAL_ATLANTIC_UTILS_FW_MSG_WOL_MAG_PKT;

		ether_addr_copy((u8 *)&prpc->msg_wol_add.magic_packet_pattern,
				mac);
	} else {
		rpc_size = sizeof(prpc->msg_wol_remove) +
			   offsetof(struct hw_atl_utils_fw_rpc, msg_wol_remove);

		prpc->msg_id = HAL_ATLANTIC_UTILS_FW_MSG_WOL_DEL;
		prpc->msg_wol_add.pattern_id =
				HAL_ATLANTIC_UTILS_FW_MSG_WOL_PATTERN;
	}

	err = hw_atl_utils_fw_rpc_call(self, rpc_size);

err_exit:
	return err;
}