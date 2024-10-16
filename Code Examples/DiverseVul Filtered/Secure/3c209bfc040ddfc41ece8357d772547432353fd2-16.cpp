  virtual void visit_field(Item_field *item)
  {
    //for (TABLE_LIST *tbl= new_parent->leaf_tables; tbl; tbl= tbl->next_local)
    //{
    //  if (tbl->table == field->table)
    //  {
        used_tables|= item->field->table->map;
    //    return;
    //  }
    //}
    //used_tables |= OUTER_REF_TABLE_BIT;
  }