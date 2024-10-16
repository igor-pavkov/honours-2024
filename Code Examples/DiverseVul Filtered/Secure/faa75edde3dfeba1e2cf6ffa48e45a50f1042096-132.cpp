static void *lsr_read_an_anim_value(GF_LASeRCodec *lsr, u32 coded_type, const char *name)
{
	u32 flag;
	u32 escapeFlag, escape_val = 0;
	u8 *enum_val;
	u32 *id_val;
	char *string;
	SVG_String *svg_string;
	SVG_Number *num;
	XMLRI *iri;
	SVG_Point *pt;
	SVG_Paint *paint;

	GF_LSR_READ_INT(lsr, escapeFlag, 1, "escapeFlag");
	if (escapeFlag) GF_LSR_READ_INT(lsr, escape_val, 2, "escapeEnum");

	switch (coded_type) {
	case 0:
		string = NULL;
		lsr_read_byte_align_string(lsr, &string, name);
		GF_SAFEALLOC(svg_string, SVG_String);
		if (!svg_string) {
			lsr->last_error = GF_OUT_OF_MEM;
			return NULL;
		}
		*svg_string = string;
		return svg_string;
	case 1:
		num = (SVG_Number*)gf_malloc(sizeof(SVG_Number));
		if (!num) {
			lsr->last_error = GF_OUT_OF_MEM;
			return NULL;
		}
		if (escapeFlag) {
			num->type = (escape_val==1) ? SVG_NUMBER_INHERIT : SVG_NUMBER_VALUE;
		} else {
			num->type = SVG_NUMBER_VALUE;
			num->value = lsr_read_fixed_16_8(lsr, name);
		}
		return num;
	case 2:
	{
		SVG_PathData *pd = (SVG_PathData *)gf_svg_create_attribute_value(SVG_PathData_datatype);
		if (!pd) {
			lsr->last_error = GF_OUT_OF_MEM;
		} else {
			lsr_read_path_type(lsr, NULL, 0, pd, name);
		}
		return pd;
	}
	case 3:
	{
		SVG_Points *pts = (SVG_Points *)gf_svg_create_attribute_value(SVG_Points_datatype);
		if (!pts) {
			lsr->last_error = GF_OUT_OF_MEM;
		} else {
			lsr_read_point_sequence(lsr, *pts, name);
		}
		return pts;
	}
	case 4:
		num = (SVG_Number*)gf_malloc(sizeof(SVG_Number));
		if (!num) {
			lsr->last_error = GF_OUT_OF_MEM;
			return NULL;
		}
		if (escapeFlag) {
			num->type = (escape_val==1) ? SVG_NUMBER_INHERIT : SVG_NUMBER_VALUE;
		} else {
			num->type = SVG_NUMBER_VALUE;
			num->value = lsr_read_fixed_clamp(lsr, name);
		}
		return num;
	case 5:
		GF_SAFEALLOC(paint, SVG_Paint);
		if (!paint) {
			lsr->last_error = GF_OUT_OF_MEM;
			return NULL;
		}
		if (escapeFlag) {
			paint->type = SVG_PAINT_INHERIT;
		} else {
			lsr_read_paint(lsr, paint, name);
		}
		return paint;
	case 6:
		enum_val = (u8*)gf_malloc(sizeof(u8));
		if (!enum_val) {
			lsr->last_error = GF_OUT_OF_MEM;
		} else {
			*enum_val = lsr_read_vluimsbf5(lsr, name);
		}
		return enum_val;
	/*TODO check this is correct*/
	case 7:
	{
		GF_List *l = gf_list_new();
		if (!l) {
			lsr->last_error = GF_OUT_OF_MEM;
			return NULL;
		}
		u32 i, count;
		count = lsr_read_vluimsbf5(lsr, "count");
		for (i=0; i<count; i++) {
			u8 *v = (u8 *)gf_malloc(sizeof(u8));
			if (!v) {
				lsr->last_error = GF_OUT_OF_MEM;
			} else {
				*v = lsr_read_vluimsbf5(lsr, "val");
				gf_list_add(l, v);
			}
			if (lsr->last_error) break;
		}
		return l;
	}
	/*TODO check this is correct*/
	case 8: // floats
	{
		GF_List *l = gf_list_new();
		if (!l) {
			lsr->last_error = GF_OUT_OF_MEM;
			return NULL;
		}
		u32 i, count;
		count = lsr_read_vluimsbf5(lsr, "count");
		for (i=0; i<count; i++) {
			Fixed *v = (Fixed *)gf_malloc(sizeof(Fixed));
			if (!v) {
				lsr->last_error = GF_OUT_OF_MEM;
			} else {
				*v = lsr_read_fixed_16_8(lsr, "val");
				gf_list_add(l, v);
			}
			if (lsr->last_error) break;
		}
		return l;
	}

	/*point */
	case 9:
		pt = (SVG_Point*)gf_malloc(sizeof(SVG_Point));
		if (!pt) {
			lsr->last_error = GF_OUT_OF_MEM;
			return NULL;
		}
		GF_LSR_READ_INT(lsr, flag, lsr->coord_bits, "valX");
		pt->x = lsr_translate_coords(lsr, flag, lsr->coord_bits);
		GF_LSR_READ_INT(lsr, flag, lsr->coord_bits, "valY");
		pt->y = lsr_translate_coords(lsr, flag, lsr->coord_bits);
		return pt;
	case 10:
		id_val = (u32*)gf_malloc(sizeof(u32));
		if (!id_val) {
			lsr->last_error = GF_OUT_OF_MEM;
		} else {
			*id_val = lsr_read_vluimsbf5(lsr, name);
		}
		return id_val;
	case 11:
	{
		SVG_FontFamily *ft;
		u32 idx;
		GF_SAFEALLOC(ft, SVG_FontFamily);
		if (!ft) {
			lsr->last_error = GF_OUT_OF_MEM;
			return NULL;
		}
		if (escapeFlag) {
			ft->type = SVG_FONTFAMILY_INHERIT;
		} else {
			idx = lsr_read_vluimsbf5(lsr, name);
			ft->type = SVG_FONTFAMILY_VALUE;
			ft->value = (char*)gf_list_get(lsr->font_table, idx);
			if (ft->value) ft->value = gf_strdup(ft->value);
		}
		return ft;
	}
	case 12:
		GF_SAFEALLOC(iri, XMLRI);
		if (!iri) {
			lsr->last_error = GF_OUT_OF_MEM;
		} else {
			lsr_read_any_uri(lsr, iri, name);
		}
		return iri;
	default:
		lsr_read_extension(lsr, name);
		break;
	}
	return NULL;
}