handler *get_new_handler(TABLE_SHARE *share, MEM_ROOT *alloc,
                         handlerton *db_type)
{
  handler *file;
  DBUG_ENTER("get_new_handler");
  DBUG_PRINT("enter", ("alloc: %p", alloc));

  if (db_type && db_type->state == SHOW_OPTION_YES && db_type->create)
  {
    if ((file= db_type->create(db_type, share, alloc)))
      file->init();
    DBUG_RETURN(file);
  }
  /*
    Try the default table type
    Here the call to current_thd() is ok as we call this function a lot of
    times but we enter this branch very seldom.
  */
  file= get_new_handler(share, alloc, ha_default_handlerton(current_thd));
  DBUG_RETURN(file);
}