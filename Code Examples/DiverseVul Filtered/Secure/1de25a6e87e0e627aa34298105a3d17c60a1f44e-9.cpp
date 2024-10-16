static int check_header_gzip(STATE_PARAM transformer_state_t *xstate)
{
	union {
		unsigned char raw[8];
		struct {
			uint8_t gz_method;
			uint8_t flags;
			uint32_t mtime;
			uint8_t xtra_flags_UNUSED;
			uint8_t os_flags_UNUSED;
		} PACKED formatted;
	} header;

	BUILD_BUG_ON(sizeof(header) != 8);

	/*
	 * Rewind bytebuffer. We use the beginning because the header has 8
	 * bytes, leaving enough for unwinding afterwards.
	 */
	bytebuffer_size -= bytebuffer_offset;
	memmove(bytebuffer, &bytebuffer[bytebuffer_offset], bytebuffer_size);
	bytebuffer_offset = 0;

	if (!top_up(PASS_STATE 8))
		return 0;
	memcpy(header.raw, &bytebuffer[bytebuffer_offset], 8);
	bytebuffer_offset += 8;

	/* Check the compression method */
	if (header.formatted.gz_method != 8) {
		return 0;
	}

	if (header.formatted.flags & 0x04) {
		/* bit 2 set: extra field present */
		unsigned extra_short;

		if (!top_up(PASS_STATE 2))
			return 0;
		extra_short = buffer_read_le_u16(PASS_STATE_ONLY);
		if (!top_up(PASS_STATE extra_short))
			return 0;
		/* Ignore extra field */
		bytebuffer_offset += extra_short;
	}

	/* Discard original name and file comment if any */
	/* bit 3 set: original file name present */
	/* bit 4 set: file comment present */
	if (header.formatted.flags & 0x18) {
		while (1) {
			do {
				if (!top_up(PASS_STATE 1))
					return 0;
			} while (bytebuffer[bytebuffer_offset++] != 0);
			if ((header.formatted.flags & 0x18) != 0x18)
				break;
			header.formatted.flags &= ~0x18;
		}
	}

	xstate->mtime = SWAP_LE32(header.formatted.mtime);

	/* Read the header checksum */
	if (header.formatted.flags & 0x02) {
		if (!top_up(PASS_STATE 2))
			return 0;
		bytebuffer_offset += 2;
	}
	return 1;
}