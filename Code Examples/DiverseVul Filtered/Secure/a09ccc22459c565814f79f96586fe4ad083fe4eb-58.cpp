ipcp_value<valtype>::add_source (cgraph_edge *cs, ipcp_value *src_val,
				 int src_idx, HOST_WIDE_INT offset)
{
  ipcp_value_source<valtype> *src;

  src = new (ipcp_sources_pool.allocate ()) ipcp_value_source<valtype>;
  src->offset = offset;
  src->cs = cs;
  src->val = src_val;
  src->index = src_idx;

  src->next = sources;
  sources = src;
}