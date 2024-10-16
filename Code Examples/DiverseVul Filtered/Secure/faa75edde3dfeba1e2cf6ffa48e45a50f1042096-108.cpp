static void lsr_read_smil_times(GF_LASeRCodec *lsr, GF_Node *n, u32 tag, SMIL_Times *times, const char *name, Bool skipable)
{
	GF_FieldInfo info;
	SMIL_Time *v;
	u32 val, i, count;

	if (skipable) {
		GF_LSR_READ_INT(lsr, val, 1, name);
		if (!val) return;
	}
	if (!times) {
		lsr->last_error = gf_node_get_attribute_by_tag(n, tag, GF_TRUE, GF_FALSE, &info);
		times = (SMIL_Times*)info.far_ptr;
	}

	while (gf_list_count(*times)) {
		v = (SMIL_Time *)gf_list_last(*times);
		gf_list_rem_last(*times);
		if (v->element_id) gf_free(v->element_id);
		gf_free(v);
	}

	GF_LSR_READ_INT(lsr, val, 1, "choice");
	if (val) {
		GF_SAFEALLOC(v, SMIL_Time);
		if (v) {
			v->type = GF_SMIL_TIME_INDEFINITE;
			gf_list_add(*times, v);
		} else {
			lsr->last_error = GF_OUT_OF_MEM;
		}
		return;
	}
	count = lsr_read_vluimsbf5(lsr, "count");
	for (i=0; i<count; i++) {
		v = lsr_read_smil_time(lsr, n);
		gf_list_add(*times, v);
		if (lsr->last_error) return;
	}
}