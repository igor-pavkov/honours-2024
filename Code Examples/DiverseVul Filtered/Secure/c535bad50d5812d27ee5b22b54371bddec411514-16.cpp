GF_Err BM_ParseIndexValueReplace(GF_BifsDecoder *codec, GF_BitStream *bs, GF_List *com_list)
{
	u32 NodeID, ind, field_ind, NumBits;
	s32 type, pos;
	GF_Command *com;
	GF_Node *node;
	GF_Err e;
	GF_FieldInfo field, sffield;
	GF_CommandField *inf;

	/*get the node*/
	NodeID = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);

	node = gf_sg_find_node(codec->current_graph, NodeID);
	if (!node) return GF_NON_COMPLIANT_BITSTREAM;
	NumBits = gf_get_bit_size(gf_node_get_num_fields_in_mode(node, GF_SG_FIELD_CODING_IN)-1);
	ind = gf_bs_read_int(bs, NumBits);
	e = gf_bifs_get_field_index(node, ind, GF_SG_FIELD_CODING_IN, &field_ind);
	if (e) return e;

	e = gf_node_get_field(node, field_ind, &field);
	if (e) return e;
	if (gf_sg_vrml_is_sf_field(field.fieldType)) return GF_NON_COMPLIANT_BITSTREAM;

	type = gf_bs_read_int(bs, 2);
	switch (type) {
	case 0:
		pos = gf_bs_read_int(bs, 16);
		break;
	case 2:
		pos = 0;
		break;
	case 3:
		pos = ((GenMFField *) field.far_ptr)->count - 1;
		break;
	default:
		return GF_NON_COMPLIANT_BITSTREAM;
	}

	com = gf_sg_command_new(codec->current_graph, GF_SG_INDEXED_REPLACE);
	BM_SetCommandNode(com, node);
	inf = gf_sg_command_field_new(com);
	inf->fieldIndex = field.fieldIndex;
	inf->pos = pos;

	if (field.fieldType == GF_SG_VRML_MFNODE) {
		inf->fieldType = GF_SG_VRML_SFNODE;
		inf->new_node = gf_bifs_dec_node(codec, bs, field.NDTtype);
		inf->field_ptr = &inf->new_node;
		if (inf->new_node) gf_node_register(inf->new_node, NULL);
	} else {
		memcpy(&sffield, &field, sizeof(GF_FieldInfo));
		sffield.fieldType = gf_sg_vrml_get_sf_type(field.fieldType);
		inf->fieldType = sffield.fieldType;
		sffield.far_ptr = inf->field_ptr = gf_sg_vrml_field_pointer_new(sffield.fieldType);
		codec->LastError = gf_bifs_dec_sf_field(codec, bs, node, &sffield, GF_TRUE);
	}
	gf_list_add(com_list, com);
	return codec->LastError;
}