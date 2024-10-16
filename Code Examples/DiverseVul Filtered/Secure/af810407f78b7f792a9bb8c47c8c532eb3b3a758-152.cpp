static my_bool xacommit_handlerton(THD *unused1, plugin_ref plugin,
                                   void *arg)
{
  handlerton *hton= plugin_hton(plugin);
  if (hton->state == SHOW_OPTION_YES && hton->recover)
  {
    hton->commit_by_xid(hton, ((struct xahton_st *)arg)->xid);
    ((struct xahton_st *)arg)->result= 0;
  }
  return FALSE;
}