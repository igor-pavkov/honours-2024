static GF_Node *lsr_read_animateMotion(GF_LASeRCodec *lsr, SVG_Element *parent)
{
	Bool flag;
	GF_Node *elt = gf_node_new(lsr->sg, TAG_SVG_animateMotion);
	lsr_read_id(lsr, elt);
	lsr_read_rare(lsr, elt);
	lsr_read_accumulate(lsr, elt);
	lsr_read_additive(lsr, elt);
	lsr_read_anim_value(lsr, elt, TAG_SVG_ATT_by, "by");
	lsr_read_calc_mode(lsr, elt);
	lsr_read_anim_value(lsr, elt, TAG_SVG_ATT_from, "from");
	lsr_read_fraction_12(lsr, elt, TAG_SVG_ATT_keySplines, "keySplines");
	lsr_read_fraction_12(lsr, elt, TAG_SVG_ATT_keyTimes, "keyTimes");
	lsr_read_anim_values(lsr, elt);
	lsr_read_attribute_type(lsr, elt);
	lsr_read_smil_times(lsr, elt, TAG_SVG_ATT_begin, NULL, "begin", 1);
	lsr_read_duration(lsr, elt);
	lsr_read_anim_fill(lsr, elt);
	lsr_read_anim_repeatCount(lsr, elt);
	lsr_read_repeat_duration(lsr, elt);
	lsr_read_anim_restart(lsr, elt);
	lsr_read_anim_value(lsr, elt, TAG_SVG_ATT_to, "to");
	lsr_read_float_list(lsr, elt, TAG_SVG_ATT_keyPoints, NULL, "keyPoints");
	GF_LSR_READ_INT(lsr, flag, 1, "hasPath");
	if (flag) lsr_read_path_type(lsr, elt, TAG_SVG_ATT_path, NULL, "path");

	lsr_read_rotate_type(lsr, elt);
	lsr_read_href(lsr, elt);
	lsr_read_lsr_enabled(lsr, elt);
	lsr_read_any_attribute(lsr, elt, 1);

	if (!lsr_setup_smil_anim(lsr, (SVG_Element*)elt, parent)) {
		gf_list_add(lsr->deferred_anims, elt);
		lsr_read_group_content_post_init(lsr, (SVG_Element*)elt, 1);
	} else {
		lsr_read_group_content_post_init(lsr, (SVG_Element*)elt, 0);
	}
	return elt;
}