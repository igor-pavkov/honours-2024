static GF_Node *lsr_read_audio(GF_LASeRCodec *lsr, SVG_Element *parent)
{
	GF_Node *elt= gf_node_new(lsr->sg, TAG_SVG_audio);
	lsr_read_id(lsr, elt);
	lsr_read_rare(lsr, elt);
	lsr_read_smil_times(lsr, elt, TAG_SVG_ATT_begin, NULL, "begin", 1);
	lsr_read_duration(lsr, elt);
	lsr_read_eRR(lsr, elt);
	lsr_read_anim_repeatCount(lsr, elt);
	lsr_read_repeat_duration(lsr, elt);
	lsr_read_anim_restart(lsr, elt);
	lsr_read_sync_behavior(lsr, elt);
	lsr_read_sync_tolerance(lsr, elt);
	lsr_read_content_type(lsr, elt);
	lsr_read_href(lsr, elt);

	lsr_read_clip_time(lsr, elt, TAG_SVG_ATT_clipBegin, "clipBegin");
	lsr_read_clip_time(lsr, elt, TAG_SVG_ATT_clipEnd, "clipEnd");
	lsr_read_sync_reference(lsr, elt);
	lsr_read_any_attribute(lsr, elt, 1);
	lsr_read_group_content(lsr, elt, 0);
	return elt;
}