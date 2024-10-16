void hw_atl_utils_mpi_read_stats(struct aq_hw_s *self,
				 struct hw_atl_utils_mbox *pmbox)
{
	int err = 0;

	err = hw_atl_utils_fw_downld_dwords(self,
					    self->mbox_addr,
					    (u32 *)(void *)pmbox,
					    sizeof(*pmbox) / sizeof(u32));
	if (err < 0)
		goto err_exit;

	if (ATL_HW_IS_CHIP_FEATURE(self, REVISION_A0)) {
		unsigned int mtu = self->aq_nic_cfg ?
					self->aq_nic_cfg->mtu : 1514U;
		pmbox->stats.ubrc = pmbox->stats.uprc * mtu;
		pmbox->stats.ubtc = pmbox->stats.uptc * mtu;
		pmbox->stats.dpc = atomic_read(&self->dpc);
	} else {
		pmbox->stats.dpc = hw_atl_rpb_rx_dma_drop_pkt_cnt_get(self);
	}

err_exit:;
}