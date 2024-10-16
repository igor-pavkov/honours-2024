GF_Err BM_ParseRouteInsert(GF_BifsDecoder *codec, GF_BitStream *bs, GF_List *com_list)
{
	GF_Err e;
	u8 flag;
	GF_Command *com;
	GF_Node *InNode, *OutNode;
	u32 RouteID, outField, inField, numBits, ind, node_id;
	char name[1000];

	RouteID = 0;

	flag = gf_bs_read_int(bs, 1);
	/*def'ed route*/
	if (flag) {
		RouteID = 1 + gf_bs_read_int(bs, codec->info->config.RouteIDBits);
		if (codec->UseName) gf_bifs_dec_name(bs, name, 1000);
	}
	/*origin*/
	node_id = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);
	OutNode = gf_sg_find_node(codec->current_graph, node_id);
	if (!OutNode) return GF_SG_UNKNOWN_NODE;

	numBits = gf_node_get_num_fields_in_mode(OutNode, GF_SG_FIELD_CODING_OUT) - 1;
	numBits = gf_get_bit_size(numBits);
	ind = gf_bs_read_int(bs, numBits);
	e = gf_bifs_get_field_index(OutNode, ind, GF_SG_FIELD_CODING_OUT, &outField);
	if (e) return e;

	/*target*/
	node_id = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);
	InNode = gf_sg_find_node(codec->current_graph, node_id);
	if (!InNode) return GF_SG_UNKNOWN_NODE;

	numBits = gf_node_get_num_fields_in_mode(InNode, GF_SG_FIELD_CODING_IN) - 1;
	numBits = gf_get_bit_size(numBits);
	ind = gf_bs_read_int(bs, numBits);
	e = gf_bifs_get_field_index(InNode, ind, GF_SG_FIELD_CODING_IN, &inField);
	if (e) return e;

	com = gf_sg_command_new(codec->current_graph, GF_SG_ROUTE_INSERT);
	com->RouteID = RouteID;
	if (codec->UseName) com->def_name = gf_strdup( name);
	com->fromNodeID = gf_node_get_id(OutNode);
	com->fromFieldIndex = outField;
	com->toNodeID = gf_node_get_id(InNode);
	com->toFieldIndex = inField;
	gf_list_add(com_list, com);
	return codec->LastError;
}