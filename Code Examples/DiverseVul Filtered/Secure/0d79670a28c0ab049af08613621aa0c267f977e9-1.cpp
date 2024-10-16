void CliprdrStream_Delete(CliprdrStream* instance)
{
	if (instance)
	{
		free(instance->iStream.lpVtbl);
		free(instance);
	}
}