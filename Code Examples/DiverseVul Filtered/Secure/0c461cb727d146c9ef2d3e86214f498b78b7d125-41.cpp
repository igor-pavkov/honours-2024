static int selinux_mmap_addr(unsigned long addr)
{
	int rc = 0;

	if (addr < CONFIG_LSM_MMAP_MIN_ADDR) {
		u32 sid = current_sid();
		rc = avc_has_perm(sid, sid, SECCLASS_MEMPROTECT,
				  MEMPROTECT__MMAP_ZERO, NULL);
	}

	return rc;
}