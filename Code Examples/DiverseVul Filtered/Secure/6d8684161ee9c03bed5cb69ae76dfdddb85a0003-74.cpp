int xwcstoutf(char *utf, const wchar_t *wcs, size_t utflen)
{
	if (!wcs || !utf || utflen < 1) {
		errno = EINVAL;
		return -1;
	}
	utflen = WideCharToMultiByte(CP_UTF8, 0, wcs, -1, utf, utflen, NULL, NULL);
	if (utflen)
		return utflen - 1;
	errno = ERANGE;
	return -1;
}