GF_Err BM_ParseIndexDelete(GF_BifsDecoder *codec, GF_BitStream *bs, GF_List *com_list)
{
	u32 NodeID, NumBits, ind, field_ind;
	s32 pos;
	GF_Command *com;
	u8 type;
	GF_Node *node;
	GF_Err e;
	GF_CommandField *inf;
	GF_FieldInfo field;

	NodeID = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);
	node = gf_sg_find_node(codec->current_graph, NodeID);
	if (!node) return GF_NON_COMPLIANT_BITSTREAM;

	NumBits = gf_get_bit_size(gf_node_get_num_fields_in_mode(node, GF_SG_FIELD_CODING_IN) - 1);
	ind = gf_bs_read_int(bs, NumBits);

	type = gf_bs_read_int(bs, 2);
	switch (type) {
	case 0:
		pos = (u32) gf_bs_read_int(bs, 16);
		break;
	case 2:
		pos = 0;
		break;
	case 3:
		pos = -1;
		break;
	default:
		return GF_NON_COMPLIANT_BITSTREAM;
	}
	e = gf_bifs_get_field_index(node, ind, GF_SG_FIELD_CODING_IN, &field_ind);
	if (e) return e;
	e = gf_node_get_field(node, field_ind, &field);
	if (e) return e;
	if (gf_sg_vrml_is_sf_field(field.fieldType)) return GF_NON_COMPLIANT_BITSTREAM;
	com = gf_sg_command_new(codec->current_graph, GF_SG_INDEXED_DELETE);
	BM_SetCommandNode(com, node);
	inf = gf_sg_command_field_new(com);
	inf->pos = pos;
	inf->fieldIndex = field.fieldIndex;
	inf->fieldType = gf_sg_vrml_get_sf_type(field.fieldType);
	gf_list_add(com_list, com);
	return codec->LastError;
}