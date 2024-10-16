void ha_maria_implicit_commit(THD *thd, bool new_trn)
{
  if (ha_maria::has_active_transaction(thd))
  {
    int error;
    MDL_request mdl_request;
    mdl_request.init(MDL_key::BACKUP, "", "", MDL_BACKUP_COMMIT, MDL_EXPLICIT);
    error= thd->mdl_context.acquire_lock(&mdl_request,
                                         thd->variables.lock_wait_timeout);
    ha_maria::implicit_commit(thd, new_trn);
    if (!error)
      thd->mdl_context.release_lock(mdl_request.ticket);
  }
}