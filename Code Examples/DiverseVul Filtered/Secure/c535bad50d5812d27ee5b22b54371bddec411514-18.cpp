GF_Err BM_ParseDelete(GF_BifsDecoder *codec, GF_BitStream *bs, GF_List *com_list)
{
	u8 type;
	u32 ID;
	GF_Command *com;
	GF_Node *n;

	type = gf_bs_read_int(bs, 2);
	switch (type) {
	case 0:
		ID = 1+gf_bs_read_int(bs, codec->info->config.NodeIDBits);
		n = gf_sg_find_node(codec->current_graph, ID);
		if (!n) return GF_OK;
		com = gf_sg_command_new(codec->current_graph, GF_SG_NODE_DELETE);
		BM_SetCommandNode(com, n);
		gf_list_add(com_list, com);
		return GF_OK;
	case 2:
		return BM_ParseIndexDelete(codec, bs, com_list);
	case 3:
		com = gf_sg_command_new(codec->current_graph, GF_SG_ROUTE_DELETE);
		com->RouteID = 1+gf_bs_read_int(bs, codec->info->config.RouteIDBits);
		gf_list_add(com_list, com);
		return GF_OK;
	default:
		return GF_NON_COMPLIANT_BITSTREAM;
	}
	return GF_OK;
}