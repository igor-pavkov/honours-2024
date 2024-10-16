static GF_Node *lsr_read_svg(GF_LASeRCodec *lsr, Bool init_node)
{
	GF_FieldInfo info;
	SMIL_Duration snap;
	u32 flag;
	GF_Node *elt = gf_node_new(lsr->sg, TAG_SVG_svg);
	lsr_read_id(lsr, elt);
	lsr_read_rare(lsr, elt);
	lsr_read_fill(lsr, elt);
	lsr_read_stroke(lsr, elt);
	lsr_read_string_attribute(lsr, elt, TAG_SVG_ATT_baseProfile, "baseProfile");
	lsr_read_string_attribute(lsr, elt, TAG_SVG_ATT_contentScriptType, "contentScriptType");
	lsr_read_eRR(lsr, elt);
	lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_height, 1, 0, &info);
	lsr_read_value_with_units(lsr, info.far_ptr, "height");
	GF_LSR_READ_INT(lsr, flag, 1, "hasPlaybackOrder");
	if (flag) {
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_playbackOrder, 1, 1, &info);
		GF_LSR_READ_INT(lsr, flag, 1, "playbackOrder");
		if (flag) *(SVG_PlaybackOrder*)info.far_ptr = SVG_PLAYBACKORDER_FORWARDONLY;
	}

	lsr_read_preserve_aspect_ratio(lsr, elt);


	GF_LSR_READ_INT(lsr, flag, 1, "has_snapshotTime");
	if (flag) {
		lsr_read_duration_ex(lsr, NULL, 0, &snap, "snapshotTime", 0);
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_snapshotTime, 1, 1, &info);
		if (snap.type==SMIL_DURATION_DEFINED) *((SVG_Clock *)info.far_ptr) = snap.clock_value;
	}

	GF_LSR_READ_INT(lsr, flag, 1, "hasSyncBehavior");
	if (flag) {
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_syncBehaviorDefault, 1, 0, &info);
		GF_LSR_READ_INT(lsr, flag, 2, "syncBehaviorDefault");
		switch (flag) {
		case 0:
			*((SMIL_SyncBehavior*)info.far_ptr) = SMIL_SYNCBEHAVIOR_CANSLIP;
			break;
		case 1:
			*((SMIL_SyncBehavior*)info.far_ptr) = SMIL_SYNCBEHAVIOR_INDEPENDENT;
			break;
		case 3:
			*((SMIL_SyncBehavior*)info.far_ptr) = SMIL_SYNCBEHAVIOR_LOCKED;
			break;
		default:
			*((SMIL_SyncBehavior*)info.far_ptr) = SMIL_SYNCBEHAVIOR_INHERIT;
			break;
		}
	}
	GF_LSR_READ_INT(lsr, flag, 1, "hasSyncToleranceDefault");
	if (flag) {
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_syncToleranceDefault, 1, 0, &info);
		((SMIL_SyncTolerance*)info.far_ptr)->type = SMIL_SYNCTOLERANCE_VALUE;
		GF_LSR_READ_INT(lsr, flag, 1, "choice");
		((SMIL_SyncTolerance*)info.far_ptr)->value = lsr_read_vluimsbf5(lsr, "value");
		((SMIL_SyncTolerance*)info.far_ptr)->value /= lsr->time_resolution;
	}
	GF_LSR_READ_INT(lsr, flag, 1, "hasTimelineBegin");
	if (flag) {
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_timelineBegin, 1, 0, &info);
		GF_LSR_READ_INT(lsr, flag, 1, "timelineBegin");
		if (flag) *(SVG_TimelineBegin*)info.far_ptr = SVG_TIMELINEBEGIN_ONLOAD;
	}
	lsr_read_string_attribute(lsr, elt, TAG_SVG_ATT_version, "version");

	GF_LSR_READ_INT(lsr, flag, 1, "hasViewBox");
	if (flag) {
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_viewBox, 1, 0, &info);
		((SVG_ViewBox*)info.far_ptr)->x = lsr_read_fixed_16_8(lsr, "viewbox.x");
		((SVG_ViewBox*)info.far_ptr)->y = lsr_read_fixed_16_8(lsr, "viewbox.y");
		((SVG_ViewBox*)info.far_ptr)->width = lsr_read_fixed_16_8(lsr, "viewbox.width");
		((SVG_ViewBox*)info.far_ptr)->height = lsr_read_fixed_16_8(lsr, "viewbox.height");
		((SVG_ViewBox*)info.far_ptr)->is_set = 1;
	}

	lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_width, 1, 0, &info);
	lsr_read_value_with_units(lsr, info.far_ptr, "width");

	GF_LSR_READ_INT(lsr, flag, 1, "hasZoomAndPan");
	if (flag) {
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_zoomAndPan, 1, 0, &info);
		GF_LSR_READ_INT(lsr, flag, 1, "zoomAndPan");
		*((SVG_ZoomAndPan*)info.far_ptr) = flag ? SVG_ZOOMANDPAN_MAGNIFY : SVG_ZOOMANDPAN_DISABLE;
	}
	lsr_read_any_attribute(lsr, elt, 1);
	/*store current root for listeners with no focus target*/
	lsr->current_root = elt;

	if (init_node) {
		gf_node_register(elt, NULL);
		gf_sg_set_root_node(lsr->sg, elt);
	}

	lsr_read_group_content(lsr, elt, 0);
	return elt;
}