static CURLcode parse_proxy(struct SessionHandle *data,
                            struct connectdata *conn, char *proxy)
{
  char *prox_portno;
  char *endofprot;

  /* We use 'proxyptr' to point to the proxy name from now on... */
  char *proxyptr;
  char *portptr;
  char *atsign;

  /* We do the proxy host string parsing here. We want the host name and the
   * port name. Accept a protocol:// prefix
   */

  /* Parse the protocol part if present */
  endofprot = strstr(proxy, "://");
  if(endofprot) {
    proxyptr = endofprot+3;
    if(checkprefix("socks5h", proxy))
      conn->proxytype = CURLPROXY_SOCKS5_HOSTNAME;
    else if(checkprefix("socks5", proxy))
      conn->proxytype = CURLPROXY_SOCKS5;
    else if(checkprefix("socks4a", proxy))
      conn->proxytype = CURLPROXY_SOCKS4A;
    else if(checkprefix("socks4", proxy) || checkprefix("socks", proxy))
      conn->proxytype = CURLPROXY_SOCKS4;
    /* Any other xxx:// : change to http proxy */
  }
  else
    proxyptr = proxy; /* No xxx:// head: It's a HTTP proxy */

  /* Is there a username and password given in this proxy url? */
  atsign = strchr(proxyptr, '@');
  if(atsign) {
    char *proxyuser = NULL;
    char *proxypasswd = NULL;
    CURLcode result =
      parse_login_details(proxyptr, atsign - proxyptr,
                          &proxyuser, &proxypasswd, NULL);
    if(!result) {
      /* found user and password, rip them out.  note that we are
         unescaping them, as there is otherwise no way to have a
         username or password with reserved characters like ':' in
         them. */
      Curl_safefree(conn->proxyuser);
      if(proxyuser && strlen(proxyuser) < MAX_CURL_USER_LENGTH)
        conn->proxyuser = curl_easy_unescape(data, proxyuser, 0, NULL);
      else
        conn->proxyuser = strdup("");

      if(!conn->proxyuser)
        result = CURLE_OUT_OF_MEMORY;
      else {
        Curl_safefree(conn->proxypasswd);
        if(proxypasswd && strlen(proxypasswd) < MAX_CURL_PASSWORD_LENGTH)
          conn->proxypasswd = curl_easy_unescape(data, proxypasswd, 0, NULL);
        else
          conn->proxypasswd = strdup("");

        if(!conn->proxypasswd)
          result = CURLE_OUT_OF_MEMORY;
      }

      if(!result) {
        conn->bits.proxy_user_passwd = TRUE; /* enable it */
        atsign++; /* the right side of the @-letter */

        proxyptr = atsign; /* now use this instead */
      }
    }

    free(proxyuser);
    free(proxypasswd);

    if(result)
      return result;
  }

  /* start scanning for port number at this point */
  portptr = proxyptr;

  /* detect and extract RFC6874-style IPv6-addresses */
  if(*proxyptr == '[') {
    char *ptr = ++proxyptr; /* advance beyond the initial bracket */
    while(*ptr && (ISXDIGIT(*ptr) || (*ptr == ':') || (*ptr == '.')))
      ptr++;
    if(*ptr == '%') {
      /* There might be a zone identifier */
      if(strncmp("%25", ptr, 3))
        infof(data, "Please URL encode %% as %%25, see RFC 6874.\n");
      ptr++;
      /* Allow unresered characters as defined in RFC 3986 */
      while(*ptr && (ISALPHA(*ptr) || ISXDIGIT(*ptr) || (*ptr == '-') ||
                     (*ptr == '.') || (*ptr == '_') || (*ptr == '~')))
        ptr++;
    }
    if(*ptr == ']')
      /* yeps, it ended nicely with a bracket as well */
      *ptr++ = 0;
    else
      infof(data, "Invalid IPv6 address format\n");
    portptr = ptr;
    /* Note that if this didn't end with a bracket, we still advanced the
     * proxyptr first, but I can't see anything wrong with that as no host
     * name nor a numeric can legally start with a bracket.
     */
  }

  /* Get port number off proxy.server.com:1080 */
  prox_portno = strchr(portptr, ':');
  if(prox_portno) {
    *prox_portno = 0x0; /* cut off number from host name */
    prox_portno ++;
    /* now set the local port number */
    conn->port = strtol(prox_portno, NULL, 10);
  }
  else {
    if(proxyptr[0]=='/')
      /* If the first character in the proxy string is a slash, fail
         immediately. The following code will otherwise clear the string which
         will lead to code running as if no proxy was set! */
      return CURLE_COULDNT_RESOLVE_PROXY;

    /* without a port number after the host name, some people seem to use
       a slash so we strip everything from the first slash */
    atsign = strchr(proxyptr, '/');
    if(atsign)
      *atsign = 0x0; /* cut off path part from host name */

    if(data->set.proxyport)
      /* None given in the proxy string, then get the default one if it is
         given */
      conn->port = data->set.proxyport;
  }

  /* now, clone the cleaned proxy host name */
  conn->proxy.rawalloc = strdup(proxyptr);
  conn->proxy.name = conn->proxy.rawalloc;

  if(!conn->proxy.rawalloc)
    return CURLE_OUT_OF_MEMORY;

  return CURLE_OK;
}