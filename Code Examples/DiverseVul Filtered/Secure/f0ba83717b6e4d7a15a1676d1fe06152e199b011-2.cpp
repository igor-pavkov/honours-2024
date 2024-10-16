static u32 Media_FindOD_ID(GF_MediaBox *mdia, GF_ISOSample *sample, u32 track_id)
{
	GF_Err e;
	GF_ODCodec *ODdecode;
	GF_ODCom *com;
	u32 the_od_id;
	GF_ODUpdate *odU;
	GF_ESD *esd;
	GF_Descriptor *desc;
	GF_TrackReferenceTypeBox *mpod;
	u32 i, j;

	if (!mdia || !sample || !sample->data || !sample->dataLength) return 0;

	mpod = NULL;
	e = Track_FindRef(mdia->mediaTrack, GF_ISOM_BOX_TYPE_MPOD, &mpod);
	if (e) return 0;
	//no references, nothing to do...
	if (!mpod) return 0;

	the_od_id = 0;

	ODdecode = gf_odf_codec_new();
	if (!ODdecode) return 0;
	e = gf_odf_codec_set_au(ODdecode, sample->data, sample->dataLength);
	if (e) goto err_exit;
	e = gf_odf_codec_decode(ODdecode);
	if (e) goto err_exit;

	while (1) {
		GF_List *esd_list = NULL;
		com = gf_odf_codec_get_com(ODdecode);
		if (!com) break;
		if (com->tag != GF_ODF_OD_UPDATE_TAG) continue;
		odU = (GF_ODUpdate *) com;

		i=0;
		while ((desc = (GF_Descriptor*)gf_list_enum(odU->objectDescriptors, &i))) {
			switch (desc->tag) {
			case GF_ODF_OD_TAG:
			case GF_ODF_IOD_TAG:
				esd_list = ((GF_ObjectDescriptor *)desc)->ESDescriptors;
				break;
			default:
				continue;
			}
			j=0;
			while ((esd = (GF_ESD*)gf_list_enum( esd_list, &j))) {
				if (esd->ESID==track_id) {
					the_od_id = ((GF_IsomObjectDescriptor*)desc)->objectDescriptorID;
					break;
				}
			}
			if (the_od_id) break;
		}
		gf_odf_com_del((GF_ODCom **)&odU);
		if (the_od_id) break;
	}

err_exit:
	gf_odf_codec_del(ODdecode);
	return the_od_id; //still 0 if error, no need to check for e
}