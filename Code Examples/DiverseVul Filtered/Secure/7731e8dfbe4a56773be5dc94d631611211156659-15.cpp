void AddNodeToConstantGraph(
    Node* n, std::unordered_map<Node*, std::vector<Node*>>* node_map,
    Graph* constant_graph) {
  std::vector<Node*>& added = (*node_map)[n];
  added.push_back(constant_graph->CopyNode(n));
  for (const Edge* in_edge : n->in_edges()) {
    // Don't copy control edges to the constant graph.
    if (!in_edge->IsControlEdge()) {
      Node* in = in_edge->src();
      auto it = node_map->find(in);
      CHECK(it != node_map->end())
          << n->DebugString() << " <-" << in->DebugString();
      if (it->second.size() == 1) {
        constant_graph->AddEdge(it->second[0], in_edge->src_output(), added[0],
                                in_edge->dst_input());
      } else {
        // The original source node had multiple outputs and was replaced by a
        // vector of constants, so the edge comes from the 0th output of the kth
        // added constant, rather than the kth output of the added node as in
        // the standard case above.
        constant_graph->AddEdge(it->second[in_edge->src_output()], 0, added[0],
                                in_edge->dst_input());
      }
    }
  }
}