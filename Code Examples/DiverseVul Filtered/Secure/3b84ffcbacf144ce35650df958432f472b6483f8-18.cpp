Bool gf_cenc_validate_key_info(const u8 *key_info, u32 key_info_size)
{
	u32 i, n_keys, kpos, nb_missing = 19;
	if (!key_info|| (key_info_size<19))
		goto exit;

	n_keys = 1;
	if (key_info[0]) {
		n_keys = key_info[1];
		n_keys <<= 8;
		n_keys |= key_info[2];
	}
	kpos=3;
	for (i=0;i<n_keys; i++) {
		u8 iv_size;
		if (kpos + 17 > key_info_size) {
			nb_missing = kpos + 17 - key_info_size;
			goto exit;
		}
		iv_size = key_info[kpos];
		kpos += 17;
		if (!iv_size) {
			if (kpos + 1 > key_info_size) {
				nb_missing = kpos + 1  - key_info_size;
				goto exit;
			}
			iv_size = key_info[kpos];
			if (kpos + 1 + iv_size > key_info_size) {
				nb_missing = kpos + 1 + iv_size - key_info_size;
				goto exit;
			}
			kpos += 1 + iv_size;
		}
	}
	return GF_TRUE;

exit:
	GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("Invalid key info format, missing %d bytes\n", nb_missing));
	return GF_FALSE;
}