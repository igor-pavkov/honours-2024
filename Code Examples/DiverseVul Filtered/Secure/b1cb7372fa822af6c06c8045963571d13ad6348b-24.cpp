static int dtv_property_process_get(struct dvb_frontend *fe,
				    const struct dtv_frontend_properties *c,
				    struct dtv_property *tvp,
				    struct file *file)
{
	int ncaps;

	switch(tvp->cmd) {
	case DTV_ENUM_DELSYS:
		ncaps = 0;
		while (ncaps < MAX_DELSYS && fe->ops.delsys[ncaps]) {
			tvp->u.buffer.data[ncaps] = fe->ops.delsys[ncaps];
			ncaps++;
		}
		tvp->u.buffer.len = ncaps;
		break;
	case DTV_FREQUENCY:
		tvp->u.data = c->frequency;
		break;
	case DTV_MODULATION:
		tvp->u.data = c->modulation;
		break;
	case DTV_BANDWIDTH_HZ:
		tvp->u.data = c->bandwidth_hz;
		break;
	case DTV_INVERSION:
		tvp->u.data = c->inversion;
		break;
	case DTV_SYMBOL_RATE:
		tvp->u.data = c->symbol_rate;
		break;
	case DTV_INNER_FEC:
		tvp->u.data = c->fec_inner;
		break;
	case DTV_PILOT:
		tvp->u.data = c->pilot;
		break;
	case DTV_ROLLOFF:
		tvp->u.data = c->rolloff;
		break;
	case DTV_DELIVERY_SYSTEM:
		tvp->u.data = c->delivery_system;
		break;
	case DTV_VOLTAGE:
		tvp->u.data = c->voltage;
		break;
	case DTV_TONE:
		tvp->u.data = c->sectone;
		break;
	case DTV_API_VERSION:
		tvp->u.data = (DVB_API_VERSION << 8) | DVB_API_VERSION_MINOR;
		break;
	case DTV_CODE_RATE_HP:
		tvp->u.data = c->code_rate_HP;
		break;
	case DTV_CODE_RATE_LP:
		tvp->u.data = c->code_rate_LP;
		break;
	case DTV_GUARD_INTERVAL:
		tvp->u.data = c->guard_interval;
		break;
	case DTV_TRANSMISSION_MODE:
		tvp->u.data = c->transmission_mode;
		break;
	case DTV_HIERARCHY:
		tvp->u.data = c->hierarchy;
		break;
	case DTV_INTERLEAVING:
		tvp->u.data = c->interleaving;
		break;

	/* ISDB-T Support here */
	case DTV_ISDBT_PARTIAL_RECEPTION:
		tvp->u.data = c->isdbt_partial_reception;
		break;
	case DTV_ISDBT_SOUND_BROADCASTING:
		tvp->u.data = c->isdbt_sb_mode;
		break;
	case DTV_ISDBT_SB_SUBCHANNEL_ID:
		tvp->u.data = c->isdbt_sb_subchannel;
		break;
	case DTV_ISDBT_SB_SEGMENT_IDX:
		tvp->u.data = c->isdbt_sb_segment_idx;
		break;
	case DTV_ISDBT_SB_SEGMENT_COUNT:
		tvp->u.data = c->isdbt_sb_segment_count;
		break;
	case DTV_ISDBT_LAYER_ENABLED:
		tvp->u.data = c->isdbt_layer_enabled;
		break;
	case DTV_ISDBT_LAYERA_FEC:
		tvp->u.data = c->layer[0].fec;
		break;
	case DTV_ISDBT_LAYERA_MODULATION:
		tvp->u.data = c->layer[0].modulation;
		break;
	case DTV_ISDBT_LAYERA_SEGMENT_COUNT:
		tvp->u.data = c->layer[0].segment_count;
		break;
	case DTV_ISDBT_LAYERA_TIME_INTERLEAVING:
		tvp->u.data = c->layer[0].interleaving;
		break;
	case DTV_ISDBT_LAYERB_FEC:
		tvp->u.data = c->layer[1].fec;
		break;
	case DTV_ISDBT_LAYERB_MODULATION:
		tvp->u.data = c->layer[1].modulation;
		break;
	case DTV_ISDBT_LAYERB_SEGMENT_COUNT:
		tvp->u.data = c->layer[1].segment_count;
		break;
	case DTV_ISDBT_LAYERB_TIME_INTERLEAVING:
		tvp->u.data = c->layer[1].interleaving;
		break;
	case DTV_ISDBT_LAYERC_FEC:
		tvp->u.data = c->layer[2].fec;
		break;
	case DTV_ISDBT_LAYERC_MODULATION:
		tvp->u.data = c->layer[2].modulation;
		break;
	case DTV_ISDBT_LAYERC_SEGMENT_COUNT:
		tvp->u.data = c->layer[2].segment_count;
		break;
	case DTV_ISDBT_LAYERC_TIME_INTERLEAVING:
		tvp->u.data = c->layer[2].interleaving;
		break;

	/* Multistream support */
	case DTV_STREAM_ID:
	case DTV_DVBT2_PLP_ID_LEGACY:
		tvp->u.data = c->stream_id;
		break;

	/* ATSC-MH */
	case DTV_ATSCMH_FIC_VER:
		tvp->u.data = fe->dtv_property_cache.atscmh_fic_ver;
		break;
	case DTV_ATSCMH_PARADE_ID:
		tvp->u.data = fe->dtv_property_cache.atscmh_parade_id;
		break;
	case DTV_ATSCMH_NOG:
		tvp->u.data = fe->dtv_property_cache.atscmh_nog;
		break;
	case DTV_ATSCMH_TNOG:
		tvp->u.data = fe->dtv_property_cache.atscmh_tnog;
		break;
	case DTV_ATSCMH_SGN:
		tvp->u.data = fe->dtv_property_cache.atscmh_sgn;
		break;
	case DTV_ATSCMH_PRC:
		tvp->u.data = fe->dtv_property_cache.atscmh_prc;
		break;
	case DTV_ATSCMH_RS_FRAME_MODE:
		tvp->u.data = fe->dtv_property_cache.atscmh_rs_frame_mode;
		break;
	case DTV_ATSCMH_RS_FRAME_ENSEMBLE:
		tvp->u.data = fe->dtv_property_cache.atscmh_rs_frame_ensemble;
		break;
	case DTV_ATSCMH_RS_CODE_MODE_PRI:
		tvp->u.data = fe->dtv_property_cache.atscmh_rs_code_mode_pri;
		break;
	case DTV_ATSCMH_RS_CODE_MODE_SEC:
		tvp->u.data = fe->dtv_property_cache.atscmh_rs_code_mode_sec;
		break;
	case DTV_ATSCMH_SCCC_BLOCK_MODE:
		tvp->u.data = fe->dtv_property_cache.atscmh_sccc_block_mode;
		break;
	case DTV_ATSCMH_SCCC_CODE_MODE_A:
		tvp->u.data = fe->dtv_property_cache.atscmh_sccc_code_mode_a;
		break;
	case DTV_ATSCMH_SCCC_CODE_MODE_B:
		tvp->u.data = fe->dtv_property_cache.atscmh_sccc_code_mode_b;
		break;
	case DTV_ATSCMH_SCCC_CODE_MODE_C:
		tvp->u.data = fe->dtv_property_cache.atscmh_sccc_code_mode_c;
		break;
	case DTV_ATSCMH_SCCC_CODE_MODE_D:
		tvp->u.data = fe->dtv_property_cache.atscmh_sccc_code_mode_d;
		break;

	case DTV_LNA:
		tvp->u.data = c->lna;
		break;

	/* Fill quality measures */
	case DTV_STAT_SIGNAL_STRENGTH:
		tvp->u.st = c->strength;
		break;
	case DTV_STAT_CNR:
		tvp->u.st = c->cnr;
		break;
	case DTV_STAT_PRE_ERROR_BIT_COUNT:
		tvp->u.st = c->pre_bit_error;
		break;
	case DTV_STAT_PRE_TOTAL_BIT_COUNT:
		tvp->u.st = c->pre_bit_count;
		break;
	case DTV_STAT_POST_ERROR_BIT_COUNT:
		tvp->u.st = c->post_bit_error;
		break;
	case DTV_STAT_POST_TOTAL_BIT_COUNT:
		tvp->u.st = c->post_bit_count;
		break;
	case DTV_STAT_ERROR_BLOCK_COUNT:
		tvp->u.st = c->block_error;
		break;
	case DTV_STAT_TOTAL_BLOCK_COUNT:
		tvp->u.st = c->block_count;
		break;
	default:
		dev_dbg(fe->dvb->device,
			"%s: FE property %d doesn't exist\n",
			__func__, tvp->cmd);
		return -EINVAL;
	}

	if (!dtv_cmds[tvp->cmd].buffer)
		dev_dbg(fe->dvb->device,
			"%s: GET cmd 0x%08x (%s) = 0x%08x\n",
			__func__, tvp->cmd, dtv_cmds[tvp->cmd].name,
			tvp->u.data);
	else
		dev_dbg(fe->dvb->device,
			"%s: GET cmd 0x%08x (%s) len %d: %*ph\n",
			__func__,
			tvp->cmd, dtv_cmds[tvp->cmd].name,
			tvp->u.buffer.len,
			tvp->u.buffer.len, tvp->u.buffer.data);

	return 0;
}