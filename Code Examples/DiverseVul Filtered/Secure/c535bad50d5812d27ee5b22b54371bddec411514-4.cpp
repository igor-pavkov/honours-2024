GF_Err BM_ParseIndexInsert(GF_BifsDecoder *codec, GF_BitStream *bs, GF_List *com_list)
{
	GF_Err e;
	u32 NodeID;
	u32 NumBits, ind, field_ind;
	u8 type;
	GF_Command *com;
	GF_CommandField *inf;
	s32 pos;
	GF_Node *def, *node;
	GF_FieldInfo field, sffield;

	NodeID = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);
	def = gf_sg_find_node(codec->current_graph, NodeID);
	if (!def) return GF_NON_COMPLIANT_BITSTREAM;
	/*index insertion uses IN mode for field index*/
	NumBits = gf_get_bit_size(gf_node_get_num_fields_in_mode(def, GF_SG_FIELD_CODING_IN)-1);
	ind = gf_bs_read_int(bs, NumBits);

	e = gf_bifs_get_field_index(def, ind, GF_SG_FIELD_CODING_IN, &field_ind);
	if (e) return e;

	type = gf_bs_read_int(bs, 2);
	switch (type) {
	case 0:
		pos = gf_bs_read_int(bs, 16);
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

	e = gf_node_get_field(def, field_ind, &field);
	if (e) return e;
	if (gf_sg_vrml_is_sf_field(field.fieldType)) return GF_NON_COMPLIANT_BITSTREAM;

	memcpy(&sffield, &field, sizeof(GF_FieldInfo));
	sffield.fieldType = gf_sg_vrml_get_sf_type(field.fieldType);

	/*rescale the MFField and parse the SFField*/
	if (field.fieldType==GF_SG_VRML_MFNODE) {
		node = gf_bifs_dec_node(codec, bs, field.NDTtype);
		if (!codec->LastError) {
			com = gf_sg_command_new(codec->current_graph, GF_SG_INDEXED_INSERT);
			BM_SetCommandNode(com, def);
			inf = gf_sg_command_field_new(com);
			inf->pos = pos;
			inf->fieldIndex = field_ind;
			inf->fieldType = sffield.fieldType;
			inf->new_node = node;
			inf->field_ptr = &inf->new_node;
			gf_list_add(com_list, com);
			/*register*/
			gf_node_register(node, NULL);
		}
	} else {
		com = gf_sg_command_new(codec->current_graph, GF_SG_INDEXED_INSERT);
		BM_SetCommandNode(com, def);
		inf = gf_sg_command_field_new(com);
		inf->pos = pos;
		inf->fieldIndex = field_ind;
		inf->fieldType = sffield.fieldType;
		sffield.far_ptr = inf->field_ptr = gf_sg_vrml_field_pointer_new(sffield.fieldType);
		codec->LastError = gf_bifs_dec_sf_field(codec, bs, def, &sffield, GF_TRUE);
		gf_list_add(com_list, com);
	}
	return codec->LastError;
}