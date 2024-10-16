u8 key_info_get_iv_size(const u8 *key_info, u32 nb_keys, u32 idx, u8 *const_iv_size, const u8 **const_iv)
{
	u32 i, kpos=3;
	if (const_iv_size) *const_iv_size = 0;
	if (const_iv) *const_iv = NULL;

	for (i=0; i<nb_keys; i++) {
		u8 civ_size=0;
		const u8 *civ = NULL;
		u8 iv_size = key_info[kpos];
		kpos += 17;
		if (!iv_size) {
			civ_size = key_info[kpos];
			civ = key_info + kpos + 1;
			kpos += 1 + iv_size;
		}
		if (i+1==idx) {
			if (const_iv_size) *const_iv_size = civ_size;
			if (const_iv) *const_iv = civ;
			return iv_size;
		}
	}
	return 0;
}