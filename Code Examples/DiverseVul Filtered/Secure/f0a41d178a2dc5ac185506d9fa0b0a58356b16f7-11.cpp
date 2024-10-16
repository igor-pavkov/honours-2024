GF_Err gf_isom_get_sample_cenc_info(GF_ISOFile *movie, u32 track, u32 sample_number, Bool *IsEncrypted, u8 *crypt_byte_block, u8 *skip_byte_block, const u8 **key_info, u32 *key_info_size)
{
	GF_TrackBox *trak = gf_isom_get_track_from_file(movie, track);
	GF_SampleEncryptionBox *senc = trak->sample_encryption;

	return gf_isom_get_sample_cenc_info_internal(trak, NULL, senc, sample_number, IsEncrypted, crypt_byte_block, skip_byte_block, key_info, key_info_size);
}