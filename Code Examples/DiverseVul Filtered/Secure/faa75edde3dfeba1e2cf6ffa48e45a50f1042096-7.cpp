static GF_Node *lsr_read_g(GF_LASeRCodec *lsr, Bool is_same)
{
	GF_Node *elt = gf_node_new(lsr->sg, TAG_SVG_g);
	if (is_same) {
		if (lsr->prev_g) {
			lsr_restore_base(lsr, (SVG_Element*) elt, lsr->prev_g, 0, 0);
		} else {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[LASeR] sameg coded in bitstream but no g defined !\n"));
		}
		lsr_read_id(lsr, elt);
	} else {
		lsr_read_id(lsr, elt);
		lsr_read_rare_full(lsr, elt);
		lsr_read_fill(lsr, elt);
		lsr_read_stroke(lsr, elt);
		lsr_read_eRR(lsr, elt);
		lsr_read_any_attribute(lsr, elt, 1);
		lsr->prev_g = (SVG_Element*)elt;
	}
	lsr_read_group_content(lsr, elt, is_same);
	return elt;
}