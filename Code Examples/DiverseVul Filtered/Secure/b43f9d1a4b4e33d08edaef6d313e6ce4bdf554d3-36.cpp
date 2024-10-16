static void naludmx_hevc_set_parall_type(GF_NALUDmxCtx *ctx, GF_HEVCConfig *hevc_cfg)
{
	u32 use_tiles, use_wpp, nb_pps, i, count;
	HEVCState hevc;

	count = gf_list_count(ctx->pps);

	memset(&hevc, 0, sizeof(HEVCState));
	hevc.sps_active_idx = -1;

	use_tiles = 0;
	use_wpp = 0;
	nb_pps = 0;

	for (i=0; i<count; i++) {
		GF_NALUFFParam *slc = (GF_NALUFFParam*)gf_list_get(ctx->pps, i);
		s32 idx = gf_hevc_read_pps(slc->data, slc->size, &hevc);

		if (idx>=0) {
			HEVC_PPS *pps;
			nb_pps++;
			pps = &hevc.pps[idx];
			if (!pps->entropy_coding_sync_enabled_flag && pps->tiles_enabled_flag)
				use_tiles++;
			else if (pps->entropy_coding_sync_enabled_flag && !pps->tiles_enabled_flag)
				use_wpp++;
		}
	}
	if (!use_tiles && !use_wpp) hevc_cfg->parallelismType = 1;
	else if (!use_wpp && (use_tiles==nb_pps) ) hevc_cfg->parallelismType = 2;
	else if (!use_tiles && (use_wpp==nb_pps) ) hevc_cfg->parallelismType = 3;
	else hevc_cfg->parallelismType = 0;
}