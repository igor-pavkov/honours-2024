ipcp_lattice<valtype>::print (FILE * f, bool dump_sources, bool dump_benefits)
{
  ipcp_value<valtype> *val;
  bool prev = false;

  if (bottom)
    {
      fprintf (f, "BOTTOM\n");
      return;
    }

  if (!values_count && !contains_variable)
    {
      fprintf (f, "TOP\n");
      return;
    }

  if (contains_variable)
    {
      fprintf (f, "VARIABLE");
      prev = true;
      if (dump_benefits)
	fprintf (f, "\n");
    }

  for (val = values; val; val = val->next)
    {
      if (dump_benefits && prev)
	fprintf (f, "               ");
      else if (!dump_benefits && prev)
	fprintf (f, ", ");
      else
	prev = true;

      print_ipcp_constant_value (f, val->value);

      if (dump_sources)
	{
	  ipcp_value_source<valtype> *s;

	  fprintf (f, " [from:");
	  for (s = val->sources; s; s = s->next)
	    fprintf (f, " %i(%f)", s->cs->caller->order,
		     s->cs->sreal_frequency ().to_double ());
	  fprintf (f, "]");
	}

      if (dump_benefits)
	fprintf (f, " [loc_time: %i, loc_size: %i, "
		 "prop_time: %i, prop_size: %i]\n",
		 val->local_time_benefit, val->local_size_cost,
		 val->prop_time_benefit, val->prop_size_cost);
    }
  if (!dump_benefits)
    fprintf (f, "\n");
}