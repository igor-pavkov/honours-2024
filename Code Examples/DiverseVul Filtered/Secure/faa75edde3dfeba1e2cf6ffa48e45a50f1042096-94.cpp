static void lsr_read_float_list(GF_LASeRCodec *lsr, GF_Node *n, u32 tag, SVG_Coordinates*coords, const char *name)
{
	u32 i, count;
	GF_LSR_READ_INT(lsr, count, 1, name);
	if (!count) return;

	if (!coords) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(n, tag, GF_TRUE, 0, &info);
		coords = (SVG_Coordinates*)info.far_ptr;
	} else {
		while (gf_list_count(*coords)) {
			Fixed *v = (Fixed *)gf_list_last(*coords);
			gf_list_rem_last(*coords);
			gf_free(v);
		}
	}
	count = lsr_read_vluimsbf5(lsr, "count");
	if (tag == TAG_SVG_ATT_text_rotate) {
		for (i=0; i<count; i++) {
			SVG_Number *num = (SVG_Number *)gf_malloc(sizeof(SVG_Number));
			if (!num) {
				lsr->last_error = GF_OUT_OF_MEM;
			} else {
				num->type = SVG_NUMBER_VALUE;
				num->value = lsr_read_fixed_16_8(lsr, "val");
				gf_list_add(*coords, num);
			}
			if (lsr->last_error) return;
		}
	}
	else {
		for (i=0; i<count; i++) {
			Fixed *num = (Fixed *)gf_malloc(sizeof(Fixed));
			if (!num) {
				lsr->last_error = GF_OUT_OF_MEM;
			} else {
				*num = lsr_read_fixed_16_8(lsr, "val");
			}
			gf_list_add(*coords, num);
			if (lsr->last_error) return;
		}
	}
}