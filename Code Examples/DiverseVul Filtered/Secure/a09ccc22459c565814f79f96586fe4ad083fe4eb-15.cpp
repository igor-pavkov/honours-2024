intersect_aggregates_with_edge (struct cgraph_edge *cs, int index,
				vec<ipa_agg_value> inter)
{
  struct ipa_jump_func *jfunc;
  jfunc = ipa_get_ith_jump_func (IPA_EDGE_REF (cs), index);
  if (jfunc->type == IPA_JF_PASS_THROUGH
      && ipa_get_jf_pass_through_operation (jfunc) == NOP_EXPR)
    {
      class ipa_node_params *caller_info = IPA_NODE_REF (cs->caller);
      int src_idx = ipa_get_jf_pass_through_formal_id (jfunc);

      if (caller_info->ipcp_orig_node)
	{
	  struct cgraph_node *orig_node = caller_info->ipcp_orig_node;
	  class ipcp_param_lattices *orig_plats;
	  orig_plats = ipa_get_parm_lattices (IPA_NODE_REF (orig_node),
					      src_idx);
	  if (agg_pass_through_permissible_p (orig_plats, jfunc))
	    {
	      if (!inter.exists ())
		inter = agg_replacements_to_vector (cs->caller, src_idx, 0);
	      else
		intersect_with_agg_replacements (cs->caller, src_idx,
						 &inter, 0);
	    }
	  else
	    {
	      inter.release ();
	      return vNULL;
	    }
	}
      else
	{
	  class ipcp_param_lattices *src_plats;
	  src_plats = ipa_get_parm_lattices (caller_info, src_idx);
	  if (agg_pass_through_permissible_p (src_plats, jfunc))
	    {
	      /* Currently we do not produce clobber aggregate jump
		 functions, adjust when we do.  */
	      gcc_checking_assert (!jfunc->agg.items);
	      if (!inter.exists ())
		inter = copy_plats_to_inter (src_plats, 0);
	      else
		intersect_with_plats (src_plats, &inter, 0);
	    }
	  else
	    {
	      inter.release ();
	      return vNULL;
	    }
	}
    }
  else if (jfunc->type == IPA_JF_ANCESTOR
	   && ipa_get_jf_ancestor_agg_preserved (jfunc))
    {
      class ipa_node_params *caller_info = IPA_NODE_REF (cs->caller);
      int src_idx = ipa_get_jf_ancestor_formal_id (jfunc);
      class ipcp_param_lattices *src_plats;
      HOST_WIDE_INT delta = ipa_get_jf_ancestor_offset (jfunc);

      if (caller_info->ipcp_orig_node)
	{
	  if (!inter.exists ())
	    inter = agg_replacements_to_vector (cs->caller, src_idx, delta);
	  else
	    intersect_with_agg_replacements (cs->caller, src_idx, &inter,
					     delta);
	}
      else
	{
	  src_plats = ipa_get_parm_lattices (caller_info, src_idx);
	  /* Currently we do not produce clobber aggregate jump
	     functions, adjust when we do.  */
	  gcc_checking_assert (!src_plats->aggs || !jfunc->agg.items);
	  if (!inter.exists ())
	    inter = copy_plats_to_inter (src_plats, delta);
	  else
	    intersect_with_plats (src_plats, &inter, delta);
	}
    }
  else if (jfunc->agg.items)
    {
      class ipa_node_params *caller_info = IPA_NODE_REF (cs->caller);
      struct ipa_agg_value *item;
      int k;

      if (!inter.exists ())
	for (unsigned i = 0; i < jfunc->agg.items->length (); i++)
	  {
	    struct ipa_agg_jf_item *agg_item = &(*jfunc->agg.items)[i];
	    struct ipa_agg_value agg_value;

	    if (self_recursive_agg_pass_through_p (cs, agg_item, index))
	      {
		/* For a self-recursive call, if aggregate jump function is a
		   simple pass-through, the exact value that it stands for is
		   not known at this point, which must comes from other call
		   sites.  But we still need to add a place holder in value
		   sets to indicate it, here we use error_mark_node to
		   represent the special unknown value, which will be replaced
		   with real one during later intersecting operations.  */
		agg_value.value = error_mark_node;
	      }
	    else
	      {
		tree value = ipa_agg_value_from_node (caller_info, cs->caller,
						      agg_item);
		if (!value)
		  continue;

		agg_value.value = value;
	      }

	    agg_value.offset = agg_item->offset;
	    inter.safe_push (agg_value);
	  }
      else
	FOR_EACH_VEC_ELT (inter, k, item)
	  {
	    int l = 0;
	    bool found = false;

	    if (!item->value)
	      continue;

	    while ((unsigned) l < jfunc->agg.items->length ())
	      {
		struct ipa_agg_jf_item *ti;
		ti = &(*jfunc->agg.items)[l];
		if (ti->offset > item->offset)
		  break;
		if (ti->offset == item->offset)
		  {
		    tree value;

		    if (self_recursive_agg_pass_through_p (cs, ti, index))
		      {
			/* A simple aggregate pass-through in self-recursive
			   call should lead to same value.  */
			found = true;
		      }
		    else if ((value = ipa_agg_value_from_node (caller_info,
							     cs->caller, ti)))
		      {
			if (values_equal_for_ipcp_p (item->value, value))
			  found = true;
			else if (item->value == error_mark_node)
			  {
			    /* Replace unknown place holder value with real
			       one.  */
			    item->value = value;
			    found = true;
			  }
		      }
		    break;
		  }
		l++;
	      }
	    if (!found)
	      item->value = NULL;
	  }
    }
  else
    {
      inter.release ();
      return vNULL;
    }
  return inter;
}