static CliprdrStream* CliprdrStream_New(ULONG index, void* pData, const FILEDESCRIPTORW* dsc)
{
	IStream* iStream;
	BOOL success = FALSE;
	BOOL isDir = FALSE;
	CliprdrStream* instance;
	wfClipboard* clipboard = (wfClipboard*)pData;
	instance = (CliprdrStream*)calloc(1, sizeof(CliprdrStream));

	if (instance)
	{
		instance->m_Dsc = *dsc;
		iStream = &instance->iStream;
		iStream->lpVtbl = (IStreamVtbl*)calloc(1, sizeof(IStreamVtbl));

		if (iStream->lpVtbl)
		{
			iStream->lpVtbl->QueryInterface = CliprdrStream_QueryInterface;
			iStream->lpVtbl->AddRef = CliprdrStream_AddRef;
			iStream->lpVtbl->Release = CliprdrStream_Release;
			iStream->lpVtbl->Read = CliprdrStream_Read;
			iStream->lpVtbl->Write = CliprdrStream_Write;
			iStream->lpVtbl->Seek = CliprdrStream_Seek;
			iStream->lpVtbl->SetSize = CliprdrStream_SetSize;
			iStream->lpVtbl->CopyTo = CliprdrStream_CopyTo;
			iStream->lpVtbl->Commit = CliprdrStream_Commit;
			iStream->lpVtbl->Revert = CliprdrStream_Revert;
			iStream->lpVtbl->LockRegion = CliprdrStream_LockRegion;
			iStream->lpVtbl->UnlockRegion = CliprdrStream_UnlockRegion;
			iStream->lpVtbl->Stat = CliprdrStream_Stat;
			iStream->lpVtbl->Clone = CliprdrStream_Clone;
			instance->m_lRefCount = 1;
			instance->m_lIndex = index;
			instance->m_pData = pData;
			instance->m_lOffset.QuadPart = 0;

			if (instance->m_Dsc.dwFlags & FD_ATTRIBUTES)
			{
				if (instance->m_Dsc.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					isDir = TRUE;
			}

			if (((instance->m_Dsc.dwFlags & FD_FILESIZE) == 0) && !isDir)
			{
				/* get content size of this stream */
				if (cliprdr_send_request_filecontents(clipboard, (void*)instance,
				                                      instance->m_lIndex, FILECONTENTS_SIZE, 0, 0,
				                                      8) == CHANNEL_RC_OK)
				{
					success = TRUE;
				}

				instance->m_lSize.QuadPart = *((LONGLONG*)clipboard->req_fdata);
				free(clipboard->req_fdata);
			}
			else
				success = TRUE;
		}
	}

	if (!success)
	{
		CliprdrStream_Delete(instance);
		instance = NULL;
	}

	return instance;
}