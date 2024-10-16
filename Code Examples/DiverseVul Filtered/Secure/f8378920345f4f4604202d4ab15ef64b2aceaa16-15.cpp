bool Subgraph::OpMightHaveSideEffect(
    const TfLiteNode* node, const TfLiteRegistration* registration) const {
  // Check if any of the input tensors are of type resource.
  if (AnyTensorOfTypeResource(tensors_, node->inputs)) return true;
  // Check if any of the output tensors are of type resource.
  if (AnyTensorOfTypeResource(tensors_, node->outputs)) return true;
  // Consider control flow ops has side effect, some ops in the control flow
  // subgraph can have side effect.
  if (registration->builtin_code == kTfLiteBuiltinIf ||
      registration->builtin_code == kTfLiteBuiltinWhile ||
      registration->builtin_code == kTfLiteBuiltinCallOnce)
    return true;
  return false;
}