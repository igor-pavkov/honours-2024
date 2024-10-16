TfLiteStatus Subgraph::PrepareOpsAndTensors() {
  if (!memory_planner_) {
    memory_planner_.reset(new ArenaPlanner(
        &context_, std::unique_ptr<GraphInfo>(new InterpreterInfo(this)),
        preserve_all_tensors_, kDefaultTensorAlignment));
    memory_planner_->PlanAllocations();
  }

  // Prepare original execution plan if any applied delegate wants it.
  // If any of the delegates is immutable, this won't be triggered
  // post-delegation (since we undo/redo delegation). For all other cases, other
  // delegates that do shape propagation themselves would still be able to.
  bool prepare_original_plan = false;
  if (!pre_delegation_execution_plan_.empty()) {
    for (int i = 0; i < delegates_applied_.size(); ++i) {
      if ((delegates_applied_[i]->flags &
           kTfLiteDelegateFlagsRequirePropagatedShapes)) {
        prepare_original_plan = true;
        break;
      }
    }
  }
  if (prepare_original_plan) {
    int last_original_exec_plan_index_prepared = 0;
    TF_LITE_ENSURE_STATUS(PrepareOpsStartingAt(
        next_execution_plan_index_to_prepare_, pre_delegation_execution_plan_,
        &last_original_exec_plan_index_prepared));
    next_original_execution_plan_index_to_prepare_ =
        last_original_exec_plan_index_prepared + 1;
  }

  int last_exec_plan_index_prepared = 0;
  TF_LITE_ENSURE_STATUS(
      PrepareOpsStartingAt(next_execution_plan_index_to_prepare_,
                           execution_plan_, &last_exec_plan_index_prepared));
  next_execution_plan_index_to_prepare_ = last_exec_plan_index_prepared + 1;

  // Execute arena allocations.
  TF_LITE_ENSURE_STATUS(memory_planner_->ExecuteAllocations(
      next_execution_plan_index_to_plan_allocation_,
      last_exec_plan_index_prepared));

  // Ensure custom allocations are large enough for applicable tensors.
  // This causes some extra validations for cases with dynamic tensors, but the
  // overhead should be minimal since the number of custom-allocated tensors
  // will typically be low.
  for (int i = 0; i < custom_allocations_.size(); ++i) {
    auto index_and_alloc = custom_allocations_[i];
    TfLiteTensor* tensor_at_index = tensor(index_and_alloc.first);
    const auto& alloc = index_and_alloc.second;
    TF_LITE_ENSURE_EQ(context(), tensor_at_index->allocation_type,
                      kTfLiteCustom);
    if (alloc.bytes < tensor_at_index->bytes) {
      ReportError("Custom allocation is too small for tensor idx: %d",
                  index_and_alloc.first);
      return kTfLiteError;
    }
  }

  next_execution_plan_index_to_plan_allocation_ =
      last_exec_plan_index_prepared + 1;

  return kTfLiteOk;
}