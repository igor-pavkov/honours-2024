GF_Err dinf_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_DataInformationBox *dinf;
	GF_Err e = gf_isom_box_array_read(s, bs, dinf_on_child_box);
	if (e) {
		return e;
	}
	dinf = (GF_DataInformationBox *)s;
	if (!dinf->dref) {
		if (! (gf_bs_get_cookie(bs) & GF_ISOM_BS_COOKIE_NO_LOGS) ) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing dref box in dinf\n"));
		}
		dinf->dref = (GF_DataReferenceBox *) gf_isom_box_new_parent(&dinf->child_boxes, GF_ISOM_BOX_TYPE_DREF);
	}
	return GF_OK;
}