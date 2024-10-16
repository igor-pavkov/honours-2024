static void naldmx_check_timestamp_switch(GF_NALUDmxCtx *ctx, u32 *nalu_store_before, u32 bytes_drop, Bool *drop_packet, GF_FilterPacket *pck)
{
	if (*nalu_store_before) {
		if (*nalu_store_before > bytes_drop) {
			*nalu_store_before -= bytes_drop;
		} else {
			//all data from previous frame consumed, update timestamps with info from current packet
			*nalu_store_before = 0;
			naldmx_switch_timestamps(ctx, pck);
			if (*drop_packet) {
				gf_filter_pid_drop_packet(ctx->ipid);
				*drop_packet = GF_FALSE;
			}
		}
	}
}