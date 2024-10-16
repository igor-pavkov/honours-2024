subselect_rowid_merge_engine::~subselect_rowid_merge_engine()
{
  /* None of the resources below is allocated if there are no ordered keys. */
  if (merge_keys_count)
  {
    my_free(row_num_to_rowid);
    for (uint i= 0; i < merge_keys_count; i++)
      delete merge_keys[i];
    delete_queue(&pq);
    if (tmp_table->file->inited == handler::RND)
      tmp_table->file->ha_rnd_end();
  }
}