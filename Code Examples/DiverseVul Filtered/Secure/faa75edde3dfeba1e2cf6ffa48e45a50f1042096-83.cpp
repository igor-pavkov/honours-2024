static GF_Node *lsr_read_conditional(GF_LASeRCodec *lsr)
{
	GF_Node *elt = gf_node_new(lsr->sg, TAG_LSR_conditional);
	lsr_read_id(lsr, elt);
	lsr_read_rare(lsr, elt);
	lsr_read_smil_times(lsr, elt, TAG_SVG_ATT_begin, NULL, "begin", 1);
	lsr_read_eRR(lsr, elt);
	lsr_read_lsr_enabled(lsr, elt);
	lsr_read_any_attribute(lsr, elt, 1);
	lsr_read_command_list(lsr, NULL, (SVG_Element*)elt, 0);

	lsr->has_conditionnals = GF_TRUE;
	gf_node_init(elt);
	return elt;
}