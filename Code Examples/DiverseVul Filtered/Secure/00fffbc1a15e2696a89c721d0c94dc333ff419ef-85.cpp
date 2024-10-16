dirserv_get_networkstatus_v2(smartlist_t *result,
                             const char *key)
{
  cached_dir_t *cached;
  smartlist_t *fingerprints = smartlist_create();
  tor_assert(result);

  if (!cached_v2_networkstatus)
    cached_v2_networkstatus = digestmap_new();

  dirserv_get_networkstatus_v2_fingerprints(fingerprints, key);
  SMARTLIST_FOREACH(fingerprints, const char *, fp,
    {
      if (router_digest_is_me(fp) && should_generate_v2_networkstatus())
        generate_v2_networkstatus_opinion();
      cached = digestmap_get(cached_v2_networkstatus, fp);
      if (cached) {
        smartlist_add(result, cached);
      } else {
        char hexbuf[HEX_DIGEST_LEN+1];
        base16_encode(hexbuf, sizeof(hexbuf), fp, DIGEST_LEN);
        log_info(LD_DIRSERV, "Don't know about any network status with "
                 "fingerprint '%s'", hexbuf);
      }
    });
  SMARTLIST_FOREACH(fingerprints, char *, cp, tor_free(cp));
  smartlist_free(fingerprints);
}