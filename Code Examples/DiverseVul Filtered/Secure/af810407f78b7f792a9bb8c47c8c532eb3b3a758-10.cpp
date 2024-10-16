static my_bool discover_names(THD *thd, plugin_ref plugin,
                              void *arg)
{
  st_discover_names_args *args= (st_discover_names_args *)arg;
  handlerton *ht= plugin_hton(plugin);

  if (ht->state == SHOW_OPTION_YES && ht->discover_table_names)
  {
    size_t old_elements= args->result->tables->elements();
    if (ht->discover_table_names(ht, args->db, args->dirp, args->result))
      return 1;

    /*
      hton_ext_based_table_discovery never discovers a table that has
      a corresponding .frm file; but custom engine discover methods might
    */
    if (ht->discover_table_names != hton_ext_based_table_discovery)
      args->possible_duplicates+= (uint)(args->result->tables->elements() - old_elements);
  }

  return 0;
}