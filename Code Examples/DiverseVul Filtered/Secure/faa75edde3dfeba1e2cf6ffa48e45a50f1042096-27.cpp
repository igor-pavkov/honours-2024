static GF_Err lsr_read_delete(GF_LASeRCodec *lsr, GF_List *com_list)
{
	GF_FieldInfo info;
	s32 idx, att_type;
	u32 type, idref;

	att_type = lsr_get_attribute_name(lsr);

	idx = -2;
	GF_LSR_READ_INT(lsr, type, 1, "has_index");
	if (type) idx = (s32) lsr_read_vluimsbf5(lsr, "index");
	idref = lsr_read_codec_IDREF_command(lsr, "ref");

	lsr_read_any_attribute(lsr, NULL, 1);
	if (com_list) {
		GF_Command *com;
		com = gf_sg_command_new(lsr->sg, GF_SG_LSR_DELETE);
		gf_list_add(com_list, com);
		com->node = gf_sg_find_node(lsr->sg, idref);
		if (!com->node) {
			com->RouteID = idref;
			gf_list_add(lsr->unresolved_commands, com);
		} else {
			gf_node_register(com->node, NULL);
		}

		if ((idx>=0) || (att_type>=0)) {
			GF_CommandField *field = gf_sg_command_field_new(com);
			field->pos = idx;
			if (att_type>=0) {
				field->fieldIndex = gf_lsr_anim_type_to_attribute(att_type);
				if (!com->node) return GF_SG_UNKNOWN_NODE;
				gf_node_get_field(com->node, field->fieldIndex, &info);
				field->fieldType = info.fieldType;
			}
		}
	} else {
		SVG_Element *elt = (SVG_Element *) gf_sg_find_node(lsr->sg, idref);
		if (!elt)
			return GF_NON_COMPLIANT_BITSTREAM;

		if (att_type>=0) {
			s32 fieldIndex = gf_lsr_anim_type_to_attribute(att_type);
			gf_node_get_field((GF_Node*)elt, fieldIndex, &info);
			/*TODO implement*/
		}
		/*node deletion*/
		else if (idx>=0) {
			GF_Node *c = (GF_Node *)gf_node_list_get_child(elt->children, idx);
			if (c) {
				if (!gf_node_list_del_child( & elt->children, c))
					return GF_IO_ERR;
				gf_node_unregister(c, (GF_Node*)elt);
			}
		} else {
			gf_node_replace((GF_Node*)elt, NULL, 0);
		}
	}
	return GF_OK;
}