static uint16_t buffer_read_le_u16(STATE_PARAM_ONLY)
{
	uint16_t res;
#if BB_LITTLE_ENDIAN
	move_from_unaligned16(res, &bytebuffer[bytebuffer_offset]);
#else
	res = bytebuffer[bytebuffer_offset];
	res |= bytebuffer[bytebuffer_offset + 1] << 8;
#endif
	bytebuffer_offset += 2;
	return res;
}