Graph* GetConstantGraph(
    const Graph* orig_graph, const std::vector<Node*>& nodes,
    const std::unordered_map<const Node*, std::vector<Tensor>>&
        shape_replacement_map,
    std::map<NodeAndOutput, NodeAndOutput>* tensors_to_fetch,
    const ConstantFoldNameGenerator& generate_new_name) {
  Graph* constant_graph = new Graph(orig_graph->op_registry());
  std::unordered_map<Node*, std::vector<Node*>> node_map;
  node_map[orig_graph->source_node()] = {constant_graph->source_node()};
  node_map[orig_graph->sink_node()] = {constant_graph->sink_node()};
  for (Node* n : nodes) {
    if (shape_replacement_map.count(n) == 0) {
      AddNodeToConstantGraph(n, &node_map, constant_graph);
    } else {
      AddShapeNodeToConstantGraph(n, shape_replacement_map, &node_map,
                                  generate_new_name, constant_graph);
    }
  }

  for (auto const& added_nodes : node_map) {
    for (const Edge* out_edge : added_nodes.first->out_edges()) {
      if (node_map.count(out_edge->dst()) == 0) {
        if (out_edge->IsControlEdge()) continue;
        if (added_nodes.second.size() == 1) {
          tensors_to_fetch->insert(
              {{added_nodes.second[0], out_edge->src_output()},
               {added_nodes.first, out_edge->src_output()}});
        } else {
          // The node had multiple outputs and was replaced by a
          // vector of constants, so the NodeAndOutput is the 0th
          // output of the kth added constant, rather than the kth
          // output of the added node as in the standard case above.
          tensors_to_fetch->insert(
              {{added_nodes.second[out_edge->src_output()], 0},
               {added_nodes.first, out_edge->src_output()}});
        }
      }
    }
  }

  return constant_graph;
}