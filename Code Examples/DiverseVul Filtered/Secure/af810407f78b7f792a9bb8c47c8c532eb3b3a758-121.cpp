static my_bool commit_checkpoint_request_handlerton(THD *unused1, plugin_ref plugin,
                                           void *data)
{
  st_commit_checkpoint_request *st= (st_commit_checkpoint_request *)data;
  handlerton *hton= plugin_hton(plugin);
  if (hton->state == SHOW_OPTION_YES && hton->commit_checkpoint_request)
  {
    void *cookie= st->cookie;
    if (st->pre_hook)
      (*st->pre_hook)(cookie);
    (*hton->commit_checkpoint_request)(hton, cookie);
  }
  return FALSE;
}