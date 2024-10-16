static void FindWin32CACert(struct Configurable *config,
                            const char *bundle_file)
{
  /* only check for cert file if "we" support SSL */
  if(curlinfo->features & CURL_VERSION_SSL) {
    DWORD buflen;
    char *ptr = NULL;
    char *retval = (char *) malloc(sizeof (TCHAR) * (MAX_PATH + 1));
    if (!retval)
      return;
    retval[0] = '\0';
    buflen = SearchPathA(NULL, bundle_file, NULL, MAX_PATH+2, retval, &ptr);
    if (buflen > 0) {
      GetStr(&config->cacert, retval);
    }
    free(retval);
  }
}