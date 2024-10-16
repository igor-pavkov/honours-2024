void ImmutableExecutorState::InitializePending(const Graph* graph,
                                               const ControlFlowInfo& cf_info) {
  for (auto& it : cf_info.unique_frame_names) {
    FrameInfo* finfo = EnsureFrameInfo(it);
    DCHECK_EQ(finfo->pending_counts.get(), nullptr);
    finfo->pending_counts =
        absl::make_unique<PendingCounts>(finfo->pending_counts_layout);
  }

  if (!requires_control_flow_) {
    atomic_pending_counts_.reset(new std::atomic<int32>[gview_.num_nodes()]);
    std::fill(atomic_pending_counts_.get(),
              atomic_pending_counts_.get() + gview_.num_nodes(), 0);
  }

  for (const Node* n : graph->nodes()) {
    if (IsSink(n)) continue;
    const int id = n->id();
    const string& name = cf_info.frame_names[id];
    size_t max_pending, max_dead;
    GetMaxPendingCounts(n, &max_pending, &max_dead);
    auto& counts = EnsureFrameInfo(name)->pending_counts;
    counts->set_initial_count(pending_ids_[id], max_pending);
    if (!requires_control_flow_) {
      atomic_pending_counts_[id] = max_pending;
    }
  }
}