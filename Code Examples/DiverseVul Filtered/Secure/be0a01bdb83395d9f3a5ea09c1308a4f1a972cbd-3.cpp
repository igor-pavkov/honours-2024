int web_server_init()
{
	int ret = 0;

	if (bWebServerState == WEB_SERVER_DISABLED) {
		/* decode media list */
		media_list_init();
		membuffer_init(&gDocumentRootDir);
		glob_alias_init();
		pVirtualDirList = NULL;

		/* Initialize callbacks */
		virtualDirCallback.get_info = NULL;
		virtualDirCallback.open = NULL;
		virtualDirCallback.read = NULL;
		virtualDirCallback.write = NULL;
		virtualDirCallback.seek = NULL;
		virtualDirCallback.close = NULL;

		if (ithread_mutex_init(&gWebMutex, NULL) == -1)
			ret = UPNP_E_OUTOF_MEMORY;
		else
			bWebServerState = WEB_SERVER_ENABLED;
	}

	return ret;
}