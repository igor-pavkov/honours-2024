void CliprdrDataObject_Delete(CliprdrDataObject* instance)
{
	if (instance)
	{
		free(instance->iDataObject.lpVtbl);
		free(instance->m_pFormatEtc);
		free(instance->m_pStgMedium);

		if (instance->m_pStream)
		{
			ULONG i;

			for (i = 0; i < instance->m_nStreams; i++)
				CliprdrStream_Release(instance->m_pStream[i]);

			free(instance->m_pStream);
		}

		free(instance);
	}
}