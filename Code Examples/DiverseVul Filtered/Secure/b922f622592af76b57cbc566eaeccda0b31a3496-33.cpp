int hw_atl_utils_update_stats(struct aq_hw_s *self)
{
	struct aq_stats_s *cs = &self->curr_stats;
	struct hw_atl_utils_mbox mbox;

	hw_atl_utils_mpi_read_stats(self, &mbox);

#define AQ_SDELTA(_N_) (self->curr_stats._N_ += \
			mbox.stats._N_ - self->last_stats._N_)

	if (self->aq_link_status.mbps) {
		AQ_SDELTA(uprc);
		AQ_SDELTA(mprc);
		AQ_SDELTA(bprc);
		AQ_SDELTA(erpt);

		AQ_SDELTA(uptc);
		AQ_SDELTA(mptc);
		AQ_SDELTA(bptc);
		AQ_SDELTA(erpr);

		AQ_SDELTA(ubrc);
		AQ_SDELTA(ubtc);
		AQ_SDELTA(mbrc);
		AQ_SDELTA(mbtc);
		AQ_SDELTA(bbrc);
		AQ_SDELTA(bbtc);
		AQ_SDELTA(dpc);
	}
#undef AQ_SDELTA

	cs->dma_pkt_rc = hw_atl_stats_rx_dma_good_pkt_counter_get(self);
	cs->dma_pkt_tc = hw_atl_stats_tx_dma_good_pkt_counter_get(self);
	cs->dma_oct_rc = hw_atl_stats_rx_dma_good_octet_counter_get(self);
	cs->dma_oct_tc = hw_atl_stats_tx_dma_good_octet_counter_get(self);

	memcpy(&self->last_stats, &mbox.stats, sizeof(mbox.stats));

	return 0;
}