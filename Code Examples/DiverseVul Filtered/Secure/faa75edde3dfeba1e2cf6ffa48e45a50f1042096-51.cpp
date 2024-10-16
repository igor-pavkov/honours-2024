static GF_Node *lsr_read_data(GF_LASeRCodec *lsr, u32 node_tag)
{
	GF_Node *elt = gf_node_new(lsr->sg, node_tag);
	lsr_read_id(lsr, elt);
	lsr_read_rare(lsr, elt);
	lsr_read_any_attribute(lsr, elt, 1);
	lsr_read_group_content(lsr, elt, 0);
	return elt;
}