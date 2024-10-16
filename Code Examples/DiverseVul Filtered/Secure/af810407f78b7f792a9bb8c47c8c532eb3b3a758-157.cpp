int ha_create_table(THD *thd, const char *path,
                    const char *db, const char *table_name,
                    HA_CREATE_INFO *create_info, LEX_CUSTRING *frm)
{
  int error= 1;
  TABLE table;
  char name_buff[FN_REFLEN];
  const char *name;
  TABLE_SHARE share;
  Abort_on_warning_instant_set old_abort_on_warning(thd, 0);
  bool temp_table __attribute__((unused)) =
    create_info->options & (HA_LEX_CREATE_TMP_TABLE | HA_CREATE_TMP_ALTER);
  DBUG_ENTER("ha_create_table");

  init_tmp_table_share(thd, &share, db, 0, table_name, path);

  if (frm)
  {
    bool write_frm_now= !create_info->db_type->discover_table &&
                        !create_info->tmp_table();

    share.frm_image= frm;

    // open an frm image
    if (share.init_from_binary_frm_image(thd, write_frm_now,
                                         frm->str, frm->length))
      goto err;
  }
  else
  {
    // open an frm file
    share.db_plugin= ha_lock_engine(thd, create_info->db_type);

    if (open_table_def(thd, &share))
      goto err;
  }

  share.m_psi= PSI_CALL_get_table_share(temp_table, &share);

  if (open_table_from_share(thd, &share, &empty_clex_str, 0, READ_ALL, 0,
                            &table, true))
    goto err;

  update_create_info_from_table(create_info, &table);

  name= get_canonical_filename(table.file, share.path.str, name_buff);

  error= table.file->ha_create(name, &table, create_info);

  if (unlikely(error))
  {
    if (!thd->is_error())
      my_error(ER_CANT_CREATE_TABLE, MYF(0), db, table_name, error);
    table.file->print_error(error, MYF(ME_WARNING));
    PSI_CALL_drop_table_share(temp_table, share.db.str, (uint)share.db.length,
                              share.table_name.str, (uint)share.table_name.length);
  }

  (void) closefrm(&table);
 
err:
  free_table_share(&share);
  DBUG_RETURN(error != 0);
}