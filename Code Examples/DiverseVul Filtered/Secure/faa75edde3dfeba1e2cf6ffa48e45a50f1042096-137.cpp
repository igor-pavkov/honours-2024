static SMIL_Time *lsr_read_smil_time(GF_LASeRCodec *lsr, GF_Node *n)
{
	SMIL_Time *t;
	u32 val;

	GF_SAFEALLOC(t, SMIL_Time);
	if (!t) {
		lsr->last_error = GF_OUT_OF_MEM;
		return NULL;
	}
	t->type = GF_SMIL_TIME_CLOCK;

	GF_LSR_READ_INT(lsr, val, 1, "hasEvent");
	if (val) {
		t->type = GF_SMIL_TIME_EVENT;
		GF_LSR_READ_INT(lsr, val, 1, "hasIdentifier");
		if (val) {
			XMLRI iri;
			iri.type = 0xFF;
			iri.string = NULL;
			lsr_read_codec_IDREF(lsr, &iri, "idref");
			gf_node_unregister_iri(lsr->sg, &iri);
			if (iri.string) {
				t->element_id = iri.string;
			} else {
				t->element = (GF_Node *)iri.target;
			}
		}
		lsr_read_event_type(lsr, &t->event);
		if (t->event.type==GF_EVENT_EXECUTION_TIME) {
			t->type = GF_SMIL_TIME_CLOCK;
			t->clock = gf_node_get_scene_time(n);
		}
	}
	GF_LSR_READ_INT(lsr, val, 1, "hasClock");
	if (val) {
		u32 now;
		GF_LSR_READ_INT(lsr, val, 1, "sign");
		now = lsr_read_vluimsbf5(lsr, "value");
		t->clock = now;
		t->clock /= lsr->time_resolution;
		if (val) t->clock *= -1;
	}
	return t;
}