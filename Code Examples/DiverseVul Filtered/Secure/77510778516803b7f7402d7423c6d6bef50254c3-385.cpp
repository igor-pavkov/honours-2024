static u32 ctrn_read_flags(GF_BitStream *bs, u32 nbbits)
{
	u32 val = gf_bs_read_int(bs, nbbits);
	if (nbbits==16) val <<= 16;
	else if (nbbits==8) val <<= 24;
	return val;
}