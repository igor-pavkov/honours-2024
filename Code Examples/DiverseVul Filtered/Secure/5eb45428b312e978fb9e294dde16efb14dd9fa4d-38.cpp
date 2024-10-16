bool fhas_fs_type(int fd, fs_type_magic magic_val)
{
	int ret;
	struct statfs sb;

	ret = fstatfs(fd, &sb);
	if (ret < 0)
		return false;

	return is_fs_type(&sb, magic_val);
}