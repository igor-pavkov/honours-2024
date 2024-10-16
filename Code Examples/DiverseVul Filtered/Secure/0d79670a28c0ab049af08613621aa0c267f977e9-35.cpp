static UINT cliprdr_send_tempdir(wfClipboard* clipboard)
{
	CLIPRDR_TEMP_DIRECTORY tempDirectory;

	if (!clipboard)
		return -1;

	if (GetEnvironmentVariableA("TEMP", tempDirectory.szTempDir, sizeof(tempDirectory.szTempDir)) ==
	    0)
		return -1;

	return clipboard->context->TempDirectory(clipboard->context, &tempDirectory);
}