static void wf_destroy_file_obj(IDataObject* instance)
{
	if (instance)
		IDataObject_Release(instance);
}