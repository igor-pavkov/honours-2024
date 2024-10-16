static uint32_t buffer_read_le_u32(STATE_PARAM_ONLY)
{
	uint32_t res;
#if BB_LITTLE_ENDIAN
	move_from_unaligned32(res, &bytebuffer[bytebuffer_offset]);
#else
	res = bytebuffer[bytebuffer_offset];
	res |= bytebuffer[bytebuffer_offset + 1] << 8;
	res |= bytebuffer[bytebuffer_offset + 2] << 16;
	res |= bytebuffer[bytebuffer_offset + 3] << 24;
#endif
	bytebuffer_offset += 4;
	return res;
}