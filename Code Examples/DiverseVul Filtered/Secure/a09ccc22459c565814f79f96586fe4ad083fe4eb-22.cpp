estimate_local_effects (struct cgraph_node *node)
{
  class ipa_node_params *info = IPA_NODE_REF (node);
  int i, count = ipa_get_param_count (info);
  vec<tree> known_csts;
  vec<ipa_polymorphic_call_context> known_contexts;
  vec<ipa_agg_value_set> known_aggs;
  bool always_const;
  int removable_params_cost;

  if (!count || !ipcp_versionable_function_p (node))
    return;

  if (dump_file && (dump_flags & TDF_DETAILS))
    fprintf (dump_file, "\nEstimating effects for %s.\n", node->dump_name ());

  always_const = gather_context_independent_values (info, &known_csts,
						    &known_contexts, &known_aggs,
						    &removable_params_cost);
  int devirt_bonus = devirtualization_time_bonus (node, known_csts,
					   known_contexts, known_aggs);
  if (always_const || devirt_bonus
      || (removable_params_cost && node->can_change_signature))
    {
      struct caller_statistics stats;
      ipa_hints hints;
      sreal time, base_time;
      int size;

      init_caller_stats (&stats);
      node->call_for_symbol_thunks_and_aliases (gather_caller_stats, &stats,
					      false);
      estimate_ipcp_clone_size_and_time (node, known_csts, known_contexts,
					 known_aggs, &size, &time,
					 &base_time, &hints);
      time -= devirt_bonus;
      time -= hint_time_bonus (hints);
      time -= removable_params_cost;
      size -= stats.n_calls * removable_params_cost;

      if (dump_file)
	fprintf (dump_file, " - context independent values, size: %i, "
		 "time_benefit: %f\n", size, (base_time - time).to_double ());

      if (size <= 0 || node->local)
	{
	  info->do_clone_for_all_contexts = true;

	  if (dump_file)
	    fprintf (dump_file, "     Decided to specialize for all "
		     "known contexts, code not going to grow.\n");
	}
      else if (good_cloning_opportunity_p (node,
					   MIN ((base_time - time).to_int (),
						65536),
					   stats.freq_sum, stats.count_sum,
					   size))
	{
	  if (size + overall_size <= max_new_size)
	    {
	      info->do_clone_for_all_contexts = true;
	      overall_size += size;

	      if (dump_file)
		fprintf (dump_file, "     Decided to specialize for all "
			 "known contexts, growth deemed beneficial.\n");
	    }
	  else if (dump_file && (dump_flags & TDF_DETAILS))
	    fprintf (dump_file, "   Not cloning for all contexts because "
		     "max_new_size would be reached with %li.\n",
		     size + overall_size);
	}
      else if (dump_file && (dump_flags & TDF_DETAILS))
	fprintf (dump_file, "   Not cloning for all contexts because "
		 "!good_cloning_opportunity_p.\n");

    }

  for (i = 0; i < count; i++)
    {
      class ipcp_param_lattices *plats = ipa_get_parm_lattices (info, i);
      ipcp_lattice<tree> *lat = &plats->itself;
      ipcp_value<tree> *val;

      if (lat->bottom
	  || !lat->values
	  || known_csts[i])
	continue;

      for (val = lat->values; val; val = val->next)
	{
	  gcc_checking_assert (TREE_CODE (val->value) != TREE_BINFO);
	  known_csts[i] = val->value;

	  int emc = estimate_move_cost (TREE_TYPE (val->value), true);
	  perform_estimation_of_a_value (node, known_csts, known_contexts,
					 known_aggs,
					 removable_params_cost, emc, val);

	  if (dump_file && (dump_flags & TDF_DETAILS))
	    {
	      fprintf (dump_file, " - estimates for value ");
	      print_ipcp_constant_value (dump_file, val->value);
	      fprintf (dump_file, " for ");
	      ipa_dump_param (dump_file, info, i);
	      fprintf (dump_file, ": time_benefit: %i, size: %i\n",
		       val->local_time_benefit, val->local_size_cost);
	    }
	}
      known_csts[i] = NULL_TREE;
    }

  for (i = 0; i < count; i++)
    {
      class ipcp_param_lattices *plats = ipa_get_parm_lattices (info, i);

      if (!plats->virt_call)
	continue;

      ipcp_lattice<ipa_polymorphic_call_context> *ctxlat = &plats->ctxlat;
      ipcp_value<ipa_polymorphic_call_context> *val;

      if (ctxlat->bottom
	  || !ctxlat->values
	  || !known_contexts[i].useless_p ())
	continue;

      for (val = ctxlat->values; val; val = val->next)
	{
	  known_contexts[i] = val->value;
	  perform_estimation_of_a_value (node, known_csts, known_contexts,
					 known_aggs,
					 removable_params_cost, 0, val);

	  if (dump_file && (dump_flags & TDF_DETAILS))
	    {
	      fprintf (dump_file, " - estimates for polymorphic context ");
	      print_ipcp_constant_value (dump_file, val->value);
	      fprintf (dump_file, " for ");
	      ipa_dump_param (dump_file, info, i);
	      fprintf (dump_file, ": time_benefit: %i, size: %i\n",
		       val->local_time_benefit, val->local_size_cost);
	    }
	}
      known_contexts[i] = ipa_polymorphic_call_context ();
    }

  for (i = 0; i < count; i++)
    {
      class ipcp_param_lattices *plats = ipa_get_parm_lattices (info, i);
      struct ipa_agg_value_set *agg;
      struct ipcp_agg_lattice *aglat;

      if (plats->aggs_bottom || !plats->aggs)
	continue;

      agg = &known_aggs[i];
      for (aglat = plats->aggs; aglat; aglat = aglat->next)
	{
	  ipcp_value<tree> *val;
	  if (aglat->bottom || !aglat->values
	      /* If the following is true, the one value is in known_aggs.  */
	      || (!plats->aggs_contain_variable
		  && aglat->is_single_const ()))
	    continue;

	  for (val = aglat->values; val; val = val->next)
	    {
	      struct ipa_agg_value item;

	      item.offset = aglat->offset;
	      item.value = val->value;
	      agg->items.safe_push (item);

	      perform_estimation_of_a_value (node, known_csts, known_contexts,
					     known_aggs,
					     removable_params_cost, 0, val);

	      if (dump_file && (dump_flags & TDF_DETAILS))
		{
		  fprintf (dump_file, " - estimates for value ");
		  print_ipcp_constant_value (dump_file, val->value);
		  fprintf (dump_file, " for ");
		  ipa_dump_param (dump_file, info, i);
		  fprintf (dump_file, "[%soffset: " HOST_WIDE_INT_PRINT_DEC
			   "]: time_benefit: %i, size: %i\n",
			   plats->aggs_by_ref ? "ref " : "",
			   aglat->offset,
			   val->local_time_benefit, val->local_size_cost);
		}

	      agg->items.pop ();
	    }
	}
    }

  known_csts.release ();
  known_contexts.release ();
  ipa_release_agg_values (known_aggs);
}