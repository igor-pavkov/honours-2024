dirserv_have_any_microdesc(const smartlist_t *fps)
{
  microdesc_cache_t *cache = get_microdesc_cache();
  SMARTLIST_FOREACH(fps, const char *, fp,
                    if (microdesc_cache_lookup_by_digest256(cache, fp))
                      return 1);
  return 0;
}