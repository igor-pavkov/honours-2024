value_topo_info<valtype>::propagate_effects ()
{
  ipcp_value<valtype> *base;

  for (base = values_topo; base; base = base->topo_next)
    {
      ipcp_value_source<valtype> *src;
      ipcp_value<valtype> *val;
      int time = 0, size = 0;

      for (val = base; val; val = val->scc_next)
	{
	  time = safe_add (time,
			   val->local_time_benefit + val->prop_time_benefit);
	  size = safe_add (size, val->local_size_cost + val->prop_size_cost);
	}

      for (val = base; val; val = val->scc_next)
	for (src = val->sources; src; src = src->next)
	  if (src->val
	      && src->cs->maybe_hot_p ())
	    {
	      src->val->prop_time_benefit = safe_add (time,
						src->val->prop_time_benefit);
	      src->val->prop_size_cost = safe_add (size,
						   src->val->prop_size_cost);
	    }
    }
}