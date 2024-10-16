static Bool nhmldmx_process_event(GF_Filter *filter, const GF_FilterEvent *evt)
{
	u32 i=0;
	u64 cur_dts = 0;
	u64 byte_offset = 0;
	u32 sample_num = 0;
	GF_XMLNode *node;
	GF_NHMLDmxCtx *ctx = gf_filter_get_udta(filter);

	switch (evt->base.type) {
	case GF_FEVT_PLAY:
		if (ctx->is_playing && (ctx->start_range ==  evt->play.start_range)) {
			return GF_TRUE;
		}

		ctx->start_range = evt->play.start_range;
		ctx->current_child_idx = 0;
		ctx->media_done = ctx->header_end;
		ctx->is_playing = GF_TRUE;
		//post a seek
		ctx->in_seek = GF_TRUE;

		//lcoate previous RAP sample
		while ((node = (GF_XMLNode *) gf_list_enum(ctx->root->content, &i))) {
			u32 j=0;
			u64 dts=0;
			u32 datalen=0;
			Bool is_rap = ctx->has_sap ? GF_FALSE : GF_TRUE;
			s32 cts_offset=0;
			u64 sample_duration = 0;
			GF_XMLAttribute *att;
			if (node->type) continue;
			if (stricmp(node->name, ctx->is_dims ? "DIMSUnit" : "NHNTSample") ) continue;

			while ( (att = (GF_XMLAttribute *)gf_list_enum(node->attributes, &j))) {
				if (!stricmp(att->name, "DTS") || !stricmp(att->name, "time")) {
					u32 h, m, s, ms;
					u64 dst_val;
					if (strchr(att->value, ':') && sscanf(att->value, "%u:%u:%u.%u", &h, &m, &s, &ms) == 4) {
						dts = (u64) ( (Double) ( ((h*3600.0 + m*60.0 + s)*1000 + ms) / 1000.0) * ctx->timescale );
					} else if (sscanf(att->value, ""LLU, &dst_val)==1) {
						dts = dst_val;
					}
				}
				else if (!stricmp(att->name, "CTSOffset")) cts_offset = atoi(att->value);
				else if (!stricmp(att->name, "duration") ) sscanf(att->value, ""LLU, &sample_duration);
				else if (!stricmp(att->name, "isRAP") ) {
					is_rap = (!stricmp(att->value, "yes")) ? GF_TRUE : GF_FALSE;
				}
				else if (!stricmp(att->name, "mediaOffset"))
					byte_offset = (s64) atof(att->value) ;
				else if (!stricmp(att->name, "dataLength"))
					datalen = atoi(att->value);
			}

			dts += cts_offset;
			if ((s64) dts < 0) dts = 0;

			if (dts) cur_dts = dts;
			if (sample_duration) cur_dts += sample_duration;
			else if (ctx->dts_inc) cur_dts += ctx->dts_inc;

			if (cur_dts >= ctx->timescale * evt->play.start_range) {
				break;
			}
			if (is_rap) {
				ctx->current_child_idx = i-1;
				ctx->media_done = byte_offset;
				ctx->sample_num = sample_num;
			}
			byte_offset += datalen;
			sample_num++;
		}

		//cancel event
		return GF_TRUE;

	case GF_FEVT_STOP:
		ctx->is_playing = GF_FALSE;
		//don't cancel event
		return GF_FALSE;

	case GF_FEVT_SET_SPEED:
		//cancel event
		return GF_TRUE;
	default:
		break;
	}
	//by default don't cancel event - to rework once we have downloading in place
	return GF_FALSE;
}