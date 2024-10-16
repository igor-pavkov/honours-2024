static GF_Err BM_ParseMultipleReplace(GF_BifsDecoder *codec, GF_BitStream *bs, GF_List *com_list)
{
	u32 i, numFields, index, flag, nbBits, field_ref, fieldind;
	GF_Err e;
	GF_FieldInfo field;
	u32 NodeID;
	GF_Node *node;
	GF_Command *com;
	GF_CommandField *inf;

	NodeID = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);
	node = gf_sg_find_node(codec->current_graph, NodeID);
	if (!node) return GF_NON_COMPLIANT_BITSTREAM;

	e = GF_OK;
	com = gf_sg_command_new(codec->current_graph, GF_SG_MULTIPLE_REPLACE);
	BM_SetCommandNode(com, node);
	flag = gf_bs_read_int(bs, 1);
	if (flag) {
		numFields = gf_node_get_num_fields_in_mode(node, GF_SG_FIELD_CODING_DEF);
		for (i=0; i<numFields; i++) {
			flag = gf_bs_read_int(bs, 1);
			if (!flag) continue;
			gf_bifs_get_field_index(node, i, GF_SG_FIELD_CODING_DEF, &index);
			e = gf_node_get_field(node, index, &field);
			if (e) goto exit;
			inf = gf_sg_command_field_new(com);
			inf->fieldType = field.fieldType;
			inf->fieldIndex = field.fieldIndex;
			if (inf->fieldType==GF_SG_VRML_SFNODE) {
				field.far_ptr = inf->field_ptr = &inf->new_node;
			} else if (inf->fieldType==GF_SG_VRML_MFNODE) {
				field.far_ptr = inf->field_ptr = &inf->node_list;
			} else {
				field.far_ptr = inf->field_ptr = gf_sg_vrml_field_pointer_new(inf->fieldType);
			}
			e = gf_bifs_dec_field(codec, bs, node, &field, GF_TRUE);
			if (e) goto exit;
		}
	} else {
		flag = gf_bs_read_int(bs, 1);
		nbBits = gf_get_bit_size(gf_node_get_num_fields_in_mode(node, GF_SG_FIELD_CODING_DEF)-1);
		while (!flag && (codec->LastError>=0)) {
			field_ref = gf_bs_read_int(bs, nbBits);
			e = gf_bifs_get_field_index(node, field_ref, GF_SG_FIELD_CODING_DEF, &fieldind);
			if (e) goto exit;
			e = gf_node_get_field(node, fieldind, &field);
			if (e) goto exit;
			inf = gf_sg_command_field_new(com);
			inf->fieldType = field.fieldType;
			inf->fieldIndex = field.fieldIndex;
			if (inf->fieldType==GF_SG_VRML_SFNODE) {
				field.far_ptr = inf->field_ptr = &inf->new_node;
			} else if (inf->fieldType==GF_SG_VRML_MFNODE) {
				field.far_ptr = inf->field_ptr = &inf->node_list;
			} else {
				field.far_ptr = inf->field_ptr = gf_sg_vrml_field_pointer_new(inf->fieldType);
			}
			e = gf_bifs_dec_field(codec, bs, node, &field, GF_TRUE);
			if (e) goto exit;
			flag = gf_bs_read_int(bs, 1);
		}
	}


exit:
	if (e) gf_sg_command_del(com);
	else gf_list_add(com_list, com);
	return e;
}