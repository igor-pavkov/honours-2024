static int hw_atl_utils_soft_reset_flb(struct aq_hw_s *self)
{
	u32 gsr, val;
	int k = 0;

	aq_hw_write_reg(self, 0x404, 0x40e1);
	AQ_HW_SLEEP(50);

	/* Cleanup SPI */
	val = aq_hw_read_reg(self, 0x53C);
	aq_hw_write_reg(self, 0x53C, val | 0x10);

	gsr = aq_hw_read_reg(self, HW_ATL_GLB_SOFT_RES_ADR);
	aq_hw_write_reg(self, HW_ATL_GLB_SOFT_RES_ADR, (gsr & 0xBFFF) | 0x8000);

	/* Kickstart MAC */
	aq_hw_write_reg(self, 0x404, 0x80e0);
	aq_hw_write_reg(self, 0x32a8, 0x0);
	aq_hw_write_reg(self, 0x520, 0x1);

	/* Reset SPI again because of possible interrupted SPI burst */
	val = aq_hw_read_reg(self, 0x53C);
	aq_hw_write_reg(self, 0x53C, val | 0x10);
	AQ_HW_SLEEP(10);
	/* Clear SPI reset state */
	aq_hw_write_reg(self, 0x53C, val & ~0x10);

	aq_hw_write_reg(self, 0x404, 0x180e0);

	for (k = 0; k < 1000; k++) {
		u32 flb_status = aq_hw_read_reg(self,
						HW_ATL_MPI_DAISY_CHAIN_STATUS);

		flb_status = flb_status & 0x10;
		if (flb_status)
			break;
		AQ_HW_SLEEP(10);
	}
	if (k == 1000) {
		aq_pr_err("MAC kickstart failed\n");
		return -EIO;
	}

	/* FW reset */
	aq_hw_write_reg(self, 0x404, 0x80e0);
	AQ_HW_SLEEP(50);
	aq_hw_write_reg(self, 0x3a0, 0x1);

	/* Kickstart PHY - skipped */

	/* Global software reset*/
	hw_atl_rx_rx_reg_res_dis_set(self, 0U);
	hw_atl_tx_tx_reg_res_dis_set(self, 0U);
	aq_hw_write_reg_bit(self, HW_ATL_MAC_PHY_CONTROL,
			    BIT(HW_ATL_MAC_PHY_MPI_RESET_BIT),
			    HW_ATL_MAC_PHY_MPI_RESET_BIT, 0x0);
	gsr = aq_hw_read_reg(self, HW_ATL_GLB_SOFT_RES_ADR);
	aq_hw_write_reg(self, HW_ATL_GLB_SOFT_RES_ADR, (gsr & 0xBFFF) | 0x8000);

	for (k = 0; k < 1000; k++) {
		u32 fw_state = aq_hw_read_reg(self, HW_ATL_MPI_FW_VERSION);

		if (fw_state)
			break;
		AQ_HW_SLEEP(10);
	}
	if (k == 1000) {
		aq_pr_err("FW kickstart failed\n");
		return -EIO;
	}
	/* Old FW requires fixed delay after init */
	AQ_HW_SLEEP(15);

	return 0;
}