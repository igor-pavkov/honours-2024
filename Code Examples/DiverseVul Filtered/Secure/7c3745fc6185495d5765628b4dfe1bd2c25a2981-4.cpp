int is_ntfs_dotgitignore(const char *name)
{
	return is_ntfs_dot_str(name, "gitignore", "gi250a");
}