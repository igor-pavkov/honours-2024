static inline int is_ntfs_dot_str(const char *name, const char *dotgit_name,
				  const char *dotgit_ntfs_shortname_prefix)
{
	return is_ntfs_dot_generic(name, dotgit_name, strlen(dotgit_name),
				   dotgit_ntfs_shortname_prefix);
}