int subselect_partial_match_engine::exec()
{
  Item_in_subselect *item_in= (Item_in_subselect *) item;
  int lookup_res;

  DBUG_ASSERT(!(item_in->left_expr_has_null() &&
                item_in->is_top_level_item()));

  if (!item_in->left_expr_has_null())
  {
    /* Try to find a matching row by index lookup. */
    if (lookup_engine->copy_ref_key(false))
    {
      /* The result is FALSE based on the outer reference. */
      item_in->value= 0;
      item_in->null_value= 0;
      return 0;
    }
    else
    {
      /* Search for a complete match. */
      if ((lookup_res= lookup_engine->index_lookup()))
      {
        /* An error occurred during lookup(). */
        item_in->value= 0;
        item_in->null_value= 0;
        return lookup_res;
      }
      else if (item_in->value || !count_columns_with_nulls)
      {
        /*
          A complete match was found, the result of IN is TRUE.
          If no match was found, and there are no NULLs in the materialized
          subquery, then the result is guaranteed to be false because this
          branch is executed when the outer reference has no NULLs as well.
          Notice: (this->item == lookup_engine->item)
        */
        return 0;
      }
    }
  }

  if (has_covering_null_row)
  {
    /*
      If there is a NULL-only row that covers all columns the result of IN
      is UNKNOWN. 
    */
    item_in->value= 0;
    /*
      TIMOUR: which one is the right way to propagate an UNKNOWN result?
      Should we also set empty_result_set= FALSE; ???
    */
    //item_in->was_null= 1;
    item_in->null_value= 1;
    return 0;
  }

  /*
    There is no complete match. Look for a partial match (UNKNOWN result), or
    no match (FALSE).
  */
  if (tmp_table->file->inited)
    tmp_table->file->ha_index_end();

  if (partial_match())
  {
    /* The result of IN is UNKNOWN. */
    item_in->value= 0;
    /*
      TIMOUR: which one is the right way to propagate an UNKNOWN result?
      Should we also set empty_result_set= FALSE; ???
    */
    //item_in->was_null= 1;
    item_in->null_value= 1;
  }
  else
  {
    /* The result of IN is FALSE. */
    item_in->value= 0;
    /*
      TIMOUR: which one is the right way to propagate an UNKNOWN result?
      Should we also set empty_result_set= FALSE; ???
    */
    //item_in->was_null= 0;
    item_in->null_value= 0;
  }

  return 0;
}