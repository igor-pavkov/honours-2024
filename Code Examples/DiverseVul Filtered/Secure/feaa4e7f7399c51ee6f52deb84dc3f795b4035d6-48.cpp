static bool r_parse_pointer(RParsedPointer *ptr, ut64 decorated_addr, RKernelCacheObj *obj) {
	/*
	 * Logic taken from:
	 * https://github.com/Synacktiv/kernelcache-laundering/blob/master/ios12_kernel_cache_helper.py
	 */

	if ((decorated_addr & 0x4000000000000000LL) == 0 && obj->rebase_info) {
		if (decorated_addr & 0x8000000000000000LL) {
			ptr->address = obj->rebase_info->kernel_base + (decorated_addr & 0xFFFFFFFFLL);
		} else {
			ptr->address = ((decorated_addr << 13) & 0xFF00000000000000LL) | (decorated_addr & 0x7ffffffffffLL);
			if (decorated_addr & 0x40000000000LL) {
				ptr->address |= 0xfffc0000000000LL;
			}
		}
	} else {
		ptr->address = decorated_addr;
	}

	return true;
}