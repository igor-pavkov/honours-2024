static int aurule_avc_callback(u32 event)
{
	int err = 0;

	if (event == AVC_CALLBACK_RESET && aurule_callback)
		err = aurule_callback();
	return err;
}