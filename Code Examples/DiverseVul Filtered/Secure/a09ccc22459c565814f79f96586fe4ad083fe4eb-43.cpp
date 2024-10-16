ipcp_lattice<valtype>::add_value (valtype newval, cgraph_edge *cs,
				  ipcp_value<valtype> *src_val,
				  int src_idx, HOST_WIDE_INT offset,
				  ipcp_value<valtype> **val_p,
				  bool unlimited)
{
  ipcp_value<valtype> *val, *last_val = NULL;

  if (val_p)
    *val_p = NULL;

  if (bottom)
    return false;

  for (val = values; val; last_val = val, val = val->next)
    if (values_equal_for_ipcp_p (val->value, newval))
      {
	if (val_p)
	  *val_p = val;

	if (ipa_edge_within_scc (cs))
	  {
	    ipcp_value_source<valtype> *s;
	    for (s = val->sources; s; s = s->next)
	      if (s->cs == cs)
		break;
	    if (s)
	      return false;
	  }

	val->add_source (cs, src_val, src_idx, offset);
	return false;
      }

  if (!unlimited && values_count == param_ipa_cp_value_list_size)
    {
      /* We can only free sources, not the values themselves, because sources
	 of other values in this SCC might point to them.   */
      for (val = values; val; val = val->next)
	{
	  while (val->sources)
	    {
	      ipcp_value_source<valtype> *src = val->sources;
	      val->sources = src->next;
	      ipcp_sources_pool.remove ((ipcp_value_source<tree>*)src);
	    }
	}
      values = NULL;
      return set_to_bottom ();
    }

  values_count++;
  val = allocate_and_init_ipcp_value (newval);
  val->add_source (cs, src_val, src_idx, offset);
  val->next = NULL;

  /* Add the new value to end of value list, which can reduce iterations
     of propagation stage for recursive function.  */
  if (last_val)
    last_val->next = val;
  else
    values = val;

  if (val_p)
    *val_p = val;

  return true;
}