edge_clone_summary_t::duplicate (cgraph_edge *src_edge, cgraph_edge *dst_edge,
				 edge_clone_summary *src_data,
				 edge_clone_summary *dst_data)
{
  if (src_data->next_clone)
    edge_clone_summaries->get (src_data->next_clone)->prev_clone = dst_edge;
  dst_data->prev_clone = src_edge;
  dst_data->next_clone = src_data->next_clone;
  src_data->next_clone = dst_edge;
}