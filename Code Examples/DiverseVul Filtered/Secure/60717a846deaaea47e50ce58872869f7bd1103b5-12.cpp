s64 ntfs_get_attribute_value_length(const ATTR_RECORD *a)
{
	if (!a) {
		errno = EINVAL;
		return 0;
	}
	errno = 0;
	if (a->non_resident)
		return sle64_to_cpu(a->data_size);
	
	return (s64)le32_to_cpu(a->value_length);
}