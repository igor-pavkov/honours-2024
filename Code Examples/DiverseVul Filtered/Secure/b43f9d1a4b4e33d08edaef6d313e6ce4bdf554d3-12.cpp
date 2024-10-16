static const char *naludmx_probe_data(const u8 *data, u32 size, GF_FilterProbeScore *score)
{
	u32 sc, sc_size;
	u32 not_hevc=0;
	u32 not_avc=0;
	u32 not_vvc=0;
	u32 nb_hevc=0;
	u32 nb_avc=0;
	u32 nb_vvc=0;
	u32 nb_nalus=0;
	u32 nb_hevc_zero=0;
	u32 nb_avc_zero=0;
	u32 nb_vvc_zero=0;
	u32 nb_sps_hevc=0,nb_pps_hevc=0,nb_vps_hevc=0;
	u32 nb_sps_avc=0,nb_pps_avc=0;
	u32 nb_sps_vvc=0,nb_pps_vvc=0,nb_vps_vvc=0;

	while (size>3) {
		u32 nal_type=0;
		sc_size = 0;
		sc = gf_media_nalu_next_start_code(data, size, &sc_size);
		if (!sc_size) break;

		data += sc + sc_size;
		if (size <= sc + sc_size) break;
		size -= sc + sc_size;

		if (data[0] & 0x80) {
			not_avc++;
			not_hevc++;
			not_vvc++;
			continue;
		}
		nb_nalus++;

		nal_type = (data[0] & 0x7E) >> 1;
		if (nal_type<=40) {
			nb_hevc++;
			switch (nal_type) {
			case GF_HEVC_NALU_PIC_PARAM:
				if (nb_sps_hevc)
					nb_pps_hevc++;
				break;
			case GF_HEVC_NALU_SEQ_PARAM:
				nb_sps_hevc++;
				break;
			case GF_HEVC_NALU_VID_PARAM:
				nb_vps_hevc++;
				break;
			case 0:
				nb_hevc_zero++;
				break;
			}
		} else {
			if ((nal_type!=GF_HEVC_NALU_DV_RPU) && (nal_type!=GF_HEVC_NALU_DV_EL))
				not_hevc++;
		}

		nal_type = data[0] & 0x1F;
		if (nal_type && nal_type<=24) {
			nb_avc++;
			switch (nal_type) {
			case GF_AVC_NALU_PIC_PARAM:
				if (nb_sps_avc)
					nb_pps_avc++;
				break;
			case GF_AVC_NALU_SEQ_PARAM:
				nb_sps_avc++;
				break;
			case 0:
				nb_avc_zero++;
				break;
			}
		} else {
			if ((nal_type!=GF_AVC_NALU_DV_RPU) && (nal_type!=GF_AVC_NALU_DV_EL))
				not_avc++;
		}

		//check vvc - 2nd bit reserved to 0
		if (data[0] & 0x40) {
			not_vvc++;
			continue;
		}
		nal_type = data[1] >> 3;
		if (nal_type>31) {
			not_vvc++;
			continue;
		}
		nb_vvc++;
		switch (nal_type) {
		case GF_VVC_NALU_PIC_PARAM:
			if (nb_sps_vvc)
				nb_pps_vvc++;
			break;
		case GF_VVC_NALU_SEQ_PARAM:
			nb_sps_vvc++;
			break;
		case GF_VVC_NALU_VID_PARAM:
			nb_vps_vvc++;
			break;
		case GF_VVC_NALU_ACCESS_UNIT:
			//to detect files without VPS correctly
			nb_vps_vvc++;
			break;
		case 0:
			nb_vvc_zero++;
			break;
		}
	}

	if (!nb_sps_avc || !nb_pps_avc) nb_avc=0;
	if (!nb_sps_hevc || !nb_pps_hevc || !nb_vps_hevc) nb_hevc=0;
	//VPS is optional in VVC, don't check for its presence
	if (!nb_sps_vvc || !nb_pps_vvc) nb_vvc=0;
	if (not_avc) nb_avc=0;
	if (not_hevc) nb_hevc=0;
	if (not_vvc) nb_vvc=0;

	if (not_avc && not_hevc && not_vvc) return NULL;
	if (nb_avc==nb_avc_zero) nb_avc=0;
	if (nb_hevc==nb_hevc_zero) nb_hevc=0;
	if (nb_vvc==nb_vvc_zero) nb_vvc=0;

	if (!nb_hevc && !nb_avc && !nb_vvc) return NULL;
	*score = GF_FPROBE_SUPPORTED;
	if (!nb_hevc) return (nb_vvc>nb_avc) ? "video/vvc" : "video/avc";
	if (!nb_avc) return (nb_vvc>nb_hevc) ? "video/vvc" : "video/hevc";
	if (!nb_vvc) return (nb_avc>nb_hevc) ? "video/avc" : "video/hevc";

	if ((nb_hevc>nb_avc) && (nb_hevc>nb_vvc)) return "video/hevc";
	if ((nb_vvc>nb_avc) && (nb_vvc>nb_hevc)) return "video/vvc";
	return "video/avc";
}