static INLINE BOOL file_transferring(wfClipboard* clipboard)
{
	return get_local_format_id_by_name(clipboard, CFSTR_FILEDESCRIPTORW) ? TRUE : FALSE;
}