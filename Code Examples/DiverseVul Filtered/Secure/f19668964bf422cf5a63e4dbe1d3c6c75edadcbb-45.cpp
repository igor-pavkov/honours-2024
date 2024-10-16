u8 key_info_get_iv_size(const u8 *key_info, u32 key_info_size, u32 idx, u8 *const_iv_size, const u8 **const_iv)
{
	u32 i=0, kpos=3;
	if (const_iv_size) *const_iv_size = 0;
	if (const_iv) *const_iv = NULL;

	if (!key_info || !key_info_size)
		return 0;

	while (1) {
		u8 civ_size=0;
		const u8 *civ = NULL;
		u8 iv_size = key_info[kpos];
		kpos += 17;

		if (!iv_size) {
			if (kpos>key_info_size)
				break;
			civ_size = key_info[kpos];
			civ = key_info + kpos + 1;
			kpos += 1 + civ_size;
		}

		if (kpos>key_info_size)
			break;

		if (i+1==idx) {
			if (const_iv_size) *const_iv_size = civ_size;
			if (const_iv) *const_iv = civ;
			return iv_size;
		}
		i++;
		if (kpos==key_info_size)
			break;
	}
	return 0;
}