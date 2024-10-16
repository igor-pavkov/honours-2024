static void lsr_read_text_content(GF_LASeRCodec *lsr, GF_Node *elt)
{
	char *str = NULL;
	lsr_read_byte_align_string(lsr, &str, "textContent");
	if (!str) return;
	gf_dom_add_text_node(elt, str);
}