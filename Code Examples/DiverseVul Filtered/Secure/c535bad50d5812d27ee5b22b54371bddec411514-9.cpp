GF_Err BM_ParseRouteReplace(GF_BifsDecoder *codec, GF_BitStream *bs, GF_List *com_list)
{
	GF_Err e;
	GF_Command *com;
	u32 RouteID, numBits, ind, node_id, fromID, toID;
	GF_Node *OutNode, *InNode;

	RouteID = 1+gf_bs_read_int(bs, codec->info->config.RouteIDBits);

	/*origin*/
	node_id = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);
	OutNode = gf_sg_find_node(codec->current_graph, node_id);
	if (!OutNode) return GF_NON_COMPLIANT_BITSTREAM;
	numBits = gf_get_bit_size(gf_node_get_num_fields_in_mode(OutNode, GF_SG_FIELD_CODING_OUT) - 1);
	ind = gf_bs_read_int(bs, numBits);
	e = gf_bifs_get_field_index(OutNode, ind, GF_SG_FIELD_CODING_OUT, &fromID);
	if (e) return e;

	/*target*/
	node_id = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);
	InNode = gf_sg_find_node(codec->current_graph, node_id);
	if (!InNode) return GF_NON_COMPLIANT_BITSTREAM;
	numBits = gf_get_bit_size(gf_node_get_num_fields_in_mode(InNode, GF_SG_FIELD_CODING_IN) - 1);
	ind = gf_bs_read_int(bs, numBits);
	e = gf_bifs_get_field_index(InNode, ind, GF_SG_FIELD_CODING_IN, &toID);
	if (e) return e;

	com = gf_sg_command_new(codec->current_graph, GF_SG_ROUTE_REPLACE);
	com->RouteID = RouteID;
	com->fromNodeID = gf_node_get_id(OutNode);
	com->fromFieldIndex = fromID;
	com->toNodeID = gf_node_get_id(InNode);
	com->toFieldIndex = toID;
	gf_list_add(com_list, com);
	return codec->LastError;
}