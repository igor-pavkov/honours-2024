GF_Err BM_ParseFieldReplace(GF_BifsDecoder *codec, GF_BitStream *bs, GF_List *com_list)
{
	GF_Err e;
	GF_Command *com;
	u32 NodeID, ind, field_ind, NumBits;
	GF_Node *node;
	GF_FieldInfo field;
	GF_CommandField *inf;

	NodeID = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);
	node = gf_sg_find_node(codec->current_graph, NodeID);
	if (!node) return GF_NON_COMPLIANT_BITSTREAM;
	NumBits = gf_get_bit_size(gf_node_get_num_fields_in_mode(node, GF_SG_FIELD_CODING_IN)-1);
	ind = gf_bs_read_int(bs, NumBits);
	e = gf_bifs_get_field_index(node, ind, GF_SG_FIELD_CODING_IN, &field_ind);
	if (e) return e;

	e = gf_node_get_field(node, field_ind, &field);
	if (e) return e;

	com = gf_sg_command_new(codec->current_graph, GF_SG_FIELD_REPLACE);
	BM_SetCommandNode(com, node);
	inf = gf_sg_command_field_new(com);
	inf->fieldIndex = field_ind;
	inf->fieldType = field.fieldType;
	if (inf->fieldType == GF_SG_VRML_SFNODE) {
		field.far_ptr = inf->field_ptr = &inf->new_node;
	} else if (inf->fieldType == GF_SG_VRML_MFNODE) {
		field.far_ptr = inf->field_ptr = &inf->node_list;
	} else {
		field.far_ptr = inf->field_ptr = gf_sg_vrml_field_pointer_new(field.fieldType);
	}
	/*parse the field*/
	codec->LastError = gf_bifs_dec_field(codec, bs, node, &field, GF_TRUE);

	gf_list_add(com_list, com);
	return codec->LastError;
}