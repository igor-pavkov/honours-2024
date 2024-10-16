static my_bool discover_existence(THD *thd, plugin_ref plugin,
                                  void *arg)
{
  st_discover_existence_args *args= (st_discover_existence_args*)arg;
  handlerton *ht= plugin_hton(plugin);
  if (ht->state != SHOW_OPTION_YES || !ht->discover_table_existence)
    return args->frm_exists;

  args->hton= ht;

  if (ht->discover_table_existence == ext_based_existence)
    return file_ext_exists(args->path, args->path_len,
                           ht->tablefile_extensions[0]);

  return ht->discover_table_existence(ht, args->db, args->table_name);
}