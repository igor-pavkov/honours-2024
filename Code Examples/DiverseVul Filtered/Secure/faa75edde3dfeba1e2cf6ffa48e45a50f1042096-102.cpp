static void lsr_read_codec_IDREF(GF_LASeRCodec *lsr, XMLRI *href, const char *name)
{
	GF_Node *n;
	u32 flag;
	u32 nID = 1+lsr_read_vluimsbf5(lsr, name);

	GF_LSR_READ_INT(lsr, flag, 1, "reserved");
	if (flag) {
		u32 len = lsr_read_vluimsbf5(lsr, "len");
		GF_LSR_READ_INT(lsr, flag, len, "reserved");
	}

	n = gf_sg_find_node(lsr->sg, nID);
	if (!n) {
		char NodeID[1024];
		sprintf(NodeID, "N%d", nID-1);
		href->string = gf_strdup(NodeID);
		if (href->type!=0xFF)
			gf_list_add(lsr->deferred_hrefs, href);
		href->type = XMLRI_ELEMENTID;
		return;
	}
	href->target = (SVG_Element *)n;
	href->type = XMLRI_ELEMENTID;
	gf_node_register_iri(lsr->sg, href);
}