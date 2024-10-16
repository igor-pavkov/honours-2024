subselect_uniquesubquery_engine::~subselect_uniquesubquery_engine()
{
  /* Tell handler we don't need the index anymore */
  //psergey-merge-todo: the following was gone in 6.0:
 //psergey-merge: don't need this after all: tab->table->file->ha_index_end();
}