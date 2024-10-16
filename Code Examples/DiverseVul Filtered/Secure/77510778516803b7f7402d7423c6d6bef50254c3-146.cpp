static void ctrn_write_ctso(GF_TrackFragmentRunBox *ctrn, GF_BitStream *bs, u32 ctso, u32 field_size)
{
	if (!field_size) return;

	if (ctrn->ctso_multiplier) {
		gf_bs_write_int(bs, ctso / ctrn->ctso_multiplier, field_size);
	} else {
		gf_bs_write_int(bs, ctso, field_size);
	}
}