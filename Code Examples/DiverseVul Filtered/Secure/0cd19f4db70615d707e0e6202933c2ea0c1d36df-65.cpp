Bool gf_avc_slice_is_intra(AVCState *avc)
{
	switch (avc->s_info.slice_type) {
	case GF_AVC_TYPE_I:
	case GF_AVC_TYPE2_I:
	case GF_AVC_TYPE_SI:
	case GF_AVC_TYPE2_SI:
		return 1;
	default:
		return 0;
	}
}