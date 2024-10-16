GF_Err BM_ParseNodeInsert(GF_BifsDecoder *codec, GF_BitStream *bs, GF_List *com_list)
{
	u32 NodeID, NDT;
	GF_CommandField *inf;
	s32 type, pos;
	GF_Node *node, *def;

	NodeID = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);
	def = gf_sg_find_node(codec->current_graph, NodeID);
	if (!def) return GF_NON_COMPLIANT_BITSTREAM;
	NDT = gf_bifs_get_child_table(def);
	if (!NDT) return GF_NON_COMPLIANT_BITSTREAM;

	type = gf_bs_read_int(bs, 2);
	switch (type) {
	case 0:
		pos = gf_bs_read_int(bs, 8);
		break;
	case 2:
		pos = 0;
		break;
	case 3:
		/*-1 means append*/
		pos = -1;
		break;
	default:
		return GF_NON_COMPLIANT_BITSTREAM;
	}
	node = gf_bifs_dec_node(codec, bs, NDT);
	if (!codec->LastError) {
		GF_Command *com = gf_sg_command_new(codec->current_graph, GF_SG_NODE_INSERT);
		BM_SetCommandNode(com, def);
		inf = gf_sg_command_field_new(com);
		inf->pos = pos;
		inf->new_node = node;
		inf->field_ptr = &inf->new_node;
		inf->fieldType = GF_SG_VRML_SFNODE;
		gf_list_add(com_list, com);
		/*register*/
		gf_node_register(node, NULL);
	}
	return codec->LastError;
}