static ut64 r_ptr(ut8 *buf, RKernelCacheObj *obj) {
	ut64 decorated_addr = r_read_le64 (buf);
	return K_PPTR (decorated_addr);
}