static Bool lsr_setup_smil_anim(GF_LASeRCodec *lsr, SVG_Element *anim, SVG_Element *anim_parent)
{
	GF_FieldInfo info;
	u32 coded_type, not_res;
	GF_Node *target;
	Bool is_animateMotion, is_animateTransform;
	XMLRI *xlink;
	SMIL_AttributeName *name = NULL;
	SMIL_AnimateValue *value;

	/*setup smil events*/
	not_res = 0;
	if (gf_node_get_attribute_by_tag((GF_Node *)anim, TAG_SVG_ATT_begin, GF_FALSE, GF_FALSE, &info)==GF_OK) {
		if (!lsr_init_smil_times(lsr, anim, *(GF_List**)info.far_ptr, anim_parent)) not_res++;
	}

	if (gf_node_get_attribute_by_tag((GF_Node *)anim, TAG_SVG_ATT_end, GF_FALSE, GF_FALSE, &info)==GF_OK) {
		if (!lsr_init_smil_times(lsr, anim, *(GF_List**)info.far_ptr, anim_parent)) not_res++;
	}


	/*get xlink*/
	xlink = NULL;
	if (gf_node_get_attribute_by_tag((GF_Node *)anim, TAG_XLINK_ATT_href, GF_FALSE, GF_FALSE, &info)==GF_OK) {
		xlink = info.far_ptr;
	}

	/*setup target node*/
	if (!xlink || !xlink->target) {
		/*target not received*/
		if (xlink && (xlink->type == XMLRI_ELEMENTID)) return GF_FALSE;

		if (!xlink) {
			/*target is parent, initialize xlink (needed by anim module)*/
			if (gf_node_get_attribute_by_tag((GF_Node *)anim, TAG_XLINK_ATT_href, GF_TRUE, GF_FALSE, &info)==GF_OK) {
				xlink = info.far_ptr;
			} else {
				return GF_FALSE;
			}
		}

		xlink->type = XMLRI_ELEMENTID;
		xlink->target = anim_parent;
		gf_node_register_iri(lsr->sg, xlink);
		target = (GF_Node *)anim_parent;
	} else {
		target = (GF_Node *)xlink->target;
	}
	if (!target || not_res) return GF_FALSE;

	is_animateTransform = is_animateMotion = GF_FALSE;
	if (anim->sgprivate->tag==TAG_SVG_animateMotion) is_animateMotion = GF_TRUE;
	else if (anim->sgprivate->tag==TAG_SVG_animateTransform) {
		is_animateTransform = GF_TRUE;
	}
	if (is_animateMotion) goto translate_vals;

	/*for all except animateMotion, get attributeName*/
	if (gf_node_get_attribute_by_tag((GF_Node *)anim, TAG_SVG_ATT_attributeName, GF_FALSE, GF_FALSE, &info)==GF_OK) {
		name = info.far_ptr;
	}
	if (!name) {
		return GF_FALSE;
	}

	if (!name->field_ptr) {
		if (gf_node_get_attribute_by_tag((GF_Node *)target, name->type, GF_TRUE, GF_FALSE, &info)!=GF_OK) return GF_FALSE;
		name->field_ptr = info.far_ptr;
		name->type = info.fieldType;
		name->tag = info.fieldIndex;
	}


	/*browse all anim types and retranslate anim values. This must be done in 2 steps since we may not have received
	the target node when parsing the animation node*/
translate_vals:

	/*and setup anim values*/
	if (gf_node_get_attribute_by_tag((GF_Node *)anim, TAG_SVG_ATT_from, GF_FALSE, GF_FALSE, &info)==GF_OK) {
		if (is_animateTransform) {
			name->type = ((SMIL_AnimateValue*)info.far_ptr)->type;
		} else {
			value = info.far_ptr;
			coded_type = value->type;
			value->type = is_animateMotion ? SVG_Motion_datatype : name->type;
			lsr_translate_anim_value(lsr, value, coded_type);
		}
	}
	if (gf_node_get_attribute_by_tag((GF_Node *)anim, TAG_SVG_ATT_by, GF_FALSE, GF_FALSE, &info)==GF_OK) {
		if (is_animateTransform) {
			name->type = ((SMIL_AnimateValue*)info.far_ptr)->type;
		} else {
			value = info.far_ptr;
			coded_type = value->type;
			value->type = is_animateMotion ? SVG_Motion_datatype : name->type;
			lsr_translate_anim_value(lsr, value, coded_type);
		}
	}
	if (gf_node_get_attribute_by_tag((GF_Node *)anim, TAG_SVG_ATT_to, GF_FALSE, GF_FALSE, &info)==GF_OK) {
		if (is_animateTransform) {
			name->type = ((SMIL_AnimateValue*)info.far_ptr)->type;
		} else {
			value = info.far_ptr;
			coded_type = value->type;
			value->type = is_animateMotion ? SVG_Motion_datatype : name->type;
			lsr_translate_anim_value(lsr, value, coded_type);
		}
	}
	if (gf_node_get_attribute_by_tag((GF_Node *)anim, TAG_SVG_ATT_values, GF_FALSE, GF_FALSE, &info)==GF_OK) {
		if (is_animateTransform) {
			name->type = ((SMIL_AnimateValues*)info.far_ptr)->type;
		} else {
			SMIL_AnimateValues *values = info.far_ptr;
			coded_type = values->type;
			values->type = is_animateMotion ? SVG_Motion_datatype : name->type;
			values->laser_strings = 0;
			lsr_translate_anim_values(lsr, values, coded_type);
		}
	}

	return GF_TRUE;
}