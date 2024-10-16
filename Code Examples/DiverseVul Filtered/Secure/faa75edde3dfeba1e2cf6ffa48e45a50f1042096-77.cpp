static void *lsr_read_update_value_indexed(GF_LASeRCodec *lsr, GF_Node*node, u32 fieldType, void *rep_val, u32 idx, Bool is_insert, Bool is_com, u32 *single_field_type)
{
	Fixed *f_val;
	SVG_Number num;

	switch (fieldType) {
	case SVG_Points_datatype/*ITYPE_point*/:
	{
		SVG_Point *pt;
		ListOfXXX *res;
		GF_SAFEALLOC(res, ListOfXXX);
		if (!res) {
			lsr->last_error = GF_OUT_OF_MEM;
			return NULL;
		}
		*res = gf_list_new();
		pt = (SVG_Point*)gf_malloc(sizeof(SVG_Point));
		if (!pt) {
			lsr->last_error = GF_OUT_OF_MEM;
		} else {
			lsr_read_coordinate(lsr, &num, 0, "coordX");
			pt->x = num.value;
			lsr_read_coordinate(lsr, &num, 0, "coordY");
			pt->y = num.value;
			gf_list_add(*res, pt);
		}
		return res;
	}
	case SMIL_KeySplines_datatype/*ITYPE_float*/:
	{
		ListOfXXX *res;
		GF_SAFEALLOC(res, ListOfXXX);
		if (!res) {
			lsr->last_error = GF_OUT_OF_MEM;
			return NULL;
		}
		*res = gf_list_new();
		f_val = (Fixed*)gf_malloc(sizeof(Fixed));
		if (!f_val) {
			lsr->last_error = GF_OUT_OF_MEM;
		} else {
			*f_val = lsr_read_fixed_16_8(lsr, "floatValue");
			gf_list_add(*res, f_val);
		}
		return res;
	}
	case SVG_StrokeDashArray_datatype:
	case SVG_ViewBox_datatype:
		f_val = (Fixed*)gf_malloc(sizeof(Fixed));
		if (!f_val) {
			lsr->last_error = GF_OUT_OF_MEM;
		} else {
			*f_val = lsr_read_fixed_16_8(lsr, "floatValue");
		}
		return f_val;
	case SMIL_KeyTimes_datatype/*ITYPE_keyTime*/:
	{
		ListOfXXX *res;
		GF_SAFEALLOC(res, ListOfXXX);
		if (!res) {
			lsr->last_error = GF_OUT_OF_MEM;
			return NULL;
		}
		*res = gf_list_new();
		f_val = lsr_read_fraction_12_item(lsr);
		if (f_val) gf_list_add(*res, f_val);
		return res;
	}
	case SMIL_KeyPoints_datatype/*ITYPE_0to1 - keyPoints*/:
	{
		ListOfXXX *res;
		GF_SAFEALLOC(res, ListOfXXX);
		if (!res) {
			lsr->last_error = GF_OUT_OF_MEM;
			return NULL;
		}
		*res = gf_list_new();
		f_val = (Fixed*)gf_malloc(sizeof(Fixed));
		if (!f_val) {
			lsr->last_error = GF_OUT_OF_MEM;
		} else {
			*f_val = lsr_read_fixed_clamp(lsr, "value");
			gf_list_add(*res, f_val);
		}
		return res;
	}
	case SMIL_Times_datatype/*ITYPE_smil_time*/:
	{
		ListOfXXX *res;
		GF_SAFEALLOC(res, ListOfXXX);
		if (!res) {
			lsr->last_error = GF_OUT_OF_MEM;
			return NULL;
		}
		*res = gf_list_new();
		if (! *res) {
			lsr->last_error = GF_OUT_OF_MEM;
		} else {
			gf_list_add(*res, lsr_read_smil_time(lsr, node) );
		}
		return res;
	}
	default:
		lsr_read_extension(lsr, "privateData");
		break;
	}
	return NULL;
}