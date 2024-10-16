CURLcode Curl_open(struct SessionHandle **curl)
{
  CURLcode result;
  struct SessionHandle *data;

  /* Very simple start-up: alloc the struct, init it with zeroes and return */
  data = calloc(1, sizeof(struct SessionHandle));
  if(!data) {
    /* this is a very serious error */
    DEBUGF(fprintf(stderr, "Error: calloc of SessionHandle failed\n"));
    return CURLE_OUT_OF_MEMORY;
  }

  data->magic = CURLEASY_MAGIC_NUMBER;

  result = Curl_resolver_init(&data->state.resolver);
  if(result) {
    DEBUGF(fprintf(stderr, "Error: resolver_init failed\n"));
    free(data);
    return result;
  }

  /* We do some initial setup here, all those fields that can't be just 0 */

  data->state.headerbuff = malloc(HEADERSIZE);
  if(!data->state.headerbuff) {
    DEBUGF(fprintf(stderr, "Error: malloc of headerbuff failed\n"));
    result = CURLE_OUT_OF_MEMORY;
  }
  else {
    result = Curl_init_userdefined(&data->set);

    data->state.headersize=HEADERSIZE;

    Curl_convert_init(data);

    /* most recent connection is not yet defined */
    data->state.lastconnect = NULL;

    data->progress.flags |= PGRS_HIDE;
    data->state.current_speed = -1; /* init to negative == impossible */

    data->wildcard.state = CURLWC_INIT;
    data->wildcard.filelist = NULL;
    data->set.fnmatch = ZERO_NULL;
    data->set.maxconnects = DEFAULT_CONNCACHE_SIZE; /* for easy handles */
  }

  if(result) {
    Curl_resolver_cleanup(data->state.resolver);
    free(data->state.headerbuff);
    Curl_freeset(data);
    free(data);
    data = NULL;
  }
  else
    *curl = data;

  return result;
}