static GF_Err lsr_read_add_replace_insert(GF_LASeRCodec *lsr, GF_List *com_list, u32 com_type)
{
	GF_FieldInfo info;
	GF_Node *n, *operandNode;
	GF_Command *com;
	GF_CommandField *field;
	s32 idx, att_type, op_att_type;
	u32 type, idref, op_idref = 0;

	operandNode = NULL;
	op_att_type = -1;

	att_type = lsr_get_attribute_name(lsr);

	idx = -1;
	if (com_type) {
		GF_LSR_READ_INT(lsr, type, 1, "has_index");
		if (type) idx = lsr_read_vluimsbf5(lsr, "index");
	}
	if (com_type!=3) {
		GF_LSR_READ_INT(lsr, type, 1, "has_operandAttribute");
		if (type) GF_LSR_READ_INT(lsr, op_att_type, 8, "attributeName");
		GF_LSR_READ_INT(lsr, type, 1, "has_operandElementId");
		if (type) {
			op_idref = lsr_read_codec_IDREF_command(lsr, "operandElementId");
			operandNode = gf_sg_find_node(lsr->sg, op_idref);
			if (!operandNode)
				return GF_NON_COMPLIANT_BITSTREAM;
		}
	}
	idref = lsr_read_codec_IDREF_command(lsr, "ref");

	n = gf_sg_find_node(lsr->sg, idref);
	if (!n) {
		if (!com_list) {
			return GF_NON_COMPLIANT_BITSTREAM;
		}
	}

	GF_LSR_READ_INT(lsr, type, 1, "has_value");
	if (type) {
		/*node or node-list replacement*/
		if (att_type==-2) {
			lsr_read_byte_align_string(lsr, NULL, "anyXML");
		}
		else if (att_type<0) {
			GF_Node *new_node;
			if (!com_type)
				return GF_NON_COMPLIANT_BITSTREAM;
			GF_LSR_READ_INT(lsr, type, 1, "isInherit");
			if (type)
				return GF_NON_COMPLIANT_BITSTREAM;
			if (idx==-1) {
				GF_LSR_READ_INT(lsr, type, 1, "escapeFlag");
				if (type)
					return GF_NON_COMPLIANT_BITSTREAM;
			}

			new_node = lsr_read_update_content_model(lsr, (idx==-1) ? NULL : (SVG_Element *)n);
			if (com_list) {
				com = gf_sg_command_new(lsr->sg, (com_type==3) ? GF_SG_LSR_INSERT : GF_SG_LSR_REPLACE);
				gf_list_add(com_list, com);
				if (n) {
					com->node = n;
					gf_node_register(com->node, NULL);
				} else {
					com->RouteID = idref;
					gf_list_add(lsr->unresolved_commands, com);
				}
				field = gf_sg_command_field_new(com);
				field->pos = idx;
				field->new_node = new_node;
				if (new_node) gf_node_register(new_node, NULL);
			} else if (com_type==3) {
				gf_node_list_insert_child(& ((SVG_Element *)n)->children, new_node, idx);
				if (new_node) gf_node_register(new_node, n);
			} else {
				/*child replacement*/
				if (idx!=-1) {
					GF_Node *old = gf_node_list_get_child( ((SVG_Element *)n)->children, idx);
					if (old)
						gf_node_replace(old, new_node, 0);
					else {
						gf_node_list_add_child( & ((SVG_Element *)n)->children, new_node);
						if (new_node) gf_node_register(new_node, n);
					}
				} else {
					/*node replacement*/
					gf_node_replace(n, new_node, 0);
				}
			}
		}
		/*value replace/add*/
		else if (com_list) {
			u32 field_type;
			Bool text_content = 0;
			com = gf_sg_command_new(lsr->sg, (com_type==0) ? GF_SG_LSR_ADD : (com_type==3) ? GF_SG_LSR_INSERT : GF_SG_LSR_REPLACE);
			field = gf_sg_command_field_new(com);
			field->pos = idx;
			field_type = 0;
			switch (att_type) {
			/*text*/
			case LSR_UPDATE_TYPE_TEXT_CONTENT:
				text_content = 1;
				break;
			/*matrix.translation, scale or rotate*/
			case LSR_UPDATE_TYPE_SCALE:
				field->fieldType = field_type = SVG_Transform_Scale_datatype;
				field->fieldIndex = gf_lsr_anim_type_to_attribute(att_type);
				break;
			case LSR_UPDATE_TYPE_ROTATE:
				field->fieldType = field_type = SVG_Transform_Rotate_datatype;
				field->fieldIndex = TAG_SVG_ATT_transform;
				break;
			case LSR_UPDATE_TYPE_TRANSLATION:
				field->fieldType = field_type = SVG_Transform_Translate_datatype;
				field->fieldIndex = gf_lsr_anim_type_to_attribute(att_type);
				break;
			case LSR_UPDATE_TYPE_SVG_HEIGHT:
				field->fieldIndex = TAG_SVG_ATT_height;
				field_type = field->fieldType = SVG_Length_datatype;
				break;
			case LSR_UPDATE_TYPE_SVG_WIDTH:
				field->fieldIndex = TAG_SVG_ATT_width;
				field_type = field->fieldType = SVG_Length_datatype;
				break;
			default:
				field->fieldIndex = gf_lsr_anim_type_to_attribute(att_type);
				if (field->fieldIndex == (u32)-1) {
					lsr->last_error = GF_NON_COMPLIANT_BITSTREAM;
					gf_sg_command_del(com);
					return lsr->last_error;
				}
				field_type = field->fieldType = gf_xml_get_attribute_type(field->fieldIndex);
				break;
			}
			gf_list_add(com_list, com);
			if (n) {
				com->node = n;
				gf_node_register(com->node, NULL);
			} else {
				com->RouteID = idref;
				gf_list_add(lsr->unresolved_commands, com);
			}
			if (idx==-1) {
				if (text_content) {
					GF_DOMText *text = gf_dom_new_text_node(lsr->sg);
					gf_node_register((GF_Node *)text, NULL);
					lsr_read_byte_align_string(lsr, &text->textContent, "val");
					field->new_node = (GF_Node*)text;
				} else {
					field->field_ptr = gf_svg_create_attribute_value(field_type);
					lsr_read_update_value(lsr, NULL, field->fieldIndex, field->fieldType, field->field_ptr, n ? n->sgprivate->tag : 0);
				}
			} else {
				field->field_ptr = lsr_read_update_value_indexed(lsr, (GF_Node*)n, field_type, NULL, idx, com_type==LSR_UPDATE_INSERT, 1, &field->fieldType);
			}
		} else {
			GF_Point2D matrix_tmp;
			SVG_Point_Angle matrix_tmp_rot;
			u32 fieldIndex = 0;
			u32 field_type = 0;
			Bool text_content = 0;
			Bool is_lsr_transform = 0;
			switch (att_type) {
			/*text*/
			case LSR_UPDATE_TYPE_TEXT_CONTENT:
				text_content = 1;
				break;
			/*matrix.translation, scale or rotate*/
			case LSR_UPDATE_TYPE_SCALE:
				info.far_ptr = (void *)&matrix_tmp;
				field_type = SVG_Transform_Scale_datatype;
				is_lsr_transform = 1;
				break;
			case LSR_UPDATE_TYPE_ROTATE:
				info.far_ptr = (void *)&matrix_tmp_rot;
				field_type = SVG_Transform_Rotate_datatype;
				is_lsr_transform = 1;
				break;
			case LSR_UPDATE_TYPE_TRANSLATION:
				info.far_ptr = (void *)&matrix_tmp;
				field_type = SVG_Transform_Translate_datatype;
				is_lsr_transform = 1;
				break;
			default:
				fieldIndex = gf_lsr_anim_type_to_attribute(att_type);
				gf_node_get_attribute_by_tag(n, fieldIndex, 1, 0, &info);
				field_type = info.fieldType;
				break;
			}
			info.fieldType = field_type;
			if (is_lsr_transform) {
				SVG_Transform *dest;
				if (idx==-1) {
					lsr_read_update_value(lsr, NULL, fieldIndex, field_type, info.far_ptr, 0);
				} else {
					assert(0);
				}


//				fieldIndex = gf_node_get_attribute_by_tag((GF_Node*)n, TAG_SVG_ATT_transform, 1, 1, &info);
				dest = (SVG_Transform *)info.far_ptr;
				if (com_type) {
					GF_Point2D scale, translate;
					SVG_Point_Angle rotate;
					if (gf_mx2d_decompose(&dest->mat, &scale, &rotate.angle, &translate)) {
						gf_mx2d_init(dest->mat);
						if (att_type==LSR_UPDATE_TYPE_SCALE) scale = matrix_tmp;
						else if (att_type==LSR_UPDATE_TYPE_TRANSLATION) translate = matrix_tmp;
						else if (att_type==LSR_UPDATE_TYPE_ROTATE) rotate = matrix_tmp_rot;

						gf_mx2d_add_scale(&dest->mat, scale.x, scale.y);
						gf_mx2d_add_rotation(&dest->mat, 0, 0, rotate.angle);
						gf_mx2d_add_translation(&dest->mat, translate.x, translate.y);
					}
				}
				else if (att_type==LSR_UPDATE_TYPE_SCALE) gf_mx2d_add_scale(&dest->mat, matrix_tmp.x, matrix_tmp.y);
				else if (att_type==LSR_UPDATE_TYPE_TRANSLATION) gf_mx2d_add_translation(&dest->mat, matrix_tmp.x, matrix_tmp.y);
				else if (att_type==LSR_UPDATE_TYPE_ROTATE) gf_mx2d_add_rotation(&dest->mat, 0, 0, matrix_tmp_rot.angle);

				gf_node_changed((GF_Node*)n, &info);
			}
			else if (com_type) {
				if (text_content) {
					GF_DOMText *t = NULL;
					if (idx>=0) {
						if (com_type==LSR_UPDATE_INSERT) {
							t = gf_dom_new_text_node(n->sgprivate->scenegraph);
							gf_node_register((GF_Node *)t, n);
							gf_node_list_insert_child(&((GF_ParentNode *)n)->children, (GF_Node*)t, idx);
						} else {
							t = (GF_DOMText *) gf_node_list_get_child(((SVG_Element*)n)->children, idx);
							if (t->sgprivate->tag!=TAG_DOMText) t = NULL;
						}
					} else {
						/*this is a replace, reset ALL node content*/
						gf_sg_parent_reset(n);
						t = gf_dom_add_text_node(n, NULL);
					}
					lsr_read_byte_align_string(lsr, t ? &t->textContent : NULL, "textContent");
					gf_node_changed(n, NULL);
				} else if (idx==-1) {
					lsr_read_update_value(lsr, (GF_Node*)n, fieldIndex, field_type, info.far_ptr, n->sgprivate->tag);
				} else {
					Fixed *v1, *v2;
					//SMIL_Time *t;
					void *prev, *new_item;
					void *tmp = lsr_read_update_value_indexed(lsr, (GF_Node*)n, field_type, info.far_ptr, idx, com_type==LSR_UPDATE_INSERT, 0, NULL);
					switch (field_type) {
					/*generic GF_List containers, no type translation needed*/
					case SMIL_KeyTimes_datatype/*ITYPE_keyTime*/:
					case SMIL_KeySplines_datatype/*ITYPE_float*/:
					case SVG_Points_datatype/*ITYPE_point*/:
					case SMIL_Times_datatype/*ITYPE_smil_time*/:
						new_item = gf_list_pop_front(*(GF_List **)tmp);
						if (com_type==LSR_UPDATE_INSERT) {
							gf_list_insert(*(SVG_Coordinates*)info.far_ptr, new_item, idx);
						} else {
							prev = gf_list_get(*(SVG_Coordinates*)info.far_ptr, idx);
							gf_free(prev);
							gf_list_rem(*(SVG_Coordinates*)info.far_ptr, idx);
							gf_list_insert(*(SVG_Coordinates*)info.far_ptr, new_item, idx);
						}
						gf_node_changed((GF_Node*)n, NULL);
						gf_list_del(*(GF_List **)tmp);
						gf_free(tmp);
						break;
					/*list of floats - to check when implementing it...*/
					case SMIL_KeyPoints_datatype/*ITYPE_0to1 - keyPoints*/:
						v1 = (Fixed *) gf_list_pop_front(*(GF_List **)tmp);
						v2 = (Fixed *) gf_list_pop_front(*(GF_List **)tmp);
						gf_list_del(*(GF_List **)tmp);
						gf_free(tmp);

						if (com_type==LSR_UPDATE_INSERT) {
							gf_list_insert(*(SVG_Coordinates*)info.far_ptr, v1, idx);
							gf_list_insert(*(SVG_Coordinates*)info.far_ptr, v2, idx+1);
						} else {
							prev = gf_list_get(*(SVG_Coordinates*)info.far_ptr, idx);
							gf_free(prev);
							gf_list_rem(*(SVG_Coordinates*)info.far_ptr, idx);
							prev = gf_list_get(*(SVG_Coordinates*)info.far_ptr, idx);
							gf_free(prev);
							gf_list_rem(*(SVG_Coordinates*)info.far_ptr, idx);
							gf_list_insert(*(SVG_Coordinates*)info.far_ptr, v1, idx);
							gf_list_insert(*(SVG_Coordinates*)info.far_ptr, v2, idx);
						}
						gf_node_changed((GF_Node*)n, NULL);
						break;
					case SVG_ViewBox_datatype:
						v1 = (Fixed*)tmp;
						switch (idx) {
						case 0:
							((SVG_ViewBox*)info.far_ptr)->x = *v1;
							break;
						case 1:
							((SVG_ViewBox*)info.far_ptr)->y = *v1;
							break;
						case 2:
							((SVG_ViewBox*)info.far_ptr)->width = *v1;
							break;
						case 3:
							((SVG_ViewBox*)info.far_ptr)->height = *v1;
							break;
						}
						gf_free(tmp);
						gf_node_changed((GF_Node*)n, NULL);
						break;
					case SVG_StrokeDashArray_datatype:
						v1 = (Fixed*)tmp;
						if (com_type==LSR_UPDATE_INSERT) {
							SVG_StrokeDashArray*da = (SVG_StrokeDashArray*)info.far_ptr;
							/*use MFFloat for insert*/
							if (gf_sg_vrml_mf_insert(&da->array, GF_SG_VRML_MFFLOAT, (void*) &v2, idx)==GF_OK)
								*v2 = *v1;
						} else {
							SVG_StrokeDashArray*da = (SVG_StrokeDashArray*)info.far_ptr;
							if (idx<(s32)da->array.count) da->array.vals[idx] = *v1;
						}
						gf_free(tmp);
						gf_node_changed((GF_Node*)n, NULL);
						break;
					default:
						gf_free(tmp);
						break;
					}
				}
			} else {
				GF_FieldInfo tmp;
				tmp = info;
				if (idx==-1) {
					tmp.far_ptr = gf_svg_create_attribute_value(info.fieldType);
					lsr_read_update_value(lsr, n, fieldIndex, field_type, tmp.far_ptr, n->sgprivate->tag);
				} else {
					tmp.far_ptr = lsr_read_update_value_indexed(lsr, n, field_type, NULL, idx, 0, 1, NULL);
				}
				gf_svg_attributes_add(&info, &tmp, &info, 0);
				gf_svg_delete_attribute_value(info.fieldType, tmp.far_ptr, gf_node_get_graph(n));
			}
		}
	}
	/*copy from node*/
	else if (operandNode && (op_att_type>=0)) {
		u32 opFieldIndex = gf_lsr_anim_type_to_attribute(op_att_type);
		if (com_list) {
			com = gf_sg_command_new(lsr->sg, com_type ? GF_SG_LSR_REPLACE : GF_SG_LSR_ADD);
			gf_list_add(com_list, com);
			com->node = n;
			gf_node_register(com->node, NULL);
			com->fromNodeID = op_idref;
			com->fromFieldIndex = opFieldIndex;
			field = gf_sg_command_field_new(com);
			field->pos = idx;
			field->fieldIndex = gf_lsr_anim_type_to_attribute(att_type);
		} else {
			u32 fieldIndex;
			GF_FieldInfo op_info;
			fieldIndex = gf_lsr_anim_type_to_attribute(att_type);
			gf_node_get_field(n, fieldIndex, &info);
			gf_node_get_field(operandNode, opFieldIndex, &op_info);
			if (com_type) {
				gf_svg_attributes_copy(&info, &op_info, 0);
			} else {
				gf_svg_attributes_add(&info, &op_info, &info, 0);
			}
		}
	}

	lsr_read_any_attribute(lsr, NULL, 1);

	/*if not add*/
	if (!com_type) return GF_OK;

	/*list replacement*/
	GF_LSR_READ_INT(lsr, type, 1, "opt_group");
	if (type) {

		if (com_list) {
			u32 count;
			GF_ChildNodeItem *last = NULL;

			if (com_type==LSR_UPDATE_INSERT) count = 1;
			else count = lsr_read_vluimsbf5(lsr, "count");

			com = gf_sg_command_new(lsr->sg, (com_type==LSR_UPDATE_REPLACE) ? GF_SG_LSR_REPLACE : GF_SG_LSR_INSERT);
			gf_list_add(com_list, com);
			com->node = n;
			gf_node_register(com->node, NULL);
			field = gf_sg_command_field_new(com);
			field->pos = idx;

			if (!count && (att_type==LSR_UPDATE_TYPE_TEXT_CONTENT)) {
				field->fieldIndex = -1;
			} else if (count==1) {
				field->new_node = lsr_read_update_content_model(lsr, (SVG_Element *) n);
				gf_node_register(field->new_node, NULL);
				if (att_type>=0) field->fieldIndex = gf_lsr_anim_type_to_attribute(att_type);
			} else {
				field->fieldType = SVG_NodeList_datatype;
				field->field_ptr = &field->node_list;
				while (count) {
					GF_Node *new_node = lsr_read_update_content_model(lsr, (SVG_Element *) n);
					gf_node_register(new_node, NULL);
					gf_node_list_add_child_last(& field->node_list, new_node, &last);
					count--;
					if (lsr->last_error) return lsr->last_error;
				}
			}
		} else {
			SVG_Element*elt = (SVG_Element*)n;
			GF_ChildNodeItem *last = NULL;
			u32 count;
			if (com_type==LSR_UPDATE_INSERT) count = 1;
			else count = lsr_read_vluimsbf5(lsr, "count");

			if (com_type==LSR_UPDATE_REPLACE) {
				if (idx>=0) {
					GF_Node *new_node = gf_node_list_del_child_idx(&elt->children, idx);
					if (new_node) gf_node_unregister(new_node, n);
				} else {
					gf_node_unregister_children(n, elt->children);
					elt->children = NULL;
				}
			}
			if ((com_type==LSR_UPDATE_INSERT) || (gf_lsr_anim_type_to_attribute(att_type) == TAG_LSR_ATT_children)) {
				while (count) {
					GF_Node *new_node = lsr_read_update_content_model(lsr, elt);
					if (new_node) {
						if (idx>=0) {
							gf_node_list_insert_child(&elt->children, new_node, idx);
						} else {
							gf_node_list_add_child_last(&elt->children, new_node, &last);
						}
						gf_node_register(new_node, n);
					}
					count--;
					if (lsr->last_error) return lsr->last_error;
				}
				gf_node_changed(n, NULL);
			}
			/*node replacement*/
			else if ((att_type==-1) && (count==1)) {
				GF_Node *new_node = lsr_read_update_content_model(lsr, elt);
				gf_node_replace((GF_Node*)elt, new_node, 0);
			}
		}
	}
	return GF_OK;
}