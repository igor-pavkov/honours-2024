static CURLcode parseurlandfillconn(struct Curl_easy *data,
                                    struct connectdata *conn)
{
  CURLcode result;
  CURLU *uh;
  CURLUcode uc;
  char *hostname;

  Curl_up_free(data); /* cleanup previous leftovers first */

  /* parse the URL */
  uh = data->state.uh = curl_url();
  if(!uh)
    return CURLE_OUT_OF_MEMORY;

  if(data->set.str[STRING_DEFAULT_PROTOCOL] &&
     !Curl_is_absolute_url(data->change.url, NULL, MAX_SCHEME_LEN)) {
    char *url;
    if(data->change.url_alloc)
      free(data->change.url);
    url = aprintf("%s://%s", data->set.str[STRING_DEFAULT_PROTOCOL],
                  data->change.url);
    if(!url)
      return CURLE_OUT_OF_MEMORY;
    data->change.url = url;
    data->change.url_alloc = TRUE;
  }

  uc = curl_url_set(uh, CURLUPART_URL, data->change.url,
                    CURLU_GUESS_SCHEME |
                    CURLU_NON_SUPPORT_SCHEME |
                    (data->set.disallow_username_in_url ?
                     CURLU_DISALLOW_USER : 0) |
                    (data->set.path_as_is ? CURLU_PATH_AS_IS : 0));
  if(uc)
    return Curl_uc_to_curlcode(uc);

  uc = curl_url_get(uh, CURLUPART_SCHEME, &data->state.up.scheme, 0);
  if(uc)
    return Curl_uc_to_curlcode(uc);

  result = findprotocol(data, conn, data->state.up.scheme);
  if(result)
    return result;

  uc = curl_url_get(uh, CURLUPART_USER, &data->state.up.user,
                    CURLU_URLDECODE);
  if(!uc) {
    conn->user = strdup(data->state.up.user);
    if(!conn->user)
      return CURLE_OUT_OF_MEMORY;
    conn->bits.user_passwd = TRUE;
  }
  else if(uc != CURLUE_NO_USER)
    return Curl_uc_to_curlcode(uc);

  uc = curl_url_get(uh, CURLUPART_PASSWORD, &data->state.up.password,
                    CURLU_URLDECODE);
  if(!uc) {
    conn->passwd = strdup(data->state.up.password);
    if(!conn->passwd)
      return CURLE_OUT_OF_MEMORY;
    conn->bits.user_passwd = TRUE;
  }
  else if(uc != CURLUE_NO_PASSWORD)
    return Curl_uc_to_curlcode(uc);

  uc = curl_url_get(uh, CURLUPART_OPTIONS, &data->state.up.options,
                    CURLU_URLDECODE);
  if(!uc) {
    conn->options = strdup(data->state.up.options);
    if(!conn->options)
      return CURLE_OUT_OF_MEMORY;
  }
  else if(uc != CURLUE_NO_OPTIONS)
    return Curl_uc_to_curlcode(uc);

  uc = curl_url_get(uh, CURLUPART_HOST, &data->state.up.hostname, 0);
  if(uc) {
    if(!strcasecompare("file", data->state.up.scheme))
      return CURLE_OUT_OF_MEMORY;
  }

  uc = curl_url_get(uh, CURLUPART_PATH, &data->state.up.path, 0);
  if(uc)
    return Curl_uc_to_curlcode(uc);

  uc = curl_url_get(uh, CURLUPART_PORT, &data->state.up.port,
                    CURLU_DEFAULT_PORT);
  if(uc) {
    if(!strcasecompare("file", data->state.up.scheme))
      return CURLE_OUT_OF_MEMORY;
  }
  else {
    unsigned long port = strtoul(data->state.up.port, NULL, 10);
    conn->remote_port = curlx_ultous(port);
  }

  (void)curl_url_get(uh, CURLUPART_QUERY, &data->state.up.query, 0);

  hostname = data->state.up.hostname;
  if(!hostname)
    /* this is for file:// transfers, get a dummy made */
    hostname = (char *)"";

  if(hostname[0] == '[') {
    /* This looks like an IPv6 address literal.  See if there is an address
       scope. */
    char *percent = strchr(++hostname, '%');
    conn->bits.ipv6_ip = TRUE;
    if(percent) {
      unsigned int identifier_offset = 3;
      char *endp;
      unsigned long scope;
      if(strncmp("%25", percent, 3) != 0) {
        infof(data,
              "Please URL encode %% as %%25, see RFC 6874.\n");
        identifier_offset = 1;
      }
      scope = strtoul(percent + identifier_offset, &endp, 10);
      if(*endp == ']') {
        /* The address scope was well formed.  Knock it out of the
           hostname. */
        memmove(percent, endp, strlen(endp) + 1);
        conn->scope_id = (unsigned int)scope;
      }
      else {
        /* Zone identifier is not numeric */
#if defined(HAVE_NET_IF_H) && defined(IFNAMSIZ) && defined(HAVE_IF_NAMETOINDEX)
        char ifname[IFNAMSIZ + 2];
        char *square_bracket;
        unsigned int scopeidx = 0;
        strncpy(ifname, percent + identifier_offset, IFNAMSIZ + 2);
        /* Ensure nullbyte termination */
        ifname[IFNAMSIZ + 1] = '\0';
        square_bracket = strchr(ifname, ']');
        if(square_bracket) {
          /* Remove ']' */
          *square_bracket = '\0';
          scopeidx = if_nametoindex(ifname);
          if(scopeidx == 0) {
            infof(data, "Invalid network interface: %s; %s\n", ifname,
                  strerror(errno));
          }
        }
        if(scopeidx > 0) {
          char *p = percent + identifier_offset + strlen(ifname);

          /* Remove zone identifier from hostname */
          memmove(percent, p, strlen(p) + 1);
          conn->scope_id = scopeidx;
        }
        else
#endif /* HAVE_NET_IF_H && IFNAMSIZ */
          infof(data, "Invalid IPv6 address format\n");
      }
    }
    percent = strchr(hostname, ']');
    if(percent)
      /* terminate IPv6 numerical at end bracket */
      *percent = 0;
  }

  /* make sure the connect struct gets its own copy of the host name */
  conn->host.rawalloc = strdup(hostname);
  if(!conn->host.rawalloc)
    return CURLE_OUT_OF_MEMORY;
  conn->host.name = conn->host.rawalloc;

  if(data->set.scope_id)
    /* Override any scope that was set above.  */
    conn->scope_id = data->set.scope_id;

  return CURLE_OK;
}