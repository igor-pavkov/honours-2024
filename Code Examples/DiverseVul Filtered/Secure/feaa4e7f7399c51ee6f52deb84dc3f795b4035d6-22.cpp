static ut64 iterate_rebase_list(RBuffer *cache_buf, ut64 multiplier, ut64 start_offset, ROnRebaseFunc func, void *user_data) {
	ut8 bytes[8];
	ut64 cursor = start_offset;

	while (true) {
		if (r_buf_read_at (cache_buf, cursor, bytes, 8) < 8) {
			return UT64_MAX;
		}

		ut64 decorated_addr = r_read_le64 (bytes);

		if (func) {
			bool carry_on = func (cursor, decorated_addr, user_data);
			if (!carry_on) {
				break;
			}
		}

		ut64 delta = ((decorated_addr >> 51) & 0x7ff) * multiplier;
		if (delta == 0) {
			break;
		}
		cursor += delta;
	}

	return cursor;
}