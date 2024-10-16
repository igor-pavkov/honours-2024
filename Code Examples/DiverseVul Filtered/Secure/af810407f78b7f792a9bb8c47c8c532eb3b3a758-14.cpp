static my_bool plugin_disable_internal_writes(THD *, plugin_ref plugin,
                                              void *disable)
{
  if (void(*diw)(bool)= plugin_hton(plugin)->disable_internal_writes)
    diw(*static_cast<bool*>(disable));
  return FALSE;
}