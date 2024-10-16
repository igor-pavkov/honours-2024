GF_Err gf_isom_setup_hint_track(GF_ISOFile *movie, u32 trackNumber, GF_ISOHintFormat HintType)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_TrackReferenceBox *tref;
	GF_TrackReferenceTypeBox *dpnd;
	GF_HintMediaHeaderBox *hmhd;
	//UDTA related ...
	GF_UserDataBox *udta;


	//what do we support
	switch (HintType) {
	case GF_ISOM_HINT_RTP:
		break;
	default:
		return GF_NOT_SUPPORTED;
	}
	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return gf_isom_last_error(movie);

	//check we have a hint ...
	if ( !IsHintTrack(trak)) {
		return GF_BAD_PARAM;
	}
	hmhd = (GF_HintMediaHeaderBox *)trak->Media->information->InfoHeader;
	//make sure the subtype was not already defined
	if (hmhd->subType) return GF_BAD_PARAM;
	//store the HintTrack format for later use...
	hmhd->subType = HintType;


	//hint tracks always have a tref and everything ...
	if (!trak->References) {
		tref = (GF_TrackReferenceBox *) gf_isom_box_new_parent(&trak->child_boxes, GF_ISOM_BOX_TYPE_TREF);
		if (!tref) return GF_OUT_OF_MEM;
		e = trak_on_child_box((GF_Box*)trak, (GF_Box *)tref, GF_FALSE);
		if (e) return e;
	}
	tref = trak->References;

	//do we have a hint reference on this trak ???
	e = Track_FindRef(trak, GF_ISOM_BOX_TYPE_HINT, &dpnd);
	if (e) return e;
	//if yes, return false (existing hint track...)
	if (dpnd) return GF_BAD_PARAM;

	//create our dep
	dpnd = (GF_TrackReferenceTypeBox *) gf_isom_box_new_parent(&tref->child_boxes, GF_ISOM_BOX_TYPE_REFT);
	if (!dpnd) return GF_OUT_OF_MEM;
	dpnd->reference_type = GF_ISOM_BOX_TYPE_HINT;

	//for RTP, we need to do some UDTA-related stuff...
	if (HintType != GF_ISOM_HINT_RTP) return GF_OK;

	if (!trak->udta) {
		//create one
		udta = (GF_UserDataBox *) gf_isom_box_new_parent(&trak->child_boxes, GF_ISOM_BOX_TYPE_UDTA);
		if (!udta) return GF_OUT_OF_MEM;
		e = trak_on_child_box((GF_Box*)trak, (GF_Box *) udta, GF_FALSE);
		if (e) return e;
	}
	udta = trak->udta;

	//HNTI
	e = udta_on_child_box((GF_Box *)udta, gf_isom_box_new(GF_ISOM_BOX_TYPE_HNTI), GF_FALSE);
	if (e) return e;

	/*
		//NAME
		e = udta_on_child_box((GF_Box *)udta, gf_isom_box_new(GF_ISOM_BOX_TYPE_NAME));
		if (e) return e;
		//HINF
		return udta_on_child_box((GF_Box *)udta, gf_isom_box_new(GF_ISOM_BOX_TYPE_HINF));
	*/
	return GF_OK;
}