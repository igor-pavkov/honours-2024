static GF_Err BM_XReplace(GF_BifsDecoder *codec, GF_BitStream *bs, GF_List *com_list)
{
	GF_FieldInfo targetField, fromField, decfield;
	GF_Node *target, *fromNode;
	s32 pos = -2;
	u32 id, nbBits, ind, aind;
	GF_Err e;
	GF_Command *com;
	GF_CommandField *inf;

	id = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);
	target = gf_sg_find_node(codec->current_graph, id);
	if (!target) return GF_SG_UNKNOWN_NODE;

	com = gf_sg_command_new(codec->current_graph, GF_SG_XREPLACE);
	BM_SetCommandNode(com, target);
	gf_list_add(com_list, com);

	nbBits = gf_get_bit_size(gf_node_get_num_fields_in_mode(target, GF_SG_FIELD_CODING_IN)-1);
	ind = gf_bs_read_int(bs, nbBits);
	e = gf_bifs_get_field_index(target, ind, GF_SG_FIELD_CODING_IN, &aind);
	if (e) return e;
	e = gf_node_get_field(target, aind, &targetField);
	if (e) return e;

	inf = gf_sg_command_field_new(com);
	inf->fieldIndex = aind;

	if (!gf_sg_vrml_is_sf_field(targetField.fieldType)) {
		/*this is indexed replacement*/
		if (gf_bs_read_int(bs, 1)) {
			/*index is dynamic*/
			if (gf_bs_read_int(bs, 1)) {
				GF_Node *n;
				id = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);
				n = gf_sg_find_node(codec->current_graph, id);
				if (!n) return GF_SG_UNKNOWN_NODE;
				com->toNodeID = id;

				nbBits = gf_get_bit_size(gf_node_get_num_fields_in_mode(n, GF_SG_FIELD_CODING_DEF)-1);
				ind = gf_bs_read_int(bs, nbBits);
				e = gf_bifs_get_field_index(n, ind, GF_SG_FIELD_CODING_DEF, &aind);
				if (e) return e;
				e = gf_node_get_field(n, aind, &fromField);
				if (e) return e;
				com->toFieldIndex = aind;
			} else {
				u32 type = gf_bs_read_int(bs, 2);
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
				}
			}
		}
		if (targetField.fieldType==GF_SG_VRML_MFNODE) {
			if (gf_bs_read_int(bs, 1)) {
				target = gf_node_list_get_child(*(GF_ChildNodeItem **)targetField.far_ptr, pos);
				if (!target) return GF_SG_UNKNOWN_NODE;

				nbBits = gf_get_bit_size(gf_node_get_num_fields_in_mode(target, GF_SG_FIELD_CODING_IN)-1);
				ind = gf_bs_read_int(bs, nbBits);
				e = gf_bifs_get_field_index(target, ind, GF_SG_FIELD_CODING_IN, &aind);
				if (e) return e;
				e = gf_node_get_field(target, aind, &targetField);
				if (e) return e;
				pos = -2;
				com->child_field = aind;
				com->ChildNodeTag = gf_node_get_tag(target);
				if (com->ChildNodeTag == TAG_ProtoNode) {
					s32 p_id = gf_sg_proto_get_id(gf_node_get_proto(target));
					com->ChildNodeTag = -p_id;
				}
			}
		}
		inf->pos = pos;
	}

	fromNode = NULL;
	if (gf_bs_read_int(bs, 1)) {
		id = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);
		fromNode = gf_sg_find_node(codec->current_graph, id);
		if (!fromNode) return GF_SG_UNKNOWN_NODE;
		com->fromNodeID = id;

		nbBits = gf_get_bit_size(gf_node_get_num_fields_in_mode(fromNode, GF_SG_FIELD_CODING_DEF)-1);
		ind = gf_bs_read_int(bs, nbBits);
		e = gf_bifs_get_field_index(fromNode, ind, GF_SG_FIELD_CODING_DEF, &aind);
		if (e) return e;
		e = gf_node_get_field(fromNode, aind, &fromField);
		if (e) return e;
		com->fromFieldIndex = aind;

		return GF_OK;
	}


	if (pos>= -1) {
		inf->fieldType = gf_sg_vrml_get_sf_type(targetField.fieldType);
	} else {
		inf->fieldType = targetField.fieldType;
	}
	decfield.fieldIndex = inf->fieldIndex;
	decfield.fieldType = inf->fieldType;

	if (inf->fieldType==GF_SG_VRML_SFNODE) {
		decfield.far_ptr = inf->field_ptr = &inf->new_node;
	} else if (inf->fieldType==GF_SG_VRML_MFNODE) {
		decfield.far_ptr = inf->field_ptr = &inf->node_list;
	} else {
		decfield.far_ptr = inf->field_ptr = gf_sg_vrml_field_pointer_new(inf->fieldType);
	}
	e = gf_bifs_dec_sf_field(codec, bs, target, &decfield, GF_TRUE);
	return e;
}