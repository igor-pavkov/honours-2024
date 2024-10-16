void AddShapeNodeToConstantGraph(
    Node* n,
    const std::unordered_map<const Node*, std::vector<Tensor>>&
        shape_replacement_map,
    std::unordered_map<Node*, std::vector<Node*>>* node_map,
    const ConstantFoldNameGenerator& generate_new_name, Graph* constant_graph) {
  std::vector<Node*>& added = (*node_map)[n];
  const string& node_name = n->name();
  for (const Tensor& t : shape_replacement_map.at(n)) {
    auto builder =
        NodeDefBuilder(generate_new_name(constant_graph, node_name), "Const")
            .Attr("dtype", t.dtype())
            .Attr("value", t);
    NodeDef def;
    CHECK(builder.Finalize(&def).ok());
    Node* constant_node;
    CHECK(NodeBuilder(builder).Finalize(constant_graph, &constant_node).ok());
    added.push_back(constant_node);
  }
  // Don't copy incoming edges to shape nodes that are being replaced.
}