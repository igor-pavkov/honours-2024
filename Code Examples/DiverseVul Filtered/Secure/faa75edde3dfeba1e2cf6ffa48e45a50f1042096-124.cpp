static void lsr_restore_base(GF_LASeRCodec *lsr, SVG_Element *elt, SVG_Element *base, Bool reset_fill, Bool reset_stroke)
{
	GF_Err e;
	GF_FieldInfo f_base, f_clone;
	SVGAttribute *att;

	/*clone all propertie from base*/
	att = base->attributes;
	while (att) {
		Bool is_fill, is_stroke;
		is_fill = is_stroke = GF_FALSE;
		switch (att->tag) {
		/*for all properties*/
		case TAG_SVG_ATT_fill:
			is_fill = GF_TRUE;
			break;
		case TAG_SVG_ATT_stroke:
			is_stroke = GF_TRUE;
			break;
		case TAG_SVG_ATT_audio_level:
		case TAG_SVG_ATT_color:
		case TAG_SVG_ATT_color_rendering:
		case TAG_SVG_ATT_display:
		case TAG_SVG_ATT_display_align:
		case TAG_SVG_ATT_fill_opacity:
		case TAG_SVG_ATT_fill_rule:
		case TAG_SVG_ATT_font_family:
		case TAG_SVG_ATT_font_size:
		case TAG_SVG_ATT_font_style:
		case TAG_SVG_ATT_font_variant:
		case TAG_SVG_ATT_font_weight:
		case TAG_SVG_ATT_image_rendering:
		case TAG_SVG_ATT_line_increment:
		case TAG_SVG_ATT_opacity:
		case TAG_SVG_ATT_pointer_events:
		case TAG_SVG_ATT_shape_rendering:
		case TAG_SVG_ATT_solid_color:
		case TAG_SVG_ATT_solid_opacity:
		case TAG_SVG_ATT_stop_color:
		case TAG_SVG_ATT_stop_opacity:
		case TAG_SVG_ATT_stroke_dasharray:
		case TAG_SVG_ATT_stroke_dashoffset:
		case TAG_SVG_ATT_stroke_linecap:
		case TAG_SVG_ATT_stroke_linejoin:
		case TAG_SVG_ATT_stroke_miterlimit:
		case TAG_SVG_ATT_stroke_opacity:
		case TAG_SVG_ATT_stroke_width:
		case TAG_SVG_ATT_text_align:
		case TAG_SVG_ATT_text_anchor:
		case TAG_SVG_ATT_text_rendering:
		case TAG_SVG_ATT_vector_effect:
		case TAG_SVG_ATT_viewport_fill:
		case TAG_SVG_ATT_viewport_fill_opacity:
		case TAG_SVG_ATT_visibility:
		/*and xml:_class*/
		case TAG_SVG_ATT__class:
		case TAG_SVG_ATT_externalResourcesRequired:
			break;

		/*pathLength for path*/
		case TAG_SVG_ATT_pathLength:
			break;
		/*rx & ry for rect*/
		case TAG_SVG_ATT_rx:
		case TAG_SVG_ATT_ry:
			if (base->sgprivate->tag!=TAG_SVG_rect) {
				att = att->next;
				continue;
			}
			break;
		/*x & y for use*/
		case TAG_SVG_ATT_x:
		case TAG_SVG_ATT_y:
			if (base->sgprivate->tag!=TAG_SVG_use) {
				att = att->next;
				continue;
			}
			break;
		/*editable & rotate for text*/
		case TAG_SVG_ATT_editable:
		case TAG_SVG_ATT_rotate:
			if (base->sgprivate->tag!=TAG_SVG_text) {
				att = att->next;
				continue;
			}
			break;
		case TAG_SVG_ATT_transform:
			break;
		default:
			att = att->next;
			continue;
		}
		/*clone field*/
		e = gf_node_get_attribute_by_tag((GF_Node*)elt, att->tag, GF_TRUE, GF_FALSE, &f_clone);
		if (e) goto err_exit;
		f_base.fieldIndex = att->tag;
		f_base.fieldType = att->data_type;
		f_base.far_ptr = att->data;
		e = gf_svg_attributes_copy(&f_clone, &f_base, GF_FALSE);
		if (e) goto err_exit;

		if (is_fill && reset_fill) {
			SVG_Paint*p = (SVG_Paint*)f_clone.far_ptr;
			if (p->iri.string) gf_free(p->iri.string);
			memset(p, 0, sizeof(SVG_Paint));
		}
		if (is_stroke && reset_stroke) {
			SVG_Paint*p = (SVG_Paint*)f_clone.far_ptr;
			if (p->iri.string) gf_free(p->iri.string);
			memset(p, 0, sizeof(SVG_Paint));
		}
		att = att->next;
	}
	return;

err_exit:
	lsr->last_error = e;
}