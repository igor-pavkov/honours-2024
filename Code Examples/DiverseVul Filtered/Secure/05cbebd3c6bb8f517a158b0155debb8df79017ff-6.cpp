ImmutableExecutorState::~ImmutableExecutorState() {
  for (int32_t i = 0; i < gview_.num_nodes(); i++) {
    NodeItem* item = gview_.node(i);
    if (item != nullptr) {
      params_.delete_kernel(item->kernel);
    }
  }
}