static ut64 p_ptr(ut64 decorated_addr, RKernelCacheObj *obj) {
	RParsedPointer ptr;
	r_parse_pointer (&ptr, decorated_addr, obj);
	return ptr.address;
}