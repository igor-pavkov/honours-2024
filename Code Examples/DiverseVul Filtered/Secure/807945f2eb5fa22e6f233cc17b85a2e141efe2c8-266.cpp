  Item *get_copy(THD *thd)
  { return get_item_copy<Item_ignore_specification>(thd, this); }