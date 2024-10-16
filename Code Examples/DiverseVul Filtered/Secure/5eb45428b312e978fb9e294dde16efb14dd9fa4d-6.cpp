bool has_fs_type(const char *path, fs_type_magic magic_val)
{
	bool has_type;
	int ret;
	struct statfs sb;

	ret = statfs(path, &sb);
	if (ret < 0)
		return false;

	has_type = is_fs_type(&sb, magic_val);
	if (!has_type && magic_val == RAMFS_MAGIC)
		WARN("When the ramfs it a tmpfs statfs() might report tmpfs");

	return has_type;
}