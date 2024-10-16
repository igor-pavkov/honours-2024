void web_server_destroy(void)
{
	if (bWebServerState == WEB_SERVER_ENABLED) {
		membuffer_destroy(&gDocumentRootDir);
		alias_release(&gAliasDoc);

		ithread_mutex_lock(&gWebMutex);
		memset(&gAliasDoc, 0, sizeof(struct xml_alias_t));
		ithread_mutex_unlock(&gWebMutex);

		ithread_mutex_destroy(&gWebMutex);
		bWebServerState = WEB_SERVER_DISABLED;
	}
}