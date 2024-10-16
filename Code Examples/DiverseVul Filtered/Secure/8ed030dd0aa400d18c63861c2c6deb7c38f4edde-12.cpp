bool ccid_support_check(u8 const *ccid_array, u8 array_len)
{
	while (array_len > 0)
		if (ccid_by_number(ccid_array[--array_len]) == NULL)
			return false;
	return true;
}