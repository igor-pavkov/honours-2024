bool Item_exists_subselect::exists2in_processor(void *opt_arg)
{
  THD *thd= (THD *)opt_arg;
  SELECT_LEX *first_select=unit->first_select(), *save_select;
  JOIN *join= first_select->join;
  Item **eq_ref= NULL;
  Item_ident *local_field= NULL;
  Item *outer_exp= NULL;
  Item *left_exp= NULL; Item_in_subselect *in_subs;
  Query_arena *arena= NULL, backup;
  int res= FALSE;
  List<Item> outer;
  Dynamic_array<EQ_FIELD_OUTER> eqs(5, 5);
  bool will_be_correlated;
  DBUG_ENTER("Item_exists_subselect::exists2in_processor");

  if (!optimizer ||
      !optimizer_flag(thd, OPTIMIZER_SWITCH_EXISTS_TO_IN) ||
      (!is_top_level_item() && (!upper_not ||
                                !upper_not->is_top_level_item())) ||
      first_select->is_part_of_union() ||
      first_select->group_list.elements ||
      first_select->order_list.elements ||
      join->having ||
      first_select->with_sum_func ||
      !first_select->leaf_tables.elements||
      !join->conds ||
      with_recursive_reference)
    DBUG_RETURN(FALSE);

  DBUG_ASSERT(first_select->order_list.elements == 0 &&
              first_select->group_list.elements == 0 &&
              first_select->having == NULL);

  if (find_inner_outer_equalities(&join->conds, eqs))
    DBUG_RETURN(FALSE);

  DBUG_ASSERT(eqs.elements() != 0);

  save_select= thd->lex->current_select;
  thd->lex->current_select= first_select;

  /* check that the subquery has only dependencies we are going pull out */
  {
    List<Item> unused;
    Collect_deps_prm prm= {&unused,          // parameters
      unit->first_select()->nest_level_base, // nest_level_base
      0,                                     // count
      unit->first_select()->nest_level,      // nest_level
      FALSE                                  // collect
    };
    walk(&Item::collect_outer_ref_processor, TRUE, &prm);
    DBUG_ASSERT(prm.count > 0);
    DBUG_ASSERT(prm.count >= (uint)eqs.elements());
    will_be_correlated= prm.count > (uint)eqs.elements();
    if (upper_not && will_be_correlated)
      goto out;
  }

  if ((uint)eqs.elements() > (first_select->item_list.elements +
                              first_select->select_n_reserved))
    goto out;
  /* It is simple query */
  DBUG_ASSERT(first_select->join->all_fields.elements ==
              first_select->item_list.elements);

  arena= thd->activate_stmt_arena_if_needed(&backup);

  while (first_select->item_list.elements > (uint)eqs.elements())
  {
    first_select->item_list.pop();
    first_select->join->all_fields.elements--;
  }
  {
    List_iterator<Item> it(first_select->item_list);

    for (uint i= 0; i < (uint)eqs.elements(); i++)
    {
      Item *item= it++;
      eq_ref= eqs.at(i).eq_ref;
      local_field= eqs.at(i).local_field;
      outer_exp= eqs.at(i).outer_exp;
      /* Add the field to the SELECT_LIST */
      if (item)
        it.replace(local_field);
      else
      {
        first_select->item_list.push_back(local_field, thd->mem_root);
        first_select->join->all_fields.elements++;
      }
      first_select->ref_pointer_array[i]= (Item *)local_field;

      /* remove the parts from condition */
      if (!upper_not || !local_field->maybe_null)
        *eq_ref= new (thd->mem_root) Item_int(thd, 1);
      else
      {
        *eq_ref= new (thd->mem_root)
          Item_func_isnotnull(thd,
                              new (thd->mem_root)
                              Item_field(thd,
                                         ((Item_field*)(local_field->
                                                        real_item()))->context,
                                         ((Item_field*)(local_field->
                                                        real_item()))->field));
        if((*eq_ref)->fix_fields(thd, (Item **)eq_ref))
        {
          res= TRUE;
          goto out;
        }
      }
      outer_exp->fix_after_pullout(unit->outer_select(), &outer_exp, FALSE);
      outer_exp->update_used_tables();
      outer.push_back(outer_exp, thd->mem_root);
    }
  }

  join->conds->update_used_tables();

  /* make IN SUBQUERY and put outer_exp as left part */
  if (eqs.elements() == 1)
    left_exp= outer_exp;
  else
  {
    if (!(left_exp= new (thd->mem_root) Item_row(thd, outer)))
    {
      res= TRUE;
      goto out;
    }
  }

  /* make EXISTS->IN permanet (see Item_subselect::init()) */
  set_exists_transformed();

  first_select->select_limit= NULL;
  if (!(in_subs= new (thd->mem_root) Item_in_subselect(thd, left_exp,
                                                         first_select)))
  {
    res= TRUE;
    goto out;
  }
  in_subs->set_exists_transformed();
  optimizer->arguments()[0]= left_exp;
  optimizer->arguments()[1]= in_subs;
  in_subs->optimizer= optimizer;
  DBUG_ASSERT(is_top_level_item() ||
              (upper_not && upper_not->is_top_level_item()));
  in_subs->top_level_item();
  {
    SELECT_LEX *current= thd->lex->current_select;
    optimizer->reset_cache(); // renew cache, and we will not keep it
    thd->lex->current_select= unit->outer_select();
    DBUG_ASSERT(optimizer);
    if (optimizer->fix_left(thd))
    {
      res= TRUE;
      /*
        We should not restore thd->lex->current_select because it will be
        reset on exit from this procedure
      */
      goto out;
    }
    /*
      As far as  Item_ref_in_optimizer do not substitute itself on fix_fields
      we can use same item for all selects.
    */
    in_subs->expr= new (thd->mem_root)
      Item_direct_ref(thd, &first_select->context,
                      (Item**)optimizer->get_cache(),
                      (char *)"<no matter>",
                      (char *)in_left_expr_name);
    if (in_subs->fix_fields(thd, optimizer->arguments() + 1))
    {
      res= TRUE;
      /*
        We should not restore thd->lex->current_select because it will be
        reset on exit from this procedure
      */
      goto out;
    }
    {
      /* Move dependence list */
      List_iterator_fast<Ref_to_outside> it(upper_refs);
      Ref_to_outside *upper;
      while ((upper= it++))
      {
        uint i;
        for (i= 0; i < (uint)eqs.elements(); i++)
          if (eqs.at(i).outer_exp->
              walk(&Item::find_item_processor, TRUE, upper->item))
            break;
        if (i == (uint)eqs.elements() &&
            (in_subs->upper_refs.push_back(upper, thd->stmt_arena->mem_root)))
          goto out;
      }
    }
    in_subs->update_used_tables();
    /*
      The engine of the subquery is fixed so above fix_fields() is not
      complete and should be fixed
    */
    in_subs->upper_refs= upper_refs;
    upper_refs.empty();
    thd->lex->current_select= current;
  }

  DBUG_ASSERT(unit->item == in_subs);
  DBUG_ASSERT(join == first_select->join);
  /*
    Fix dependency info
  */
  in_subs->is_correlated= will_be_correlated;
  if (!will_be_correlated)
  {
    first_select->uncacheable&= ~UNCACHEABLE_DEPENDENT_GENERATED;
    unit->uncacheable&= ~UNCACHEABLE_DEPENDENT_GENERATED;
  }
  /*
    set possible optimization strategies
  */
  in_subs->emb_on_expr_nest= emb_on_expr_nest;
  res= check_and_do_in_subquery_rewrites(join);
  first_select->join->prepare_stage2();

  first_select->fix_prepare_information(thd, &join->conds, &join->having);

  if (upper_not)
  {
    Item *exp;
    if (eqs.elements() == 1)
    {
      exp= (optimizer->arguments()[0]->maybe_null ?
            (Item*) new (thd->mem_root)
            Item_cond_and(thd,
                          new (thd->mem_root)
                          Item_func_isnotnull(thd,
                                              new (thd->mem_root)
                                              Item_direct_ref(thd,
                                                              &unit->outer_select()->context,
                                                              optimizer->arguments(),
                                                              (char *)"<no matter>",
                                                              (char *)exists_outer_expr_name)),
                          optimizer) :
            (Item *)optimizer);
    }
    else
    {
      List<Item> *and_list= new List<Item>;
      if (!and_list)
      {
        res= TRUE;
        goto out;
      }
      for (size_t i= 0; i < eqs.elements(); i++)
      {
        if (optimizer->arguments()[0]->maybe_null)
        {
          and_list->
            push_front(new (thd->mem_root)
                       Item_func_isnotnull(thd,
                                           new (thd->mem_root)
                                           Item_direct_ref(thd,
                                                           &unit->outer_select()->context,
                                                           optimizer->arguments()[0]->addr(i),
                                                           (char *)"<no matter>",
                                                           (char *)exists_outer_expr_name)),
                       thd->mem_root);
        }
      }
      if (and_list->elements > 0)
      {
        and_list->push_front(optimizer, thd->mem_root);
        exp= new (thd->mem_root) Item_cond_and(thd, *and_list);
      }
      else
        exp= optimizer;
    }
    upper_not->arguments()[0]= exp;
    if (!exp->fixed && exp->fix_fields(thd, upper_not->arguments()))
    {
      res= TRUE;
      goto out;
    }
  }

out:
  thd->lex->current_select= save_select;
  if (arena)
    thd->restore_active_arena(arena, &backup);
  DBUG_RETURN(res);
}