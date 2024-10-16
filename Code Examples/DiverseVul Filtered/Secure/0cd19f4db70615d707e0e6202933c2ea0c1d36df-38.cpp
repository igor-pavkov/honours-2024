static GF_Err gf_m4v_parse_config_mpeg12(GF_M4VParser *m4v, GF_M4VDecSpecInfo *dsi)
{
	unsigned char p[4];
	u32 ext_type;
	s32 o_type;
	u8 go, par;

	if (!m4v || !dsi) return GF_BAD_PARAM;

	memset(dsi, 0, sizeof(GF_M4VDecSpecInfo));
	dsi->VideoPL = 0;

	go = 1;
	while (go) {
		o_type = M4V_LoadObject(m4v);
		switch (o_type) {
		case M2V_SEQ_START_CODE:
			dsi->RAP_stream = 1;
			gf_bs_read_data(m4v->bs, (char *)p, 4);
			dsi->width = (p[0] << 4) | ((p[1] >> 4) & 0xf);
			dsi->height = ((p[1] & 0xf) << 8) | p[2];

			dsi->VideoPL = GF_CODECID_MPEG1;
			par = (p[3] >> 4) & 0xf;
			switch (par) {
			case 2:
				dsi->par_num = dsi->height / 3;
				dsi->par_den = dsi->width / 4;
				break;
			case 3:
				dsi->par_num = dsi->height / 9;
				dsi->par_den = dsi->width / 16;
				break;
			case 4:
				dsi->par_num = dsi->height / 2;
				dsi->par_den = dsi->width / 21;
				break;
			default:
				dsi->par_den = dsi->par_num = 0;
				break;
			}
			switch (p[3] & 0xf) {
			case 0:
				break;
			case 1:
				dsi->fps = 24000.0 / 1001.0;
				break;
			case 2:
				dsi->fps = 24.0;
				break;
			case 3:
				dsi->fps = 25.0;
				break;
			case 4:
				dsi->fps = 30000.0 / 1001.0;
				break;
			case 5:
				dsi->fps = 30.0;
				break;
			case 6:
				dsi->fps = 50.0;
				break;
			case 7:
				dsi->fps = ((60.0*1000.0) / 1001.0);
				break;
			case 8:
				dsi->fps = 60.0;
				break;
			case 9:
				dsi->fps = 1;
				break;
			case 10:
				dsi->fps = 5;
				break;
			case 11:
				dsi->fps = 10;
				break;
			case 12:
				dsi->fps = 12;
				break;
			case 13:
				dsi->fps = 15;
				break;
			}
			break;
		case M2V_EXT_START_CODE:
			gf_bs_read_data(m4v->bs, (char *)p, 4);
			ext_type = ((p[0] >> 4) & 0xf);
			if (ext_type == 1) {
				dsi->VideoPL = 0x65;
				dsi->height = ((p[1] & 0x1) << 13) | ((p[2] & 0x80) << 5) | (dsi->height & 0x0fff);
				dsi->width = (((p[2] >> 5) & 0x3) << 12) | (dsi->width & 0x0fff);
			}
			break;
		case M2V_PIC_START_CODE:
			if (dsi->width) go = 0;
			break;
		default:
			break;
			/*EOS*/
		case -1:
			go = 0;
			m4v->current_object_start = gf_bs_get_position(m4v->bs);
			break;
		}
	}
	return GF_OK;
}