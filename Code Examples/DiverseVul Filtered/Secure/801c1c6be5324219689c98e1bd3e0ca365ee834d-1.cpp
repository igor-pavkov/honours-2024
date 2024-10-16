TfLiteStatus ResizeAndTransposeWeights(TfLiteContext* context,
                                       const TfLiteTensor* weights,
                                       TfLiteTensor* transposed_weights) {
  TfLiteIntArray* transposed_weights_shape_array = TfLiteIntArrayCreate(4);
  const RuntimeShape& input_shape = GetTensorShape(weights);
  transposed_weights_shape_array->data[0] = input_shape.Dims(1);
  transposed_weights_shape_array->data[1] = input_shape.Dims(2);
  transposed_weights_shape_array->data[2] = input_shape.Dims(0);
  transposed_weights_shape_array->data[3] = input_shape.Dims(3);

  transposed_weights->type = weights->type;
  transposed_weights->allocation_type = kTfLiteDynamic;
  TF_LITE_ENSURE_STATUS(context->ResizeTensor(context, transposed_weights,
                                              transposed_weights_shape_array));

  // Transpose the weights from OHWI order to HWOI order.
  TransposeParams transpose_params;
  transpose_params.perm_count = 4;
  transpose_params.perm[0] = 1;
  transpose_params.perm[1] = 2;
  transpose_params.perm[2] = 0;
  transpose_params.perm[3] = 3;

  if (weights->type == kTfLiteFloat32) {
    optimized_ops::Transpose(transpose_params, input_shape,
                             GetTensorData<float>(weights),
                             GetTensorShape(transposed_weights),
                             GetTensorData<float>(transposed_weights));
  } else if (weights->type == kTfLiteUInt8) {
    optimized_ops::Transpose(transpose_params, input_shape,
                             GetTensorData<uint8>(weights),
                             GetTensorShape(transposed_weights),
                             GetTensorData<uint8>(transposed_weights));
  } else if (weights->type == kTfLiteInt8) {
    // int16 transpose_conv also with int8 weights
    optimized_ops::Transpose(transpose_params, input_shape,
                             GetTensorData<int8>(weights),
                             GetTensorShape(transposed_weights),
                             GetTensorData<int8>(transposed_weights));
  } else {
    TF_LITE_KERNEL_LOG(
        context,
        "Only float32, uint8, int8, int16 is supported currently, got %s.",
        TfLiteTypeGetName(weights->type));
    return kTfLiteError;
  }

  return kTfLiteOk;
}