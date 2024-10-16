bool subselect_rowid_merge_engine::partial_match()
{
  Ordered_key *min_key; /* Key that contains the current minimum position. */
  rownum_t min_row_num; /* Current row number of min_key. */
  Ordered_key *cur_key;
  rownum_t cur_row_num;
  uint count_nulls_in_search_key= 0;
  uint max_null_in_any_row=
    ((select_materialize_with_stats *) result)->get_max_nulls_in_row();
  bool res= FALSE;

  /* If there is a non-NULL key, it must be the first key in the keys array. */
  DBUG_ASSERT(!non_null_key || (non_null_key && merge_keys[0] == non_null_key));
  /* The prioryty queue for keys must be empty. */
  DBUG_ASSERT(!pq.elements);

  /* All data accesses during execution are via handler::ha_rnd_pos() */
  if (tmp_table->file->ha_rnd_init_with_error(0))
  {
    res= FALSE;
    goto end;
  }

  /* Check if there is a match for the columns of the only non-NULL key. */
  if (non_null_key && !non_null_key->lookup())
  {
    res= FALSE;
    goto end;
  }

  /*
    If all nullable columns contain only NULLs, then there is a guaranteed
    partial match, and we don't need to search for a matching row.
  */
  if (has_covering_null_columns)
  {
    res= TRUE;
    goto end;
  }

  if (non_null_key)
    queue_insert(&pq, (uchar *) non_null_key);
  /*
    Do not add the non_null_key, since it was already processed above.
  */
  bitmap_clear_all(&matching_outer_cols);
  for (uint i= MY_TEST(non_null_key); i < merge_keys_count; i++)
  {
    DBUG_ASSERT(merge_keys[i]->get_column_count() == 1);
    if (merge_keys[i]->get_search_key(0)->null_value)
    {
      ++count_nulls_in_search_key;
      bitmap_set_bit(&matching_outer_cols, merge_keys[i]->get_keyid());
    }
    else if (merge_keys[i]->lookup())
      queue_insert(&pq, (uchar *) merge_keys[i]);
  }

  /*
    If the outer reference consists of only NULLs, or if it has NULLs in all
    nullable columns (above we guarantee there is a match for the non-null
    coumns), the result is UNKNOWN.
  */
  if (count_nulls_in_search_key == merge_keys_count - MY_TEST(non_null_key))
  {
    res= TRUE;
    goto end;
  }

  /*
    If the outer row has NULLs in some columns, and
    there is no match for any of the remaining columns, and
    there is a subquery row with NULLs in all unmatched columns,
    then there is a partial match, otherwise the result is FALSE.
  */
  if (count_nulls_in_search_key && !pq.elements)
  {
    DBUG_ASSERT(!non_null_key);
    /*
      Check if the intersection of all NULL bitmaps of all keys that
      are not in matching_outer_cols is non-empty.
    */
    res= exists_complementing_null_row(&matching_outer_cols);
    goto end;
  }

  /*
    If there is no NULL (sub)row that covers all NULL columns, and there is no
    match for any of the NULL columns, the result is FALSE. Notice that if there
    is a non-null key, and there is only one matching key, the non-null key is
    the matching key. This is so, because this method returns FALSE if the
    non-null key doesn't have a match.
  */
  if (!count_nulls_in_search_key &&
      (!pq.elements ||
       (pq.elements == 1 && non_null_key &&
        max_null_in_any_row < merge_keys_count-1)))
  {
    if (!pq.elements)
    {
      DBUG_ASSERT(!non_null_key);
      /*
        The case of a covering null row is handled by
        subselect_partial_match_engine::exec()
      */
      DBUG_ASSERT(max_null_in_any_row != tmp_table->s->fields);
    }
    res= FALSE;
    goto end;
  }

  DBUG_ASSERT(pq.elements);

  min_key= (Ordered_key*) queue_remove_top(&pq);
  min_row_num= min_key->current();
  bitmap_set_bit(&matching_keys, min_key->get_keyid());
  bitmap_union(&matching_keys, &matching_outer_cols);
  if (min_key->next_same())
    queue_insert(&pq, (uchar *) min_key);

  if (pq.elements == 0)
  {
    /*
      Check the only matching row of the only key min_key for NULL matches
      in the other columns.
    */
    res= test_null_row(min_row_num);
    goto end;
  }

  while (TRUE)
  {
    cur_key= (Ordered_key*) queue_remove_top(&pq);
    cur_row_num= cur_key->current();

    if (cur_row_num == min_row_num)
      bitmap_set_bit(&matching_keys, cur_key->get_keyid());
    else
    {
      /* Follows from the correct use of priority queue. */
      DBUG_ASSERT(cur_row_num > min_row_num);
      if (test_null_row(min_row_num))
      {
        res= TRUE;
        goto end;
      }
      else
      {
        min_key= cur_key;
        min_row_num= cur_row_num;
        bitmap_clear_all(&matching_keys);
        bitmap_set_bit(&matching_keys, min_key->get_keyid());
        bitmap_union(&matching_keys, &matching_outer_cols);
      }
    }

    if (cur_key->next_same())
      queue_insert(&pq, (uchar *) cur_key);

    if (pq.elements == 0)
    {
      /* Check the last row of the last column in PQ for NULL matches. */
      res= test_null_row(min_row_num);
      goto end;
    }
  }

  /* We should never get here - all branches must be handled explicitly above. */
  DBUG_ASSERT(FALSE);

end:
  if (!has_covering_null_columns)
    bitmap_clear_all(&matching_keys);
  queue_remove_all(&pq);
  tmp_table->file->ha_rnd_end();
  return res;
}