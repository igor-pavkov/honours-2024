GF_Err BM_ParseReplace(GF_BifsDecoder *codec, GF_BitStream *bs, GF_List *com_list)
{
	u8 type;
	type = gf_bs_read_int(bs, 2);
	switch (type) {
	case 0:
		return BM_ParseNodeReplace(codec, bs, com_list);
	case 1:
		return BM_ParseFieldReplace(codec, bs, com_list);
	case 2:
		return BM_ParseIndexValueReplace(codec, bs, com_list);
	case 3:
		return BM_ParseRouteReplace(codec, bs, com_list);
	}
	return GF_OK;
}