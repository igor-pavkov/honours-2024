handlerton *ha_checktype(THD *thd, handlerton *hton, bool no_substitute)
{
  if (ha_storage_engine_is_enabled(hton))
    return hton;

  if (no_substitute)
    return NULL;
#ifdef WITH_WSREP
  (void)wsrep_after_rollback(thd, false);
#endif /* WITH_WSREP */

  return ha_default_handlerton(thd);
} /* ha_checktype */