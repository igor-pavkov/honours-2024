static GF_Node *lsr_read_polygon(GF_LASeRCodec *lsr, Bool is_polyline, u32 same_type)
{
	GF_FieldInfo info;
	GF_Node *elt = gf_node_new(lsr->sg, is_polyline ? TAG_SVG_polyline : TAG_SVG_polygon);

	lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_points, 1, 0, &info);

	if (same_type) {
		if (lsr->prev_polygon) {
			lsr_restore_base(lsr, (SVG_Element*)elt, (SVG_Element *)lsr->prev_polygon, /*(same_type==2) ? 1 : */ 0, /*(same_type==3) ? 1 : */ 0);
		} else {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[LASeR] samepolyXXX coded in bitstream but no polyXXX defined !\n"));
		}
		lsr_read_id(lsr, elt);
		if (same_type==2) lsr_read_fill(lsr, elt);
		else if (same_type==3) lsr_read_stroke(lsr, elt);
		lsr_read_point_sequence(lsr, *(GF_List**)info.far_ptr, "points");
	} else {
		lsr_read_id(lsr, elt);
		lsr_read_rare_full(lsr, elt);
		lsr_read_fill(lsr, elt);
		lsr_read_stroke(lsr, elt);
		lsr_read_point_sequence(lsr, *(GF_List**)info.far_ptr, "points");
		lsr_read_any_attribute(lsr, elt, 1);
		lsr->prev_polygon = (SVG_Element*)elt;
	}
	lsr_read_group_content(lsr, elt, same_type);
	return elt;
}