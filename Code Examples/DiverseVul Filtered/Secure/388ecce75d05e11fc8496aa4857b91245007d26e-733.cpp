GF_Err stsd_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_UnknownBox *def;
	if (!a) return GF_OK;

	if (gf_box_valid_in_parent(a, "stsd")) {
		return GF_OK;
	}
	switch (a->type) {
	//unknown sample description: we need a specific box to handle the data ref index
	//rather than a default box ...
	case GF_ISOM_BOX_TYPE_UNKNOWN:
		def = (GF_UnknownBox *)a;
		/*we need at least 8 bytes for unknown sample entries*/
		if (def->dataSize < 8) {
			gf_isom_box_del_parent(&s->child_boxes, a);
			return GF_ISOM_INVALID_MEDIA;
		}
		return GF_OK;

	default:
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Cannot process box of type %s\n", gf_4cc_to_str(a->type)));
		return GF_ISOM_INVALID_FILE;
	}
}