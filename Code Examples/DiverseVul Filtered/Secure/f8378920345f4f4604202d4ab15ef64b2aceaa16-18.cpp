TfLiteStatus Subgraph::PreserveAllTensorsExperimental() {
  if (memory_planner_) {
    ReportError(
        "PreserveAllTensorsExperimental called after memory was planned. ");
    return kTfLiteError;
  }
  preserve_all_tensors_ = true;
  return kTfLiteOk;
}