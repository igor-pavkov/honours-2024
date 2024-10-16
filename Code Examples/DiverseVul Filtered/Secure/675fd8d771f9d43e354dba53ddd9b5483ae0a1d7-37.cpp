static bool shadow_copy2_find_slashes(TALLOC_CTX *mem_ctx, const char *str,
				      size_t **poffsets,
				      unsigned *pnum_offsets)
{
	unsigned num_offsets;
	size_t *offsets;
	const char *p;

	num_offsets = 0;

	p = str;
	while ((p = strchr(p, '/')) != NULL) {
		num_offsets += 1;
		p += 1;
	}

	offsets = talloc_array(mem_ctx, size_t, num_offsets);
	if (offsets == NULL) {
		return false;
	}

	p = str;
	num_offsets = 0;
	while ((p = strchr(p, '/')) != NULL) {
		offsets[num_offsets] = p-str;
		num_offsets += 1;
		p += 1;
	}

	*poffsets = offsets;
	*pnum_offsets = num_offsets;
	return true;
}