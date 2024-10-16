uint handler::get_dup_key(int error)
{
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE || m_lock_type != F_UNLCK);
  DBUG_ENTER("handler::get_dup_key");
  if (table->s->long_unique_table && table->file->errkey < table->s->keys)
    DBUG_RETURN(table->file->errkey);
  table->file->errkey  = (uint) -1;
  if (error == HA_ERR_FOUND_DUPP_KEY ||
      error == HA_ERR_FOREIGN_DUPLICATE_KEY ||
      error == HA_ERR_FOUND_DUPP_UNIQUE || error == HA_ERR_NULL_IN_SPATIAL ||
      error == HA_ERR_DROP_INDEX_FK)
    table->file->info(HA_STATUS_ERRKEY | HA_STATUS_NO_LOCK);
  DBUG_RETURN(table->file->errkey);
}