static void php_free_pcre_cache(void *data) /* {{{ */
{
	pcre_cache_entry *pce = (pcre_cache_entry *) data;
	if (!pce) return;
	pefree(pce->re, 1);
	if (pce->extra) pefree(pce->extra, 1);
#if HAVE_SETLOCALE
	if ((void*)pce->tables) pefree((void*)pce->tables, 1);
	pefree(pce->locale, 1);
#endif
}