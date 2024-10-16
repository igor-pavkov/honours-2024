static int dtv_property_process_set(struct dvb_frontend *fe,
					struct file *file,
					u32 cmd, u32 data)
{
	int r = 0;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;

	/** Dump DTV command name and value*/
	if (!cmd || cmd > DTV_MAX_COMMAND)
		dev_warn(fe->dvb->device, "%s: SET cmd 0x%08x undefined\n",
				 __func__, cmd);
	else
		dev_dbg(fe->dvb->device,
				"%s: SET cmd 0x%08x (%s) to 0x%08x\n",
				__func__, cmd, dtv_cmds[cmd].name, data);
	switch (cmd) {
	case DTV_CLEAR:
		/*
		 * Reset a cache of data specific to the frontend here. This does
		 * not effect hardware.
		 */
		dvb_frontend_clear_cache(fe);
		break;
	case DTV_TUNE:
		/*
		 * Use the cached Digital TV properties to tune the
		 * frontend
		 */
		dev_dbg(fe->dvb->device,
			"%s: Setting the frontend from property cache\n",
			__func__);

		r = dtv_set_frontend(fe);
		break;
	case DTV_FREQUENCY:
		c->frequency = data;
		break;
	case DTV_MODULATION:
		c->modulation = data;
		break;
	case DTV_BANDWIDTH_HZ:
		c->bandwidth_hz = data;
		break;
	case DTV_INVERSION:
		c->inversion = data;
		break;
	case DTV_SYMBOL_RATE:
		c->symbol_rate = data;
		break;
	case DTV_INNER_FEC:
		c->fec_inner = data;
		break;
	case DTV_PILOT:
		c->pilot = data;
		break;
	case DTV_ROLLOFF:
		c->rolloff = data;
		break;
	case DTV_DELIVERY_SYSTEM:
		r = dvbv5_set_delivery_system(fe, data);
		break;
	case DTV_VOLTAGE:
		c->voltage = data;
		r = dvb_frontend_handle_ioctl(file, FE_SET_VOLTAGE,
			(void *)c->voltage);
		break;
	case DTV_TONE:
		c->sectone = data;
		r = dvb_frontend_handle_ioctl(file, FE_SET_TONE,
			(void *)c->sectone);
		break;
	case DTV_CODE_RATE_HP:
		c->code_rate_HP = data;
		break;
	case DTV_CODE_RATE_LP:
		c->code_rate_LP = data;
		break;
	case DTV_GUARD_INTERVAL:
		c->guard_interval = data;
		break;
	case DTV_TRANSMISSION_MODE:
		c->transmission_mode = data;
		break;
	case DTV_HIERARCHY:
		c->hierarchy = data;
		break;
	case DTV_INTERLEAVING:
		c->interleaving = data;
		break;

	/* ISDB-T Support here */
	case DTV_ISDBT_PARTIAL_RECEPTION:
		c->isdbt_partial_reception = data;
		break;
	case DTV_ISDBT_SOUND_BROADCASTING:
		c->isdbt_sb_mode = data;
		break;
	case DTV_ISDBT_SB_SUBCHANNEL_ID:
		c->isdbt_sb_subchannel = data;
		break;
	case DTV_ISDBT_SB_SEGMENT_IDX:
		c->isdbt_sb_segment_idx = data;
		break;
	case DTV_ISDBT_SB_SEGMENT_COUNT:
		c->isdbt_sb_segment_count = data;
		break;
	case DTV_ISDBT_LAYER_ENABLED:
		c->isdbt_layer_enabled = data;
		break;
	case DTV_ISDBT_LAYERA_FEC:
		c->layer[0].fec = data;
		break;
	case DTV_ISDBT_LAYERA_MODULATION:
		c->layer[0].modulation = data;
		break;
	case DTV_ISDBT_LAYERA_SEGMENT_COUNT:
		c->layer[0].segment_count = data;
		break;
	case DTV_ISDBT_LAYERA_TIME_INTERLEAVING:
		c->layer[0].interleaving = data;
		break;
	case DTV_ISDBT_LAYERB_FEC:
		c->layer[1].fec = data;
		break;
	case DTV_ISDBT_LAYERB_MODULATION:
		c->layer[1].modulation = data;
		break;
	case DTV_ISDBT_LAYERB_SEGMENT_COUNT:
		c->layer[1].segment_count = data;
		break;
	case DTV_ISDBT_LAYERB_TIME_INTERLEAVING:
		c->layer[1].interleaving = data;
		break;
	case DTV_ISDBT_LAYERC_FEC:
		c->layer[2].fec = data;
		break;
	case DTV_ISDBT_LAYERC_MODULATION:
		c->layer[2].modulation = data;
		break;
	case DTV_ISDBT_LAYERC_SEGMENT_COUNT:
		c->layer[2].segment_count = data;
		break;
	case DTV_ISDBT_LAYERC_TIME_INTERLEAVING:
		c->layer[2].interleaving = data;
		break;

	/* Multistream support */
	case DTV_STREAM_ID:
	case DTV_DVBT2_PLP_ID_LEGACY:
		c->stream_id = data;
		break;

	/* ATSC-MH */
	case DTV_ATSCMH_PARADE_ID:
		fe->dtv_property_cache.atscmh_parade_id = data;
		break;
	case DTV_ATSCMH_RS_FRAME_ENSEMBLE:
		fe->dtv_property_cache.atscmh_rs_frame_ensemble = data;
		break;

	case DTV_LNA:
		c->lna = data;
		if (fe->ops.set_lna)
			r = fe->ops.set_lna(fe);
		if (r < 0)
			c->lna = LNA_AUTO;
		break;

	default:
		return -EINVAL;
	}

	return r;
}