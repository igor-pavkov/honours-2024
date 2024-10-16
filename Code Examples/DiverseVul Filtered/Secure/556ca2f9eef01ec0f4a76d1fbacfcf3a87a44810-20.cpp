RZ_API bool rz_dyldcache_check_magic(const char *magic) {
	return !strcmp(magic, "dyld_v1   arm64") || !strcmp(magic, "dyld_v1  arm64e") || !strcmp(magic, "dyld_v1  x86_64") || !strcmp(magic, "dyld_v1 x86_64h");
}