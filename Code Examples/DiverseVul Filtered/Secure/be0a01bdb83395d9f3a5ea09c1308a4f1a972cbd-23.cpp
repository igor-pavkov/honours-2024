static int isFileInVirtualDir(
	/*! [in] Directory path to be tested for virtual directory. */
	char *filePath)
{
	virtualDirList *pCurVirtualDir;
	size_t webDirLen;

	pCurVirtualDir = pVirtualDirList;
	while (pCurVirtualDir != NULL) {
		webDirLen = strlen(pCurVirtualDir->dirName);
		if (webDirLen) {
			if (pCurVirtualDir->dirName[webDirLen - 1] == '/') {
				if (strncmp(pCurVirtualDir->dirName, filePath,
						webDirLen) == 0)
					return !0;
			} else {
				if (strncmp(pCurVirtualDir->dirName, filePath,
						webDirLen) == 0 &&
				    (filePath[webDirLen] == '/' ||
				     filePath[webDirLen] == '\0' ||
				     filePath[webDirLen] == '?'))
					return !0;
			}
		}
		pCurVirtualDir = pCurVirtualDir->next;
	}

	return 0;
}