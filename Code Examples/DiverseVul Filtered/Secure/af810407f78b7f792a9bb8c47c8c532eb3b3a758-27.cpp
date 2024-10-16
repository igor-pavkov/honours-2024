static my_bool checkpoint_state_handlerton(THD *unused1, plugin_ref plugin,
                                           void *disable)
{
  handlerton *hton= plugin_hton(plugin);
  if (hton->state == SHOW_OPTION_YES && hton->checkpoint_state)
    hton->checkpoint_state(hton, (int) *(bool*) disable);
  return FALSE;
}