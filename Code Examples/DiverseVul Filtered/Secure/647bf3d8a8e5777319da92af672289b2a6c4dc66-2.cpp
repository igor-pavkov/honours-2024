static void lookup_iova(
	struct rxe_mem	*mem,
	u64			iova,
	int			*m_out,
	int			*n_out,
	size_t			*offset_out)
{
	size_t			offset = iova - mem->iova + mem->offset;
	int			map_index;
	int			buf_index;
	u64			length;

	if (likely(mem->page_shift)) {
		*offset_out = offset & mem->page_mask;
		offset >>= mem->page_shift;
		*n_out = offset & mem->map_mask;
		*m_out = offset >> mem->map_shift;
	} else {
		map_index = 0;
		buf_index = 0;

		length = mem->map[map_index]->buf[buf_index].size;

		while (offset >= length) {
			offset -= length;
			buf_index++;

			if (buf_index == RXE_BUF_PER_MAP) {
				map_index++;
				buf_index = 0;
			}
			length = mem->map[map_index]->buf[buf_index].size;
		}

		*m_out = map_index;
		*n_out = buf_index;
		*offset_out = offset;
	}
}