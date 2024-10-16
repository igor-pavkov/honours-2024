void Item_subselect::recalc_used_tables(st_select_lex *new_parent, 
                                        bool after_pullout)
{
  List_iterator_fast<Ref_to_outside> it(upper_refs);
  Ref_to_outside *upper;
  DBUG_ENTER("recalc_used_tables");
  
  used_tables_cache= 0;
  while ((upper= it++))
  {
    bool found= FALSE;
    /*
      Check if
        1. the upper reference refers to the new immediate parent select, or
        2. one of the further ancestors.

      We rely on the fact that the tree of selects is modified by some kind of
      'flattening', i.e. a process where child selects are merged into their
      parents.
      The merged selects are removed from the select tree but keep pointers to
      their parents.
    */
    for (st_select_lex *sel= upper->select; sel; sel= sel->outer_select())
    {
      /* 
        If we've reached the new parent select by walking upwards from
        reference's original select, this means that the reference is now 
        referring to the direct parent:
      */
      if (sel == new_parent)
      {
        found= TRUE;
        /* 
          upper->item may be NULL when we've referred to a grouping function,
          in which case we don't care about what it's table_map really is,
          because item->with_sum_func==1 will ensure correct placement of the
          item.
        */
        if (upper->item)
        {
          // Now, iterate over fields and collect used_tables() attribute:
          Field_fixer fixer;
          fixer.used_tables= 0;
          fixer.new_parent= new_parent;
          upper->item->walk(&Item::enumerate_field_refs_processor, 0, &fixer);
          used_tables_cache |= fixer.used_tables;
          upper->item->walk(&Item::update_table_bitmaps_processor, FALSE, NULL);
/*
          if (after_pullout)
            upper->item->fix_after_pullout(new_parent, &(upper->item));
          upper->item->update_used_tables();
*/          
        }
      }
    }
    if (!found)
      used_tables_cache|= OUTER_REF_TABLE_BIT;
  }
  /* 
    Don't update const_tables_cache yet as we don't yet know which of the
    parent's tables are constant. Parent will call update_used_tables() after
    he has done const table detection, and that will be our chance to update
    const_tables_cache.
  */
  DBUG_PRINT("exit", ("used_tables_cache: %llx", used_tables_cache));
  DBUG_VOID_RETURN;
}