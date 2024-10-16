static bool find_inner_outer_equalities(Item **conds,
                                        Dynamic_array<EQ_FIELD_OUTER> &result)
{
  bool found=  FALSE;
  EQ_FIELD_OUTER element;
  if (is_cond_and(*conds))
  {
    List_iterator<Item> li(*((Item_cond*)*conds)->argument_list());
    Item *item;
    bool allow_subselect= true;
    while ((item= li++))
    {
      if (item->type() == Item::FUNC_ITEM &&
          check_equality_for_exist2in((Item_func *)item,
                                      allow_subselect,
                                      &element.local_field,
                                      &element.outer_exp))
      {
        found= TRUE;
        allow_subselect= false;
        element.eq_ref= li.ref();
        if (result.append(element))
          goto alloc_err;
      }
    }
  }
  else if ((*conds)->type() == Item::FUNC_ITEM &&
           check_equality_for_exist2in((Item_func *)*conds,
                                       true,
                                       &element.local_field,
                                       &element.outer_exp))
  {
    found= TRUE;
    element.eq_ref= conds;
    if (result.append(element))
      goto alloc_err;
  }

  return !found;
alloc_err:
  return TRUE;
}