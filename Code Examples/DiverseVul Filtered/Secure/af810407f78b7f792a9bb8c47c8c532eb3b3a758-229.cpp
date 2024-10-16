int ha_discover_table_names(THD *thd, LEX_CSTRING *db, MY_DIR *dirp,
                            Discovered_table_list *result, bool reusable)
{
  int error;
  DBUG_ENTER("ha_discover_table_names");

  if (engines_with_discover_file_names == 0 && !reusable)
  {
    st_discover_names_args args= {db, NULL, result, 0};
    error= ext_table_discovery_simple(dirp, result) ||
           plugin_foreach(thd, discover_names,
                            MYSQL_STORAGE_ENGINE_PLUGIN, &args);
  }
  else
  {
    st_discover_names_args args= {db, dirp, result, 0};

    /* extension_based_table_discovery relies on dirp being sorted */
    my_qsort(dirp->dir_entry, dirp->number_of_files,
             sizeof(FILEINFO), cmp_file_names);

    error= extension_based_table_discovery(dirp, reg_ext, result) ||
           plugin_foreach(thd, discover_names,
                            MYSQL_STORAGE_ENGINE_PLUGIN, &args);
    if (args.possible_duplicates > 0)
      result->remove_duplicates();
  }

  DBUG_RETURN(error);
}