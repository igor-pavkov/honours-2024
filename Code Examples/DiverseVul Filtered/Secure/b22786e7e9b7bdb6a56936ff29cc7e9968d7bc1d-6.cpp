TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  OpContext op_context(context, node);

  // When the 'axis' tensor is non-const we can't resize output tensors in
  // Prepare(), and we have to do it now.
  if (!IsConstantTensor(op_context.axis)) {
    TF_LITE_ENSURE_OK(
        context,
        ResizeOutputTensors(context, node, op_context.axis, op_context.input,
                            op_context.params->num_splits));
  }

  int axis_value = GetTensorData<int>(op_context.axis)[0];
  if (axis_value < 0) {
    axis_value += NumDimensions(op_context.input);
  }

  TF_LITE_ENSURE(context, axis_value >= 0);
  TF_LITE_ENSURE(context, axis_value < NumDimensions(op_context.input));

  // TODO(b/173221795): Our usage of VectorOfTensors could be optimized by
  // calculating it in Prepare, unless we defer shape calculation.
  // We can improve the optimized_ops version to handle other
  // cases too.
#define TF_LITE_SPLIT(scalar)                                       \
  VectorOfTensors<scalar> all_outputs(*context, *node->outputs);    \
  tflite::SplitParams op_params;                                    \
  op_params.num_split = NumOutputs(node);                           \
  op_params.axis = axis_value;                                      \
  reference_ops::Split(op_params, GetTensorShape(op_context.input), \
                       GetTensorData<scalar>(op_context.input),     \
                       all_outputs.shapes(), all_outputs.data());

  switch (op_context.input->type) {
    case kTfLiteFloat32: {
      TF_LITE_SPLIT(float);
      break;
    }
    case kTfLiteUInt8: {
      TF_LITE_SPLIT(uint8_t);
      break;
    }
    case kTfLiteInt8: {
      TF_LITE_SPLIT(int8_t);
      break;
    }
    case kTfLiteInt16: {
      TF_LITE_SPLIT(int16_t);
      break;
    }
    case kTfLiteInt32: {
      TF_LITE_SPLIT(int32_t);
      break;
    }
    default:
      context->ReportError(context, "Type %s currently not supported.",
                           TfLiteTypeGetName(op_context.input->type));
      return kTfLiteError;
  }
#undef TF_LITE_SPLIT

  return kTfLiteOk;
}