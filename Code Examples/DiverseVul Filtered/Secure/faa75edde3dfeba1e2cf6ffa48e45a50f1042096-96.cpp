static GF_Node *lsr_read_mpath(GF_LASeRCodec *lsr)
{
	GF_Node *elt = gf_node_new(lsr->sg, TAG_SVG_mpath);
	lsr_read_id(lsr, elt);
	lsr_read_rare(lsr, elt);
	lsr_read_href(lsr, elt);
	lsr_read_any_attribute(lsr, elt, 1);
	lsr_read_group_content(lsr, elt, 0);
	return elt;
}