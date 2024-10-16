static GF_Err BM_ParseExtendedUpdates(GF_BifsDecoder *codec, GF_BitStream *bs, GF_List *com_list)
{
	u32 type = gf_bs_read_int(bs, 8);
	GF_Err e;

	switch (type) {
	case 0:
	{
		GF_Command *com = gf_sg_command_new(codec->current_graph, GF_SG_PROTO_INSERT);
		e = gf_bifs_dec_proto_list(codec, bs, com->new_proto_list);
		if (e) gf_sg_command_del(com);
		else gf_list_add(com_list, com);
	}
	return e;
	case 1:
		return BM_ParseProtoDelete(codec, bs, com_list);
	case 2:
	{
		GF_Command *com = gf_sg_command_new(codec->current_graph, GF_SG_PROTO_DELETE_ALL);
		return gf_list_add(com_list, com);
	}
	case 3:
		return BM_ParseMultipleIndexedReplace(codec, bs, com_list);
	case 4:
		return BM_ParseMultipleReplace(codec, bs, com_list);
	case 5:
		return BM_ParseGlobalQuantizer(codec, bs, com_list);
	case 6:
	{
		GF_Command *com;
		u32 ID = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);
		GF_Node *n = gf_sg_find_node(codec->current_graph, ID);
		if (!n) return GF_OK;
		com = gf_sg_command_new(codec->current_graph, GF_SG_NODE_DELETE_EX);
		BM_SetCommandNode(com, n);
		gf_list_add(com_list, com);
	}
	return GF_OK;
	case 7:
		return BM_XReplace(codec, bs, com_list);

	default:
		return GF_BIFS_UNKNOWN_VERSION;
	}
}