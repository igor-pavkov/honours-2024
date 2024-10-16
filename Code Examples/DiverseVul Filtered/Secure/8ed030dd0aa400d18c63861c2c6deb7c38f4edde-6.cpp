int ccid_get_builtin_ccids(u8 **ccid_array, u8 *array_len)
{
	*ccid_array = kmalloc(ARRAY_SIZE(ccids), gfp_any());
	if (*ccid_array == NULL)
		return -ENOBUFS;

	for (*array_len = 0; *array_len < ARRAY_SIZE(ccids); *array_len += 1)
		(*ccid_array)[*array_len] = ccids[*array_len]->ccid_id;
	return 0;
}