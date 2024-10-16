static int hw_atl_utils_init_ucp(struct aq_hw_s *self,
				 const struct aq_hw_caps_s *aq_hw_caps)
{
	int err = 0;

	if (!aq_hw_read_reg(self, 0x370U)) {
		unsigned int rnd = 0U;
		unsigned int ucp_0x370 = 0U;

		get_random_bytes(&rnd, sizeof(unsigned int));

		ucp_0x370 = 0x02020202U | (0xFEFEFEFEU & rnd);
		aq_hw_write_reg(self, HW_ATL_UCP_0X370_REG, ucp_0x370);
	}

	hw_atl_reg_glb_cpu_scratch_scp_set(self, 0x00000000U, 25U);

	/* check 10 times by 1ms */
	err = readx_poll_timeout_atomic(hw_atl_scrpad25_get,
					self, self->mbox_addr,
					self->mbox_addr != 0U,
					1000U, 10000U);
	err = readx_poll_timeout_atomic(aq_fw1x_rpc_get, self,
					self->rpc_addr,
					self->rpc_addr != 0U,
					1000U, 100000U);

	return err;
}