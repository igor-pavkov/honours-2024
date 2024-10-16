bool IsInitializationOp(const Node* node) {
  return node->op_def().allows_uninitialized_input();
}