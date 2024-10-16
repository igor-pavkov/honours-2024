int ha_delete_table(THD *thd, handlerton *table_type, const char *path,
                    const LEX_CSTRING *db, const LEX_CSTRING *alias, bool generate_warning)
{
  handler *file;
  char tmp_path[FN_REFLEN];
  int error;
  TABLE dummy_table;
  TABLE_SHARE dummy_share;
  DBUG_ENTER("ha_delete_table");

  /* table_type is NULL in ALTER TABLE when renaming only .frm files */
  if (table_type == NULL || table_type == view_pseudo_hton ||
      ! (file=get_new_handler((TABLE_SHARE*)0, thd->mem_root, table_type)))
    DBUG_RETURN(0);

  bzero((char*) &dummy_table, sizeof(dummy_table));
  bzero((char*) &dummy_share, sizeof(dummy_share));
  dummy_table.s= &dummy_share;

  path= get_canonical_filename(file, path, tmp_path);
  if (unlikely((error= file->ha_delete_table(path))))
  {
    /*
      it's not an error if the table doesn't exist in the engine.
      warn the user, but still report DROP being a success
    */
    bool intercept= error == ENOENT || error == HA_ERR_NO_SUCH_TABLE;

    if (!intercept || generate_warning)
    {
      /* Fill up strucutures that print_error may need */
      dummy_share.path.str= (char*) path;
      dummy_share.path.length= strlen(path);
      dummy_share.normalized_path= dummy_share.path;
      dummy_share.db= *db;
      dummy_share.table_name= *alias;
      dummy_table.alias.set(alias->str, alias->length, table_alias_charset);
      file->change_table_ptr(&dummy_table, &dummy_share);
      file->print_error(error, MYF(intercept ? ME_WARNING : 0));
    }
    if (intercept)
      error= 0;
  }
  delete file;

  DBUG_RETURN(error);
}