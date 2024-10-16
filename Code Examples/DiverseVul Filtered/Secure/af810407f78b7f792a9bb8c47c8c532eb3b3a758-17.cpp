int handler::ha_open(TABLE *table_arg, const char *name, int mode,
                     uint test_if_locked, MEM_ROOT *mem_root,
                     List<String> *partitions_to_open)
{
  int error;
  DBUG_ENTER("handler::ha_open");
  DBUG_PRINT("enter",
             ("name: %s  db_type: %d  db_stat: %d  mode: %d  lock_test: %d",
              name, ht->db_type, table_arg->db_stat, mode,
              test_if_locked));

  table= table_arg;
  DBUG_ASSERT(table->s == table_share);
  DBUG_ASSERT(m_lock_type == F_UNLCK);
  DBUG_PRINT("info", ("old m_lock_type: %d F_UNLCK %d", m_lock_type, F_UNLCK));
  DBUG_ASSERT(alloc_root_inited(&table->mem_root));

  set_partitions_to_open(partitions_to_open);

  if (unlikely((error=open(name,mode,test_if_locked))))
  {
    if ((error == EACCES || error == EROFS) && mode == O_RDWR &&
	(table->db_stat & HA_TRY_READ_ONLY))
    {
      table->db_stat|=HA_READ_ONLY;
      error=open(name,O_RDONLY,test_if_locked);
    }
  }
  if (unlikely(error))
  {
    my_errno= error;                            /* Safeguard */
    DBUG_PRINT("error",("error: %d  errno: %d",error,errno));
  }
  else
  {
    DBUG_ASSERT(m_psi == NULL);
    DBUG_ASSERT(table_share != NULL);
    /*
      Do not call this for partitions handlers, since it may take too much
      resources.
      So only use the m_psi on table level, not for individual partitions.
    */
    if (!(test_if_locked & HA_OPEN_NO_PSI_CALL))
    {
      m_psi= PSI_CALL_open_table(ha_table_share_psi(), this);
    }

    if (table->s->db_options_in_use & HA_OPTION_READ_ONLY_DATA)
      table->db_stat|=HA_READ_ONLY;
    (void) extra(HA_EXTRA_NO_READCHECK);	// Not needed in SQL

    /* Allocate ref in thd or on the table's mem_root */
    if (!(ref= (uchar*) alloc_root(mem_root ? mem_root : &table->mem_root, 
                                   ALIGN_SIZE(ref_length)*2)))
    {
      ha_close();
      error=HA_ERR_OUT_OF_MEM;
    }
    else
      dup_ref=ref+ALIGN_SIZE(ref_length);
    cached_table_flags= table_flags();
  }
  reset_statistics();
  internal_tmp_table= MY_TEST(test_if_locked & HA_OPEN_INTERNAL_TABLE);

  DBUG_RETURN(error);
}