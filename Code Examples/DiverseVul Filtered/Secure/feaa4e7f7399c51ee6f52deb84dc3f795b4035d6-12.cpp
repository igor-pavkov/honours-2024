static RList *resolve_syscalls(RKernelCacheObj *obj, ut64 enosys_addr) {
	struct section_t *sections = NULL;
	if (!(sections = MACH0_(get_sections) (obj->mach0))) {
		return NULL;
	}

	RList *syscalls = NULL;
	RSyscall *syscall = NULL;
	ut8 *data_const = NULL;
	ut64 data_const_offset = 0, data_const_size = 0, data_const_vaddr = 0;
	int i = 0;
	for (; !sections[i].last; i++) {
		if (strstr (sections[i].name, "__DATA_CONST.__const")) {
			data_const_offset = sections[i].offset;
			data_const_size = sections[i].size;
			data_const_vaddr = K_PPTR (sections[i].addr);
			break;
		}
	}

	if (!data_const_offset || !data_const_size || !data_const_vaddr) {
		goto beach;
	}

	data_const = malloc (data_const_size);
	if (!data_const) {
		goto beach;
	}
	if (r_buf_read_at (obj->cache_buf, data_const_offset, data_const, data_const_size) < data_const_size) {
		goto beach;
	}

	ut8 *cursor = data_const;
	ut8 *end = data_const + data_const_size;
	while (cursor < end) {
		ut64 test = r_read_le64 (cursor);
		if (test == enosys_addr) {
			break;
		}
		cursor += 8;
	}

	if (cursor >= end) {
		goto beach;
	}

	cursor -= 24;
	while (cursor >= data_const) {
		ut64 addr = r_read_le64 (cursor);
		ut64 x = r_read_le64 (cursor + 8);
		ut64 y = r_read_le64 (cursor + 16);

		if (IS_KERNEL_ADDR (addr) &&
			(x == 0 || IS_KERNEL_ADDR (x)) &&
			(y != 0 && !IS_KERNEL_ADDR (y))) {
			cursor -= 24;
			continue;
		}

		cursor += 24;
		break;
	}

	if (cursor < data_const) {
		goto beach;
	}

	syscalls = r_list_newf (r_bin_symbol_free);
	if (!syscalls) {
		goto beach;
	}

	syscall = r_syscall_new ();
	if (!syscall) {
		goto beach;
	}
	r_syscall_setup (syscall, "arm", 64, NULL, "ios");
	if (!syscall->db) {
		r_syscall_free (syscall);
		goto beach;
	}

	ut64 sysent_vaddr = cursor - data_const + data_const_vaddr;

	RBinSymbol *sym = R_NEW0 (RBinSymbol);
	if (!sym) {
		goto beach;
	}

	sym->name = r_str_newf ("sysent");
	sym->vaddr = sysent_vaddr;
	sym->paddr = cursor - data_const + data_const_offset;
	sym->size = 0;
	sym->forwarder = "NONE";
	sym->bind = "GLOBAL";
	sym->type = "OBJECT";
	r_list_append (syscalls, sym);

	i = 1;
	cursor += 24;
	int num_syscalls = sdb_count (syscall->db);
	while (cursor < end && i < num_syscalls) {
		ut64 addr = r_read_le64 (cursor);
		RSyscallItem *item = r_syscall_get (syscall, i, 0x80);
		if (item && item->name) {
			RBinSymbol *sym = R_NEW0 (RBinSymbol);
			if (!sym) {
				r_syscall_item_free (item);
				goto beach;
			}

			sym->name = r_str_newf ("syscall.%d.%s", i, item->name);
			sym->vaddr = addr;
			sym->paddr = addr;
			sym->size = 0;
			sym->forwarder = "NONE";
			sym->bind = "GLOBAL";
			sym->type = "FUNC";
			r_list_append (syscalls, sym);
		}
		r_syscall_item_free (item);
		cursor += 24;
		i++;
	}

	r_syscall_free (syscall);
	R_FREE (data_const);
	R_FREE (sections);
	return syscalls;

beach:
	r_syscall_free (syscall);
	if (syscalls) {
		r_list_free (syscalls);
	}
	R_FREE (data_const);
	R_FREE (sections);
	return NULL;
}