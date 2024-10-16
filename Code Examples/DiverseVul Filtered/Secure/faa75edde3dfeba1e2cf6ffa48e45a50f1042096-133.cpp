static GF_Node *lsr_read_foreignObject(GF_LASeRCodec *lsr)
{
	u32 flag;
	GF_Node *elt = gf_node_new(lsr->sg, TAG_SVG_foreignObject);
	lsr_read_id(lsr, elt);
	lsr_read_rare_full(lsr, elt);
	lsr_read_fill(lsr, elt);
	lsr_read_stroke(lsr, elt);
	lsr_read_eRR(lsr, elt);
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_height, 0, "height");
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_width, 0, "width");
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_x, 1, "x");
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_y, 1, "y");

	lsr_read_any_attribute(lsr, elt, 1);
	/*	TODO
		bit(1) opt_group;
		if(opt_group) {
			vluimsbf5 occ1;
			for(int t=0;t<occ1;t++) {
				privateElementContainer child0[[t]];
			}
		}
	*/
	GF_LSR_READ_INT(lsr, flag, 1, "opt_group");
	return elt;
}