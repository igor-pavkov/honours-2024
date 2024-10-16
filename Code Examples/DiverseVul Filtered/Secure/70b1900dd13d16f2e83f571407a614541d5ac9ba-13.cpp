static void free_config_fields(struct Configurable *config)
{
  if(config->random_file)
    free(config->random_file);
  if(config->egd_file)
    free(config->egd_file);
  if(config->trace_dump)
    free(config->trace_dump);
  if(config->cipher_list)
    free(config->cipher_list);
  if(config->userpwd)
    free(config->userpwd);
  if(config->postfields)
    free(config->postfields);
  if(config->proxy)
    free(config->proxy);
  if(config->proxyuserpwd)
    free(config->proxyuserpwd);
  if(config->cookie)
    free(config->cookie);
  if(config->cookiefile)
    free(config->cookiefile);
  if(config->krb4level)
    free(config->krb4level);
  if(config->headerfile)
    free(config->headerfile);
  if(config->ftpport)
    free(config->ftpport);
  if(config->range)
    free(config->range);
  if(config->customrequest)
    free(config->customrequest);
  if(config->writeout)
    free(config->writeout);
  if(config->httppost)
    curl_formfree(config->httppost);
  if (config->cert)
    free(config->cert);
  if(config->cacert)
    free(config->cacert);
  if (config->cert_type)
    free(config->cert_type);
  if(config->capath)
    free(config->capath);
  if(config->cookiejar)
    free(config->cookiejar);
  if(config->ftp_account)
    free(config->ftp_account);
  if(config->ftp_alternative_to_user)
    free(config->ftp_alternative_to_user);
  if(config->iface)
    free(config->iface);
  if(config->socksproxy)
    free(config->socksproxy);
  if(config->libcurl)
    free(config->libcurl);
  if (config->key_passwd)
    free(config->key_passwd);
  if (config->key)
    free(config->key);
  if (config->key_type)
    free(config->key_type);
  if (config->pubkey)
    free(config->pubkey);
  if (config->referer)
    free(config->referer);

  curl_slist_free_all(config->quote); /* checks for config->quote == NULL */
  curl_slist_free_all(config->prequote);
  curl_slist_free_all(config->postquote);
  curl_slist_free_all(config->headers);
  curl_slist_free_all(config->telnet_options);
}