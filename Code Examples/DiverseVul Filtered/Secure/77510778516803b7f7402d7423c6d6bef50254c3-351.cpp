static void ctrn_write_sample_flags(GF_BitStream *bs, u32 flags, u32 field_size)
{
	if (!field_size) return;

	if (field_size==8) flags = flags>>24;
	else if (field_size==16) flags = flags>>16;
	gf_bs_write_int(bs, flags, field_size);
}