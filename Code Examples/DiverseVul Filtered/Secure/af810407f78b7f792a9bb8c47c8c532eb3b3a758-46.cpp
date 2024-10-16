int ha_release_savepoint(THD *thd, SAVEPOINT *sv)
{
  int error=0;
  Ha_trx_info *ha_info= sv->ha_list;
  DBUG_ENTER("ha_release_savepoint");

  for (; ha_info; ha_info= ha_info->next())
  {
    int err;
    handlerton *ht= ha_info->ht();
    /* Savepoint life time is enclosed into transaction life time. */
    DBUG_ASSERT(ht);
    if (!ht->savepoint_release)
      continue;
    if ((err= ht->savepoint_release(ht, thd,
                                    (uchar *)(sv+1) + ht->savepoint_offset)))
    { // cannot happen
      my_error(ER_GET_ERRNO, MYF(0), err, hton_name(ht)->str);
      error=1;
    }
  }
  DBUG_RETURN(error);
}