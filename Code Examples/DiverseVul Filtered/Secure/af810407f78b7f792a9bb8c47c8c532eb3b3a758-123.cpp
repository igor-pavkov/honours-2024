static my_bool exts_handlerton(THD *unused, plugin_ref plugin,
                               void *arg)
{
  List<char> *found_exts= (List<char> *) arg;
  handlerton *hton= plugin_hton(plugin);
  List_iterator_fast<char> it(*found_exts);
  const char **ext, *old_ext;

  for (ext= hton->tablefile_extensions; *ext; ext++)
  {
    while ((old_ext= it++))
    {
      if (!strcmp(old_ext, *ext))
        break;
    }
    if (!old_ext)
      found_exts->push_back((char *) *ext);

    it.rewind();
  }
  return FALSE;
}