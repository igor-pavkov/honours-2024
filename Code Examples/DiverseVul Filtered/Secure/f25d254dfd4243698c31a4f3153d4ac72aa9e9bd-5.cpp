unpack_gz_stream(transformer_state_t *xstate)
{
	uint32_t v32;
	IF_DESKTOP(long long) int total, n;
	DECLARE_STATE;

#if !ENABLE_FEATURE_SEAMLESS_Z
	if (check_signature16(xstate, GZIP_MAGIC))
		return -1;
#else
	if (!xstate->signature_skipped) {
		uint16_t magic2;

		if (full_read(xstate->src_fd, &magic2, 2) != 2) {
 bad_magic:
			bb_simple_error_msg("invalid magic");
			return -1;
		}
		if (magic2 == COMPRESS_MAGIC) {
			xstate->signature_skipped = 2;
			return unpack_Z_stream(xstate);
		}
		if (magic2 != GZIP_MAGIC)
			goto bad_magic;
	}
#endif

	total = 0;

	ALLOC_STATE;
	to_read = -1;
//	bytebuffer_max = 0x8000;
	bytebuffer = xmalloc(bytebuffer_max);
	gunzip_src_fd = xstate->src_fd;

 again:
	if (!check_header_gzip(PASS_STATE xstate)) {
		bb_simple_error_msg("corrupted data");
		total = -1;
		goto ret;
	}

	n = inflate_unzip_internal(PASS_STATE xstate);
	if (n < 0) {
		total = -1;
		goto ret;
	}
	total += n;

	if (!top_up(PASS_STATE 8)) {
		bb_simple_error_msg("corrupted data");
		total = -1;
		goto ret;
	}

	/* Validate decompression - crc */
	v32 = buffer_read_le_u32(PASS_STATE_ONLY);
	if ((~gunzip_crc) != v32) {
		bb_simple_error_msg("crc error");
		total = -1;
		goto ret;
	}

	/* Validate decompression - size */
	v32 = buffer_read_le_u32(PASS_STATE_ONLY);
	if ((uint32_t)gunzip_bytes_out != v32) {
		bb_simple_error_msg("incorrect length");
		total = -1;
	}

	if (!top_up(PASS_STATE 2))
		goto ret; /* EOF */

	if (bytebuffer[bytebuffer_offset] == 0x1f
	 && bytebuffer[bytebuffer_offset + 1] == 0x8b
	) {
		bytebuffer_offset += 2;
		goto again;
	}
	/* GNU gzip says: */
	/*bb_error_msg("decompression OK, trailing garbage ignored");*/

 ret:
	free(bytebuffer);
	DEALLOC_STATE;
	return total;
}