  virtual Object_creation_ctx *create_backup_ctx(THD *thd) const
  {
    /*
      We can avoid usual backup/restore employed in stored programs since we
      know that this is a top level statement and the worker thread is
      allocated exclusively to execute this event.
    */

    return NULL;
  }