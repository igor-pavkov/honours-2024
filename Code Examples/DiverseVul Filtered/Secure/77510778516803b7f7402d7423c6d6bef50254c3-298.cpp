static u32 ctrn_ctts_to_index(GF_TrackFragmentRunBox *ctrn, s32 ctts)
{
	if (!(ctrn->flags & GF_ISOM_TRUN_CTS_OFFSET))
		return 0;

	if (!ctts) return 0;

	if (ctrn->version) {
		if (ctrn->ctso_multiplier) return ctrn_s32_to_index(ctts / ctrn->ctso_multiplier);
		return ctrn_s32_to_index(ctts);
	}
	assert(ctts>0);
	if (ctrn->ctso_multiplier) return ctrn_u32_to_index((u32)ctts / ctrn->ctso_multiplier);
	return ctrn_s32_to_index((u32)ctts);
}