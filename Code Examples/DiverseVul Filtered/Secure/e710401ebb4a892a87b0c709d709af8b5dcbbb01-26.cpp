R_API int r_egg_setup(REgg *egg, const char *arch, int bits, int endian, const char *os) {
	const char *asmcpu = NULL; // TODO
	egg->remit = NULL;

	egg->os = os? r_str_hash (os): R_EGG_OS_DEFAULT;
	//eprintf ("%s -> %x (linux=%x) (darwin=%x)\n", os, egg->os, R_EGG_OS_LINUX, R_EGG_OS_DARWIN);
	// TODO: setup egg->arch for all archs
	if (!strcmp (arch, "x86")) {
		egg->arch = R_SYS_ARCH_X86;
		switch (bits) {
		case 32:
			r_syscall_setup (egg->syscall, arch, bits, asmcpu, os);
			egg->remit = &emit_x86;
			egg->bits = bits;
			break;
		case 64:
			r_syscall_setup (egg->syscall, arch, bits, asmcpu, os);
			egg->remit = &emit_x64;
			egg->bits = bits;
			break;
		}
	} else if (!strcmp (arch, "arm")) {
		egg->arch = R_SYS_ARCH_ARM;
		switch (bits) {
		case 16:
		case 32:
			r_syscall_setup (egg->syscall, arch, bits, asmcpu, os);
			egg->remit = &emit_arm;
			egg->bits = bits;
			egg->endian = endian;
			break;
		}
	} else if (!strcmp (arch, "trace")) {
		//r_syscall_setup (egg->syscall, arch, os, bits);
		egg->remit = &emit_trace;
		egg->bits = bits;
		egg->endian = endian;
	}
	return 0;
}