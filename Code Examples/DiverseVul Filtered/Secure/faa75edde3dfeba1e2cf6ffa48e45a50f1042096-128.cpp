static void lsr_read_update_value(GF_LASeRCodec *lsr, GF_Node *node, u32 att_tag, u32 fieldType, void *val, u32 node_tag)
{
	u32 is_default, has_escape, escape_val = 0;
	SVG_Number num, *n;

	switch (fieldType) {
	case SVG_Boolean_datatype:
		GF_LSR_READ_INT(lsr, *(SVG_Boolean*)val, 1, "val");
		break;
	case SVG_Paint_datatype:
		lsr_read_paint(lsr, (SVG_Paint*)val, "val");
		break;
	/*
		case SVG_AudioLevel_datatype:
			n = val;
			GF_LSR_READ_INT(lsr, is_default, 1, "isDefaultValue");
			if (is_default) n->type=SVG_NUMBER_INHERIT;
			else {
				n->type = SVG_NUMBER_VALUE;
				n->value = lsr_read_fixed_clamp(lsr, "val");
			}
			break;
	*/
	case SVG_Transform_Scale_datatype:
		((SVG_Point *)val)->x = lsr_read_fixed_16_8(lsr, "scale_x");
		((SVG_Point *)val)->y = lsr_read_fixed_16_8(lsr, "scale_y");
		break;
	case LASeR_Size_datatype:
	case SVG_Transform_Translate_datatype:
		lsr_read_coordinate(lsr, &num, 0, "translation_x");
		((SVG_Point *)val)->x = num.value;
		lsr_read_coordinate(lsr, &num, 0, "translation_y");
		((SVG_Point *)val)->y = num.value;
		break;
	case SVG_Transform_Rotate_datatype:
		GF_LSR_READ_INT(lsr, is_default, 1, "isDefaultValue");
		if (is_default) ((SVG_Point_Angle*)val)->angle = 0;
		else {
			GF_LSR_READ_INT(lsr, has_escape, 1, "escapeFlag");
			if (has_escape) {
				GF_LSR_READ_INT(lsr, escape_val, 2, "escapeEnum");
				((SVG_Point_Angle*)val)->angle = 0;
			}
			else {
				((SVG_Point_Angle*)val)->angle = lsr_read_fixed_16_8(lsr, "rotate");
			}
		}
		break;
	case SVG_Transform_datatype:
		lsr_read_matrix(lsr, val);
		break;
	case SVG_Number_datatype:
	case SVG_FontSize_datatype:
	case SVG_Length_datatype:
		n = (SVG_Number*)val;
		switch (att_tag) {
		/*fractions*/
		case TAG_SVG_ATT_audio_level:
		case TAG_SVG_ATT_fill_opacity:
		case TAG_SVG_ATT_offset:
		case TAG_SVG_ATT_opacity:
		case TAG_SVG_ATT_solid_opacity:
		case TAG_SVG_ATT_stop_opacity:
		case TAG_SVG_ATT_stroke_opacity:
		case TAG_SVG_ATT_viewport_fill_opacity:
			GF_LSR_READ_INT(lsr, is_default, 1, "isDefaultValue");
			if (is_default) n->type=SVG_NUMBER_INHERIT;
			else {
				n->type = SVG_NUMBER_VALUE;
				n->value = lsr_read_fixed_clamp(lsr, "val");
			}
			break;
		case TAG_SVG_ATT_width:
		case TAG_SVG_ATT_height:
			if (node_tag==TAG_SVG_svg) {
				lsr_read_value_with_units(lsr, n, "val");
			} else {
				lsr_read_coordinate(lsr, n, 0, "val");
			}
			break;
		default:
			GF_LSR_READ_INT(lsr, is_default, 1, "isDefaultValue");
			if (is_default) n->type=SVG_NUMBER_INHERIT;
			else {
				GF_LSR_READ_INT(lsr, has_escape, 1, "escapeFlag");
				if (has_escape) {
					GF_LSR_READ_INT(lsr, escape_val, 2, "escapeEnum");
					n->type = SVG_NUMBER_AUTO;//only lineIncrement
				} else {
					n->type = SVG_NUMBER_VALUE;
					n->value = lsr_read_fixed_16_8(lsr, "val");
				}
			}
			break;
		}
		break;
	case SVG_Coordinate_datatype:
		n = (SVG_Number*)val;
		n->type = SVG_NUMBER_VALUE;
		lsr_read_coordinate(lsr, n, 0, "val");
		break;

	case SVG_Rotate_datatype:
		n = (SVG_Number*)val;
		GF_LSR_READ_INT(lsr, is_default, 1, "isDefaultValue");
		if (is_default) n->type=SVG_NUMBER_INHERIT;
		else {
			GF_LSR_READ_INT(lsr, has_escape, 1, "escapeFlag");
			if (has_escape) {
				GF_LSR_READ_INT(lsr, escape_val, 2, "escapeEnum");
				n->type = escape_val ? SVG_NUMBER_AUTO_REVERSE : SVG_NUMBER_AUTO;
			} else {
				n->type = SVG_NUMBER_VALUE;
				n->value = lsr_read_fixed_16_8(lsr, "rotate");
			}
		}
		break;
	case SVG_Coordinates_datatype:
		lsr_read_float_list(lsr, NULL, 0, val, "val");
		break;
	case SVG_ViewBox_datatype:
	{
		u32 count;
		SVG_ViewBox *vb = (SVG_ViewBox *)val;
		GF_LSR_READ_INT(lsr, count, 1, "isDefault");
		if (count) {
			vb->is_set = 0;
		} else {
			vb->is_set = 1;
			GF_LSR_READ_INT(lsr, count, 1, "escapeFlag");
			count = lsr_read_vluimsbf5(lsr, "count");
			if (count) {
				vb->x = lsr_read_fixed_16_8(lsr, "val");
				count--;
			}
			if (count) {
				vb->y = lsr_read_fixed_16_8(lsr, "val");
				count--;
			}
			if (count) {
				vb->width = lsr_read_fixed_16_8(lsr, "val");
				count--;
			}
			if (count) {
				vb->height = lsr_read_fixed_16_8(lsr, "val");
			}
		}
	}
	break;
	case XMLRI_datatype:
	case SVG_Focus_datatype:
		if ((att_tag==TAG_XLINK_ATT_href) || (att_tag==TAG_SVG_ATT_syncReference)) {
			lsr_read_any_uri(lsr, (XMLRI*)val, "val");
		} else {
			Bool is_escape;
			u32 ID;
			escape_val = ID = 0;
			is_escape = 0;
			GF_LSR_READ_INT(lsr, is_default, 1, "isDefault");
			if (!is_default) {
				GF_LSR_READ_INT(lsr, is_escape, 1, "isEscape");
				if (is_escape) {
					GF_LSR_READ_INT(lsr, escape_val, 2, "escapeEnumVal");
				} else {
					ID = lsr_read_vluimsbf5(lsr, "ID");
				}
			}
			if (att_tag==SVG_Focus_datatype) {
				if (is_default) ((SVG_Focus*)val)->type = SVG_FOCUS_AUTO;
				else if (is_escape) ((SVG_Focus*)val)->type = escape_val;
				else {
					((SVG_Focus*)val)->type = SVG_FOCUS_IRI;
					((SVG_Focus*)val)->target.type = XMLRI_ELEMENTID;
					((SVG_Focus*)val)->target.node_id = ID;
				}
			} else {
				if (is_default) ((XMLRI*)val)->type = XMLRI_STRING;
				else {
					((XMLRI *)val)->type = XMLRI_ELEMENTID;
					((XMLRI *)val)->node_id = ID;
				}
			}
		}
		break;

	case DOM_String_datatype:
	case SVG_ContentType_datatype:
	case SVG_LanguageID_datatype:
		lsr_read_byte_align_string(lsr, (char**)val, "val");
		break;
	case SVG_Motion_datatype:
		lsr_read_coordinate(lsr, &num, 0, "pointValueX");
		((GF_Matrix2D*)val)->m[2] = num.value;
		lsr_read_coordinate(lsr, &num, 0, "pointValueY");
		((GF_Matrix2D*)val)->m[5] = num.value;
		break;
	case SVG_Points_datatype:
		lsr_read_point_sequence(lsr, *(GF_List **)val, "val");
		break;
	case SVG_PathData_datatype:
		lsr_read_path_type(lsr, NULL, 0, (SVG_PathData*)val, "val");
		break;
	case SVG_FontFamily_datatype:
	{
		u32 idx;
		SVG_FontFamily *ff = (SVG_FontFamily *)val;
		GF_LSR_READ_INT(lsr, idx, 1, "isDefault");
		ff->type = SVG_FONTFAMILY_INHERIT;
		if (!idx) {
			char *ft;
			GF_LSR_READ_INT(lsr, idx, 1, "escapeFlag");
			idx = lsr_read_vluimsbf5(lsr, "index");
			if (ff->value) gf_free(ff->value);
			ff->value = NULL;
			ft = (char*)gf_list_get(lsr->font_table, idx);
			if (ft) {
				ff->value = gf_strdup(ft);
				ff->type = SVG_FONTFAMILY_VALUE;
			}
		}
	}
		break;
	case LASeR_Choice_datatype:
		GF_LSR_READ_INT(lsr, is_default, 1, "isDefaultValue");
		if (is_default) ((LASeR_Choice *)val)->type = LASeR_CHOICE_ALL;
		else {
			GF_LSR_READ_INT(lsr, has_escape, 1, "escapeFlag");
			if (has_escape) {
				GF_LSR_READ_INT(lsr, escape_val, 2, "escapeEnum");
				((LASeR_Choice *)val)->type = escape_val ? LASeR_CHOICE_NONE : LASeR_CHOICE_ALL;
			} else {
				((LASeR_Choice *)val)->type = LASeR_CHOICE_N;
				((LASeR_Choice *)val)->choice_index = lsr_read_vluimsbf5(lsr, "value");
			}
		}
		break;
	default:
		if ((fieldType>=SVG_FillRule_datatype) && (fieldType<=SVG_LAST_U8_PROPERTY)) {
			/*TODO fixme, check inherit values*/
			GF_LSR_READ_INT(lsr, is_default, 1, "isDefaultValue");
			if (is_default) *(u8 *)val = 0;
			else *(u8 *)val = lsr_read_vluimsbf5(lsr, "val");
		} else {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[LASeR] Warning: update value not supported: fieldType %d - attribute tag %d\n", fieldType, att_tag));
		}
	}
	if (node) {
		//gf_node_dirty_set(node, 0, 0);
		gf_node_changed(node, NULL);
	}
}