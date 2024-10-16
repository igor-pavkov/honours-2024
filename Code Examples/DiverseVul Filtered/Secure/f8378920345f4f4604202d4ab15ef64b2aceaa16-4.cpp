TfLiteStatus Subgraph::GetExecutionPlan(TfLiteIntArray** execution_plan) {
  plan_cache_.reset(TfLiteIntArrayCreate(execution_plan_.size()));
  *execution_plan = plan_cache_.get();
  static_assert(sizeof(plan_cache_->data[0]) == sizeof(execution_plan_[0]),
                "TfLiteIntArray and execution_plan do not contain same type.");
  std::memcpy(plan_cache_->data, execution_plan_.data(),
              sizeof(plan_cache_->data[0]) * execution_plan_.size());
  return kTfLiteOk;
}