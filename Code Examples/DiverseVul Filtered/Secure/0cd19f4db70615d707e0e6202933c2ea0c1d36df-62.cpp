static GF_Err gf_m4v_parse_frame_mpeg12(GF_M4VParser *m4v, GF_M4VDecSpecInfo *dsi, u8 *frame_type, u32 *time_inc, u64 *size, u64 *start, Bool *is_coded)
{
	u8 go, hasVOP, firstObj, val;
	s32 o_type;

	if (!m4v || !size || !start || !frame_type) return GF_BAD_PARAM;

	*size = 0;
	firstObj = 1;
	hasVOP = 0;
	*is_coded = GF_FALSE;
	*frame_type = 0;

	if (!m4v->step_mode)
		M4V_Reset(m4v, m4v->current_object_start);

	m4v->current_object_type = (u32)-1;
	go = 1;
	while (go) {
		o_type = M4V_LoadObject(m4v);
		switch (o_type) {
		case M2V_PIC_START_CODE:
			/*done*/
			if (hasVOP) {
				go = 0;
				break;
			}
			if (firstObj) {
				*start = m4v->current_object_start;
				firstObj = 0;
			}
			hasVOP = 1;
			*is_coded = 1;

			/*val = */gf_bs_read_u8(m4v->bs);
			val = gf_bs_read_u8(m4v->bs);
			*frame_type = ((val >> 3) & 0x7) - 1;
			break;
		case M2V_GOP_START_CODE:
			if (firstObj) {
				*start = m4v->current_object_start;
				firstObj = 0;
			}
			if (hasVOP) go = 0;
			break;

		case M2V_SEQ_START_CODE:
			if (firstObj) {
				*start = m4v->current_object_start;
				firstObj = 0;
			}
			if (hasVOP) {
				go = 0;
				break;
			}

			/**/
			break;

		default:
			break;

		case -1:
			*size = gf_bs_get_position(m4v->bs) - *start;
			return GF_EOS;
		}
		if (m4v->step_mode)
			return GF_OK;
	}
	*size = m4v->current_object_start - *start;
	return GF_OK;
}