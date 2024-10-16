void CliprdrEnumFORMATETC_Delete(CliprdrEnumFORMATETC* instance)
{
	LONG i;

	if (instance)
	{
		free(instance->iEnumFORMATETC.lpVtbl);

		if (instance->m_pFormatEtc)
		{
			for (i = 0; i < instance->m_nNumFormats; i++)
			{
				if (instance->m_pFormatEtc[i].ptd)
					CoTaskMemFree(instance->m_pFormatEtc[i].ptd);
			}

			free(instance->m_pFormatEtc);
		}

		free(instance);
	}
}