GF_Err BM_ParseNodeReplace(GF_BifsDecoder *codec, GF_BitStream *bs, GF_List *com_list)
{
	u32 NodeID;
	GF_Command *com;
	GF_Node *node;
	GF_CommandField *inf;

	NodeID = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);
	/*this is delete / new on a DEF node: replace ALL instances*/
	node = gf_sg_find_node(codec->current_graph, NodeID);
	if (!node) return GF_NON_COMPLIANT_BITSTREAM;

	com = gf_sg_command_new(codec->current_graph, GF_SG_NODE_REPLACE);
	BM_SetCommandNode(com, node);
	inf = gf_sg_command_field_new(com);
	inf->new_node = gf_bifs_dec_node(codec, bs, NDT_SFWorldNode);
	inf->fieldType = GF_SG_VRML_SFNODE;
	inf->field_ptr = &inf->new_node;
	gf_list_add(com_list, com);
	gf_node_register(inf->new_node, NULL);
	return codec->LastError;
}