void ConsiderConstantFoldableNode(
    Node* n, const ConstantFoldingOptions& opts, std::vector<Node*>* nodes,
    std::unordered_map<const Node*, gtl::FlatSet<Node*>>* constant_control_deps,
    std::unordered_map<const Node*, std::vector<Tensor>>* shape_replacement_map,
    bool* internal_node_inserted) {
  if (IsConstantFoldable(n, opts.shape_map, opts.consider,
                         opts.max_constant_size_in_bytes,
                         shape_replacement_map)) {
    // A node is constant provided all of its non-control incoming Tensors come
    // from constant nodes, or it's a shape Op with statically known inputs in
    // which case it is placed in shape_replacement_map.
    //
    // We allow control dependencies from non-constant nodes to constant nodes,
    // but to preserve the graph structure we must transfer the control
    // dependency onto any constant replacement.
    bool all_parents_constant = true;
    for (const Edge* in : n->in_edges()) {
      // Allows non-constant -> constant control edges.
      if (!in->IsControlEdge() &&
          constant_control_deps->count(in->src()) == 0) {
        all_parents_constant = false;
        break;
      }
    }
    if (all_parents_constant || shape_replacement_map->count(n) != 0) {
      gtl::FlatSet<Node*>& control_deps = (*constant_control_deps)[n];
      for (const Edge* e : n->in_edges()) {
        if (constant_control_deps->count(e->src()) == 0) {
          // This branch is taken if the incoming edge is a control dependency,
          // in which case we want to add it to the dependencies being
          // accumulated for this node, or the incoming edge is not
          // constant. The latter may happen when n is a shape node and the
          // source has known shape. In that case add a control dependency from
          // the source node, since there was previously a data dependency and
          // we want to preserve sequencing constraints.
          if (!e->src()->IsSource()) {
            control_deps.insert(e->src());
          }
        } else {
          // If the parent has been accumulating control dependencies, add all
          // of its transitive control deps.
          const gtl::FlatSet<Node*>& parent_deps =
              (*constant_control_deps)[e->src()];
          control_deps.insert(parent_deps.begin(), parent_deps.end());
        }
      }
      nodes->push_back(n);
      if (!n->IsConstant()) {
        *internal_node_inserted = true;
      }
    }
  }
}