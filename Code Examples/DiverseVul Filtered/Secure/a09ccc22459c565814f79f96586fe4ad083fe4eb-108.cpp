print_all_lattices (FILE * f, bool dump_sources, bool dump_benefits)
{
  struct cgraph_node *node;
  int i, count;

  fprintf (f, "\nLattices:\n");
  FOR_EACH_FUNCTION_WITH_GIMPLE_BODY (node)
    {
      class ipa_node_params *info;

      info = IPA_NODE_REF (node);
      /* Skip constprop clones since we don't make lattices for them.  */
      if (info->ipcp_orig_node)
	continue;
      fprintf (f, "  Node: %s:\n", node->dump_name ());
      count = ipa_get_param_count (info);
      for (i = 0; i < count; i++)
	{
	  struct ipcp_agg_lattice *aglat;
	  class ipcp_param_lattices *plats = ipa_get_parm_lattices (info, i);
	  fprintf (f, "    param [%d]: ", i);
	  plats->itself.print (f, dump_sources, dump_benefits);
	  fprintf (f, "         ctxs: ");
	  plats->ctxlat.print (f, dump_sources, dump_benefits);
	  plats->bits_lattice.print (f);
	  fprintf (f, "         ");
	  plats->m_value_range.print (f);
	  fprintf (f, "\n");
	  if (plats->virt_call)
	    fprintf (f, "        virt_call flag set\n");

	  if (plats->aggs_bottom)
	    {
	      fprintf (f, "        AGGS BOTTOM\n");
	      continue;
	    }
	  if (plats->aggs_contain_variable)
	    fprintf (f, "        AGGS VARIABLE\n");
	  for (aglat = plats->aggs; aglat; aglat = aglat->next)
	    {
	      fprintf (f, "        %soffset " HOST_WIDE_INT_PRINT_DEC ": ",
		       plats->aggs_by_ref ? "ref " : "", aglat->offset);
	      aglat->print (f, dump_sources, dump_benefits);
	    }
	}
    }
}