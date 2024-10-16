GF_Err BM_ParseInsert(GF_BifsDecoder *codec, GF_BitStream *bs, GF_List *com_list)
{
	u8 type;

	type = gf_bs_read_int(bs, 2);
	switch (type) {
	case 0:
		return BM_ParseNodeInsert(codec, bs, com_list);
	case 1:
		return BM_ParseExtendedUpdates(codec, bs, com_list);
	case 2:
		return BM_ParseIndexInsert(codec, bs, com_list);
	case 3:
		return BM_ParseRouteInsert(codec, bs, com_list);
	default:
		return GF_NON_COMPLIANT_BITSTREAM;
	}
}