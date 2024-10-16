Bool gf_avc_slice_is_IDR(AVCState *avc)
{
	if (avc->sei.recovery_point.valid)
	{
		avc->sei.recovery_point.valid = 0;
		return 1;
	}
	if (avc->s_info.nal_unit_type != GF_AVC_NALU_IDR_SLICE)
		return 0;
	return gf_avc_slice_is_intra(avc);
}