TfLiteStatus Subgraph::ModifyGraphWithDelegate(TfLiteDelegate* delegate) {
  TFLITE_SCOPED_TAGGED_DEFAULT_PROFILE(profiler_.get(),
                                       "ModifyGraphWithDelegate");

  if (delegate == nullptr) {
    ReportError("Null delegate.");
    return kTfLiteDelegateError;
  }

  // Restore delegation state if applicable.
  TF_LITE_ENSURE_STATUS(RedoAllDelegates());

  if (state_ == kStateInvokableAndImmutable) {
    ReportError(
        "ModifyGraphWithDelegate is disallowed when graph is immutable.");
    return kTfLiteApplicationError;
  }

  if (!(delegate->flags & kTfLiteDelegateFlagsAllowDynamicTensors)) {
    int last_execution_plan_index_prepared;
    TF_LITE_ENSURE_OK(
        &context_, PrepareOpsStartingAt(0, execution_plan_,
                                        &last_execution_plan_index_prepared));
    if (has_dynamic_tensors_) {
      // Make sure that we are in a defined ready state before returning.
      // Plan and allocate tensors before returning.
      TF_LITE_ENSURE_OK(&context_, EnsureMemoryAllocations());
      ReportError(
          "Attempting to use a delegate that only supports static-sized "
          "tensors with a graph that has dynamic-sized tensors.");
      return kTfLiteApplicationError;
    }
  }

  const bool was_invokable_before_delegate = state_ == kStateInvokable;
  if (delegates_applied_.empty()) {
    // This is the first delegate being applied, so remember original execution
    // plan.
    // TODO(b/119623453): Restore execution plan to this state if delegate
    // application fails.
    pre_delegation_execution_plan_ = execution_plan_;
  }

  // TODO(aselle): Consider if it is worth storing pointers to delegates.
  // Setup additional context interface.
  SwitchToDelegateContext();

  auto reset_delegation_if_not_ok = [this](TfLiteStatus status) {
    if (status != kTfLiteOk) {
      TF_LITE_ENSURE_STATUS(RemoveAllDelegates());
      ReportError(
          "Restored original execution plan after delegate application "
          "failure.");
      return kTfLiteDelegateError;
    }
    return kTfLiteOk;
  };

  TfLiteStatus status = delegate->Prepare(&context_, delegate);

  // Remove additional context info.
  SwitchToKernelContext();

  TF_LITE_ENSURE_STATUS(reset_delegation_if_not_ok(status));

  if (!(delegate->flags & kTfLiteDelegateFlagsAllowDynamicTensors)) {
    // Reset the state to force tensor/op reallocation.
    state_ = kStateUninvokable;
    TF_LITE_ENSURE_STATUS(
        reset_delegation_if_not_ok(EnsureMemoryAllocations()));
    // After using a delegate which doesn't support dynamic tensors, make the
    // entire graph immutable.
    state_ = kStateInvokableAndImmutable;
  } else if (was_invokable_before_delegate) {
    // If the graph was invokable prior to delegate application, flush
    // allocation now to leave it in a consistent state.
    TF_LITE_ENSURE_STATUS(
        reset_delegation_if_not_ok(EnsureMemoryAllocations()));
  }
  delegates_applied_.push_back(delegate);

  return status;
}