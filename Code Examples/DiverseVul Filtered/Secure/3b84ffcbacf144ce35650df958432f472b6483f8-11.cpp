GF_Err gf_isom_cenc_get_default_info(GF_ISOFile *the_file, u32 trackNumber, u32 sampleDescriptionIndex, u32 *container_type, Bool *default_IsEncrypted, u8 *crypt_byte_block, u8 *skip_byte_block, const u8 **key_info, u32 *key_info_size)
{
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	gf_isom_cenc_get_default_info_internal(trak, sampleDescriptionIndex, container_type, default_IsEncrypted, crypt_byte_block, skip_byte_block, key_info, key_info_size);
	return GF_OK;
}