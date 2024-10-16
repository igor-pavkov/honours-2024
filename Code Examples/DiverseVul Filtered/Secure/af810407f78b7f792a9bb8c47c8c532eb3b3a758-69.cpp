bool handler::ha_commit_inplace_alter_table(TABLE *altered_table,
                                            Alter_inplace_info *ha_alter_info,
                                            bool commit)
{
   /*
     At this point we should have an exclusive metadata lock on the table.
     The exception is if we're about to roll back changes (commit= false).
     In this case, we might be rolling back after a failed lock upgrade,
     so we could be holding the same lock level as for inplace_alter_table().
   */
   DBUG_ASSERT(ha_thd()->mdl_context.is_lock_owner(MDL_key::TABLE,
                                                   table->s->db.str,
                                                   table->s->table_name.str,
                                                   MDL_EXCLUSIVE) ||
               !commit);

   return commit_inplace_alter_table(altered_table, ha_alter_info, commit);
}