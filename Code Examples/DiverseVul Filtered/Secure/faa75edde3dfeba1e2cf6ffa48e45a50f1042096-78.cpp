static GF_Node *lsr_read_video(GF_LASeRCodec *lsr, SVG_Element *parent)
{
	GF_FieldInfo info;
	u32 flag;
	GF_Node*elt = gf_node_new(lsr->sg, TAG_SVG_video);
	lsr_read_id(lsr, elt);
	lsr_read_rare_full(lsr, elt);
	lsr_read_smil_times(lsr, elt, TAG_SVG_ATT_begin, NULL, "begin", 1);
	lsr_read_duration(lsr, elt);
	lsr_read_eRR(lsr, elt);
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_height, 1, "height");
	GF_LSR_READ_INT(lsr, flag, 1, "hasOverlay");
	if (flag) {
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_overlay, 1, 1, &info);
		GF_LSR_READ_INT(lsr, flag, 1, "choice");
		if (flag) {
			GF_LSR_READ_INT(lsr, *(SVG_Overlay*)info.far_ptr, 1, "choice");
		} else {
			char *str = NULL;
			lsr_read_byte_align_string(lsr, & str, "overlayExt");
			if (str) gf_free(str);
		}
	}
	lsr_read_preserve_aspect_ratio(lsr, elt);
	lsr_read_anim_repeatCount(lsr, elt);
	lsr_read_repeat_duration(lsr, elt);
	lsr_read_anim_restart(lsr, elt);
	lsr_read_sync_behavior(lsr, elt);
	lsr_read_sync_tolerance(lsr, elt);
	lsr_read_transform_behavior(lsr, elt);
	lsr_read_content_type(lsr, elt);
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_width, 1, "width");
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_x, 1, "x");
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_y, 1, "y");
	lsr_read_href(lsr, elt);

	lsr_read_clip_time(lsr, elt, TAG_SVG_ATT_clipBegin, "clipBegin");
	lsr_read_clip_time(lsr, elt, TAG_SVG_ATT_clipEnd, "clipEnd");

	GF_LSR_READ_INT(lsr, flag, 1, "hasFullscreen");
	if (flag) {
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_fullscreen, 1, 0, &info);
		GF_LSR_READ_INT(lsr, *(SVG_Boolean *)info.far_ptr, 1, "fullscreen");
	}

	lsr_read_sync_reference(lsr, elt);
	lsr_read_any_attribute(lsr, elt, 1);
	lsr_read_group_content(lsr, elt, 0);
	return elt;
}