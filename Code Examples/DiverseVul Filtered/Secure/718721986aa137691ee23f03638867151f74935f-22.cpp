TfLiteStatus EvalHybrid(TfLiteContext* context, TfLiteNode* node,
                        TfLiteFullyConnectedParams* params, OpData* data,
                        const TfLiteTensor* input, const TfLiteTensor* filter,
                        const TfLiteTensor* bias, TfLiteTensor* input_quantized,
                        TfLiteTensor* scaling_factors,
                        TfLiteTensor* accum_scratch, TfLiteTensor* row_sums,
                        TfLiteTensor* input_offsets, TfLiteTensor* output) {
  const auto& output_shape = GetTensorShape(output);
  CpuBackendContext* cpu_backend_context =
      CpuBackendContext::GetFromContext(context);
  const bool is_dense = filter->sparsity == nullptr;
  if (is_dense) {
    return EvalHybridDense(context, node, params, data, input, filter, bias,
                           input_quantized, scaling_factors, accum_scratch,
                           row_sums, input_offsets, output);
  }

  TfLiteTensor* filter_ledger = &context->tensors[node->temporaries->data[5]];
  if (!data->ledger_initialized) {
    PopulateLedgerData(filter->sparsity, context,
                       GetTensorData<uint8_t>(filter_ledger));
    data->ledger_initialized = true;
  }

  // The multi-threaded kernel slices the workload along the batch dimension. If
  // there's not enough batches of data, the number of threads used is equal to
  // the batch size.
  // TODO(b/173442777): If needed, we can improve this later with slicing along
  // the row dimension of the weight.
  const int max_threads = cpu_backend_context->max_num_threads();
  const int batches =
      FlatSizeSkipDim(output_shape, output_shape.DimensionsCount() - 1);
  const int thread_count = std::max(1, std::min(batches, max_threads));

  std::vector<SparseHybridFullyConnectedTask> tasks;
  tasks.reserve(thread_count);
  int thread_start = 0;
  for (int i = 0; i < thread_count; ++i) {
    // This makes sure the workload is relatively balanced when batches is not
    // a multiple of thread_count. The first mod(batches, thread_count) tasks
    // need to process one more batch than the rest.
    int thread_end = thread_start + batches / thread_count;
    if (i < batches % thread_count) thread_end++;

    tasks.emplace_back(context, node, params, data, input, filter, bias,
                       thread_start, thread_end, input_quantized,
                       scaling_factors, accum_scratch, row_sums, input_offsets,
                       output);
    thread_start = thread_end;
  }
  cpu_backend_threadpool::Execute(tasks.size(), tasks.data(),
                                  cpu_backend_context);
  return kTfLiteOk;
}