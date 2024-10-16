static int set_hidden_flag(const wchar_t *path, int set)
{
	DWORD original = GetFileAttributesW(path), modified;
	if (set)
		modified = original | FILE_ATTRIBUTE_HIDDEN;
	else
		modified = original & ~FILE_ATTRIBUTE_HIDDEN;
	if (original == modified || SetFileAttributesW(path, modified))
		return 0;
	errno = err_win_to_posix(GetLastError());
	return -1;
}