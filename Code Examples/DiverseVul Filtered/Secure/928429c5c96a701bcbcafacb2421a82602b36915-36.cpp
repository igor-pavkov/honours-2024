GIT_INLINE(git_filemode_t) normalize_filemode(git_filemode_t filemode)
{
	/* Tree bits set, but it's not a commit */
	if (GIT_MODE_TYPE(filemode) == GIT_FILEMODE_TREE)
		return GIT_FILEMODE_TREE;

	/* If any of the x bits are set */
	if (GIT_PERMS_IS_EXEC(filemode))
		return GIT_FILEMODE_BLOB_EXECUTABLE;

	/* 16XXXX means commit */
	if (GIT_MODE_TYPE(filemode) == GIT_FILEMODE_COMMIT)
		return GIT_FILEMODE_COMMIT;

	/* 12XXXX means commit */
	if (GIT_MODE_TYPE(filemode) == GIT_FILEMODE_LINK)
		return GIT_FILEMODE_LINK;

	/* Otherwise, return a blob */
	return GIT_FILEMODE_BLOB;
}