static u16 unmap_class(u16 tclass)
{
	if (tclass < current_mapping_size)
		return current_mapping[tclass].value;

	return tclass;
}