static BOOL wf_create_file_obj(wfClipboard* clipboard, IDataObject** ppDataObject)
{
	FORMATETC fmtetc[2];
	STGMEDIUM stgmeds[2];

	if (!ppDataObject)
		return FALSE;

	fmtetc[0].cfFormat = RegisterClipboardFormat(CFSTR_FILEDESCRIPTORW);
	fmtetc[0].dwAspect = DVASPECT_CONTENT;
	fmtetc[0].lindex = 0;
	fmtetc[0].ptd = NULL;
	fmtetc[0].tymed = TYMED_HGLOBAL;
	stgmeds[0].tymed = TYMED_HGLOBAL;
	stgmeds[0].hGlobal = NULL;
	stgmeds[0].pUnkForRelease = NULL;
	fmtetc[1].cfFormat = RegisterClipboardFormat(CFSTR_FILECONTENTS);
	fmtetc[1].dwAspect = DVASPECT_CONTENT;
	fmtetc[1].lindex = 0;
	fmtetc[1].ptd = NULL;
	fmtetc[1].tymed = TYMED_ISTREAM;
	stgmeds[1].tymed = TYMED_ISTREAM;
	stgmeds[1].pstm = NULL;
	stgmeds[1].pUnkForRelease = NULL;
	*ppDataObject = (IDataObject*)CliprdrDataObject_New(fmtetc, stgmeds, 2, clipboard);
	return (*ppDataObject) ? TRUE : FALSE;
}