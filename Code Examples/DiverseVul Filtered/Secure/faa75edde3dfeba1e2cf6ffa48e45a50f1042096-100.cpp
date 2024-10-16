static GF_Node *lsr_read_animateTransform(GF_LASeRCodec *lsr, SVG_Element *parent)
{
	u32 type;
	u32 flag;
	GF_FieldInfo info;
	GF_Node *elt= gf_node_new(lsr->sg, TAG_SVG_animateTransform);
	lsr_read_id(lsr, elt);
	lsr_read_rare(lsr, elt);
	lsr_read_attribute_name(lsr, elt);

	/*enumeration rotate{0} scale{1} skewX{2} skewY{3} translate{4}*/
	GF_LSR_READ_INT(lsr, flag, 3, "rotscatra");
	switch (flag) {
	case 0:
		type = SVG_TRANSFORM_ROTATE;
		break;
	case 1:
		type = SVG_TRANSFORM_SCALE;
		break;
	case 2:
		type = SVG_TRANSFORM_SKEWX;
		break;
	case 3:
		type = SVG_TRANSFORM_SKEWY;
		break;
	case 4:
		type = SVG_TRANSFORM_TRANSLATE;
		break;
	default:
		type = SVG_TRANSFORM_ROTATE;
		break;
	}
	if (gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_transform_type, 1, 0, &info)==GF_OK) {
		*(SVG_TransformType *)info.far_ptr = type;
	}

	lsr_read_accumulate(lsr, elt);
	lsr_read_additive(lsr, elt);
	lsr_read_anim_value_ex(lsr, elt, TAG_SVG_ATT_by, "by", &type);
	lsr_read_calc_mode(lsr, elt);
	lsr_read_anim_value_ex(lsr, elt, TAG_SVG_ATT_from, "from", &type);
	lsr_read_fraction_12(lsr, elt, TAG_SVG_ATT_keySplines, "keySplines");
	lsr_read_fraction_12(lsr, elt, TAG_SVG_ATT_keyTimes, "keyTimes");
	lsr_read_anim_values_ex(lsr, elt, &type);
	lsr_read_attribute_type(lsr, elt);

	lsr_read_smil_times(lsr, elt, TAG_SVG_ATT_begin, NULL, "begin", 1);
	lsr_read_duration(lsr, elt);
	lsr_read_anim_fill(lsr, elt);
	lsr_read_anim_repeatCount(lsr, elt);
	lsr_read_repeat_duration(lsr, elt);
	lsr_read_anim_restart(lsr, elt);
	lsr_read_anim_value_ex(lsr, elt, TAG_SVG_ATT_to, "to", &type);

	lsr_read_href(lsr, elt);
	lsr_read_lsr_enabled(lsr, elt);
	lsr_read_any_attribute(lsr, elt, 1);

	if (!lsr_setup_smil_anim(lsr, (SVG_Element*)elt, parent)) {
		gf_list_add(lsr->deferred_anims, elt);
		lsr_read_group_content_post_init(lsr, (SVG_Element*)elt, 1);
	} else {
		lsr_read_group_content(lsr, elt, 0);
	}
	return elt;
}