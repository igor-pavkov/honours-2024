dirserv_get_networkstatus_v2_fingerprints(smartlist_t *result,
                                          const char *key)
{
  tor_assert(result);

  if (!cached_v2_networkstatus)
    cached_v2_networkstatus = digestmap_new();

  if (should_generate_v2_networkstatus())
    generate_v2_networkstatus_opinion();

  if (!strcmp(key,"authority")) {
    if (authdir_mode_v2(get_options())) {
      routerinfo_t *me = router_get_my_routerinfo();
      if (me)
        smartlist_add(result,
                      tor_memdup(me->cache_info.identity_digest, DIGEST_LEN));
    }
  } else if (!strcmp(key, "all")) {
    if (digestmap_size(cached_v2_networkstatus)) {
      digestmap_iter_t *iter;
      iter = digestmap_iter_init(cached_v2_networkstatus);
      while (!digestmap_iter_done(iter)) {
        const char *ident;
        void *val;
        digestmap_iter_get(iter, &ident, &val);
        smartlist_add(result, tor_memdup(ident, DIGEST_LEN));
        iter = digestmap_iter_next(cached_v2_networkstatus, iter);
      }
    } else {
      SMARTLIST_FOREACH(router_get_trusted_dir_servers(),
                  trusted_dir_server_t *, ds,
                  if (ds->type & V2_AUTHORITY)
                    smartlist_add(result, tor_memdup(ds->digest, DIGEST_LEN)));
    }
    smartlist_sort_digests(result);
    if (smartlist_len(result) == 0)
      log_info(LD_DIRSERV,
               "Client requested 'all' network status objects; we have none.");
  } else if (!strcmpstart(key, "fp/")) {
    dir_split_resource_into_fingerprints(key+3, result, NULL,
                                         DSR_HEX|DSR_SORT_UNIQ);
  }
}