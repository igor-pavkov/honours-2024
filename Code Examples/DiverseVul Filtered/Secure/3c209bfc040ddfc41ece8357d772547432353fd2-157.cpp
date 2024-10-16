void Item_singlerow_subselect::store(uint i, Item *item)
{
  row[i]->store(item);
  row[i]->cache_value();
}