static NTSTATUS marshall_stream_info(unsigned int num_streams,
				     const struct stream_struct *streams,
				     char *data,
				     unsigned int max_data_bytes,
				     unsigned int *data_size)
{
	unsigned int i;
	unsigned int ofs = 0;

	if (max_data_bytes < 32) {
		return NT_STATUS_INFO_LENGTH_MISMATCH;
	}

	for (i = 0; i < num_streams; i++) {
		unsigned int next_offset;
		size_t namelen;
		smb_ucs2_t *namebuf;

		if (!push_ucs2_talloc(talloc_tos(), &namebuf,
				      streams[i].name, &namelen) ||
		    namelen <= 2)
		{
			return NT_STATUS_INVALID_PARAMETER;
		}

		/*
		 * name_buf is now null-terminated, we need to marshall as not
		 * terminated
		 */

		namelen -= 2;

		/*
		 * We cannot overflow ...
		 */
		if ((ofs + 24 + namelen) > max_data_bytes) {
			DEBUG(10, ("refusing to overflow reply at stream %u\n",
				i));
			TALLOC_FREE(namebuf);
			return STATUS_BUFFER_OVERFLOW;
		}

		SIVAL(data, ofs+4, namelen);
		SOFF_T(data, ofs+8, streams[i].size);
		SOFF_T(data, ofs+16, streams[i].alloc_size);
		memcpy(data+ofs+24, namebuf, namelen);
		TALLOC_FREE(namebuf);

		next_offset = ofs + 24 + namelen;

		if (i == num_streams-1) {
			SIVAL(data, ofs, 0);
		}
		else {
			unsigned int align = ndr_align_size(next_offset, 8);

			if ((next_offset + align) > max_data_bytes) {
				DEBUG(10, ("refusing to overflow align "
					"reply at stream %u\n",
					i));
				TALLOC_FREE(namebuf);
				return STATUS_BUFFER_OVERFLOW;
			}

			memset(data+next_offset, 0, align);
			next_offset += align;

			SIVAL(data, ofs, next_offset - ofs);
			ofs = next_offset;
		}

		ofs = next_offset;
	}

	DEBUG(10, ("max_data: %u, data_size: %u\n", max_data_bytes, ofs));

	*data_size = ofs;

	return NT_STATUS_OK;
}