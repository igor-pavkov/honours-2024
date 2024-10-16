Bool gf_vvc_slice_is_ref(VVCState *vvc)
{
	if (!vvc->s_info.irap_or_gdr_pic) {
		return GF_FALSE;
	}
	if (vvc->s_info.gdr_pic) {
		if (vvc->s_info.recovery_point_valid) {
			vvc->s_info.recovery_point_valid = 0;
			return GF_TRUE;
		}
		return GF_FALSE;
	}
	return GF_TRUE;
}