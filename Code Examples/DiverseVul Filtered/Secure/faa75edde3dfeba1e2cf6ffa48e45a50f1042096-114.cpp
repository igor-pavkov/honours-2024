static void lsr_read_focus(GF_LASeRCodec *lsr, SVG_Focus *foc, const char *name)
{
	u32 flag;

	if (foc->target.string) {
		gf_free(foc->target.string);
		foc->target.string = NULL;
	}
	if (foc->target.target) foc->target.target = NULL;
	gf_node_unregister_iri(lsr->sg, &foc->target);

	GF_LSR_READ_INT(lsr, flag, 1, "isEnum");
	if (flag) {
		GF_LSR_READ_INT(lsr, foc->type, 1, "enum");
	} else {
		foc->type = SVG_FOCUS_IRI;
		lsr_read_codec_IDREF(lsr, &foc->target, "id");
	}
}