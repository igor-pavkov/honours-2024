static GF_Err BM_ParseMultipleIndexedReplace(GF_BifsDecoder *codec, GF_BitStream *bs, GF_List *com_list)
{
	u32 ID, ind, field_ind, NumBits, lenpos, lennum, count;
	GF_Node *node;
	GF_Err e;
	GF_Command *com;
	GF_CommandField *inf;
	GF_FieldInfo field;

	ID = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);
	node = gf_sg_find_node(codec->current_graph, ID);
	if (!node) return GF_NON_COMPLIANT_BITSTREAM;
	NumBits = gf_get_bit_size(gf_node_get_num_fields_in_mode(node, GF_SG_FIELD_CODING_IN)-1);
	ind = gf_bs_read_int(bs, NumBits);
	e = gf_bifs_get_field_index(node, ind, GF_SG_FIELD_CODING_IN, &field_ind);
	if (e) return e;
	e = gf_node_get_field(node, field_ind, &field);
	if (gf_sg_vrml_is_sf_field(field.fieldType)) return GF_NON_COMPLIANT_BITSTREAM;

	lenpos = gf_bs_read_int(bs, 5);
	lennum = gf_bs_read_int(bs, 5);
	count = gf_bs_read_int(bs, lennum);

	com = gf_sg_command_new(codec->current_graph, GF_SG_MULTIPLE_INDEXED_REPLACE);
	BM_SetCommandNode(com, node);
	field.fieldType = gf_sg_vrml_get_sf_type(field.fieldType);

	while (count) {
		inf = gf_sg_command_field_new(com);
		inf->pos = gf_bs_read_int(bs, lenpos);
		inf->fieldIndex = field.fieldIndex;
		inf->fieldType = field.fieldType;

		if (field.fieldType==GF_SG_VRML_SFNODE) {
			inf->new_node = gf_bifs_dec_node(codec, bs, field.NDTtype);
			if (codec->LastError) goto err;
			inf->field_ptr = &inf->new_node;
			gf_node_register(inf->new_node, NULL);
		} else {
			field.far_ptr = inf->field_ptr = gf_sg_vrml_field_pointer_new(inf->fieldType);
			e = gf_bifs_dec_sf_field(codec, bs, node, &field, GF_TRUE);
			if (e) goto err;
		}
		count--;
	}
err:
	if (e) gf_sg_command_del(com);
	else gf_list_add(com_list, com);
	return e;
}