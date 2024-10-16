GF_Err gf_isom_cenc_allocate_storage(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	if (trak->sample_encryption) return GF_OK;
	trak->sample_encryption = (GF_SampleEncryptionBox *)gf_isom_create_samp_enc_box(0, 0);
	//senc will be written and destroyed with the other boxes
	if (!trak->child_boxes) trak->child_boxes = gf_list_new();
	return gf_list_add(trak->child_boxes, trak->sample_encryption);
}