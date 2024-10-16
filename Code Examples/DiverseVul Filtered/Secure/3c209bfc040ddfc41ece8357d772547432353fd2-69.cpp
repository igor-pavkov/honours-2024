subselect_rowid_merge_engine::init(MY_BITMAP *non_null_key_parts,
                                   MY_BITMAP *partial_match_key_parts)
{
  THD *thd= get_thd();
  /* The length in bytes of the rowids (positions) of tmp_table. */
  uint rowid_length= tmp_table->file->ref_length;
  ha_rows row_count= tmp_table->file->stats.records;
  rownum_t cur_rownum= 0;
  select_materialize_with_stats *result_sink=
    (select_materialize_with_stats *) result;
  uint cur_keyid= 0;
  Item_in_subselect *item_in= (Item_in_subselect*) item;
  int error;

  if (merge_keys_count == 0)
  {
    DBUG_ASSERT(bitmap_bits_set(partial_match_key_parts) == 0 ||
                has_covering_null_row);
    /* There is nothing to initialize, we will only do regular lookups. */
    return FALSE;
  }

  /*
    If all nullable columns contain only NULLs, there must be one index
    over all non-null columns.
  */
  DBUG_ASSERT(!has_covering_null_columns ||
              (has_covering_null_columns &&
               merge_keys_count == 1 && non_null_key_parts));
  /*
    Allocate buffers to hold the merged keys and the mapping between rowids and
    row numbers. All small buffers are allocated in the runtime memroot. Big
    buffers are allocated from the OS via malloc.
  */
  if (!(merge_keys= (Ordered_key**) thd->alloc(merge_keys_count *
                                               sizeof(Ordered_key*))) ||
      !(null_bitmaps= (MY_BITMAP**) thd->alloc(merge_keys_count *
                                               sizeof(MY_BITMAP*))) ||
      !(row_num_to_rowid= (uchar*) my_malloc((size_t)(row_count * rowid_length),
        MYF(MY_WME | MY_THREAD_SPECIFIC))))
    return TRUE;

  /* Create the only non-NULL key if there is any. */
  if (non_null_key_parts)
  {
    non_null_key= new Ordered_key(cur_keyid, tmp_table, item_in->left_expr,
                                  0, 0, 0, row_num_to_rowid);
    if (non_null_key->init(non_null_key_parts))
      return TRUE;
    merge_keys[cur_keyid]= non_null_key;
    merge_keys[cur_keyid]->first();
    ++cur_keyid;
  }

  /*
    If all nullable columns contain NULLs, the only key that is needed is the
    only non-NULL key that is already created above.
  */
  if (!has_covering_null_columns)
  {
    if (my_bitmap_init_memroot(&matching_keys, merge_keys_count, thd->mem_root) ||
        my_bitmap_init_memroot(&matching_outer_cols, merge_keys_count, thd->mem_root))
      return TRUE;

    /*
      Create one single-column NULL-key for each column in
      partial_match_key_parts.
    */
    for (uint i= 0; i < partial_match_key_parts->n_bits; i++)
    {
      /* Skip columns that have no NULLs, or contain only NULLs. */
      if (!bitmap_is_set(partial_match_key_parts, i) ||
          result_sink->get_null_count_of_col(i) == row_count)
        continue;

      merge_keys[cur_keyid]= new Ordered_key(
                                     cur_keyid, tmp_table,
                                     item_in->left_expr->element_index(i),
                                     result_sink->get_null_count_of_col(i),
                                     result_sink->get_min_null_of_col(i),
                                     result_sink->get_max_null_of_col(i),
                                     row_num_to_rowid);
      if (merge_keys[cur_keyid]->init(i))
        return TRUE;
      merge_keys[cur_keyid]->first();
      ++cur_keyid;
    }
  }
  DBUG_ASSERT(cur_keyid == merge_keys_count);

  /* Populate the indexes with data from the temporary table. */
  if (tmp_table->file->ha_rnd_init_with_error(1))
    return TRUE;
  tmp_table->file->extra_opt(HA_EXTRA_CACHE,
                             current_thd->variables.read_buff_size);
  tmp_table->null_row= 0;
  while (TRUE)
  {
    error= tmp_table->file->ha_rnd_next(tmp_table->record[0]);
    if (error == HA_ERR_RECORD_DELETED)
    {
      /* We get this for duplicate records that should not be in tmp_table. */
      continue;
    }
    /*
      This is a temp table that we fully own, there should be no other
      cause to stop the iteration than EOF.
    */
    DBUG_ASSERT(!error || error == HA_ERR_END_OF_FILE);
    if (error == HA_ERR_END_OF_FILE)
    {
      DBUG_ASSERT(cur_rownum == tmp_table->file->stats.records);
      break;
    }

    /*
      Save the position of this record in the row_num -> rowid mapping.
    */
    tmp_table->file->position(tmp_table->record[0]);
    memcpy(row_num_to_rowid + cur_rownum * rowid_length,
           tmp_table->file->ref, rowid_length);

    /* Add the current row number to the corresponding keys. */
    if (non_null_key)
    {
      /* By definition there are no NULLs in the non-NULL key. */
      non_null_key->add_key(cur_rownum);
    }

    for (uint i= (non_null_key ? 1 : 0); i < merge_keys_count; i++)
    {
      /*
        Check if the first and only indexed column contains NULL in the current
        row, and add the row number to the corresponding key.
      */
      if (merge_keys[i]->get_field(0)->is_null())
        merge_keys[i]->set_null(cur_rownum);
      else
        merge_keys[i]->add_key(cur_rownum);
    }
    ++cur_rownum;
  }

  tmp_table->file->ha_rnd_end();

  /* Sort all the keys by their NULL selectivity. */
  my_qsort(merge_keys, merge_keys_count, sizeof(Ordered_key*),
           (qsort_cmp) cmp_keys_by_null_selectivity);

  /* Sort the keys in each of the indexes. */
  for (uint i= 0; i < merge_keys_count; i++)
    if (merge_keys[i]->sort_keys())
      return TRUE;

  if (init_queue(&pq, merge_keys_count, 0, FALSE,
                 subselect_rowid_merge_engine::cmp_keys_by_cur_rownum, NULL,
                 0, 0))
    return TRUE;

  return FALSE;
}