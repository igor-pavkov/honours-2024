  void Compute(OpKernelContext* ctx) override {
    const Tensor* inputs;
    const Tensor* labels_indices;
    const Tensor* labels_values;
    const Tensor* seq_len;
    OP_REQUIRES_OK(ctx, ctx->input("inputs", &inputs));
    OP_REQUIRES_OK(ctx, ctx->input("labels_indices", &labels_indices));
    OP_REQUIRES_OK(ctx, ctx->input("labels_values", &labels_values));
    OP_REQUIRES_OK(ctx, ctx->input("sequence_length", &seq_len));

    OP_REQUIRES(ctx, inputs->shape().dims() == 3,
                errors::InvalidArgument("inputs is not a 3-Tensor"));
    OP_REQUIRES(ctx, TensorShapeUtils::IsVector(seq_len->shape()),
                errors::InvalidArgument("sequence_length is not a vector"));
    OP_REQUIRES(ctx, TensorShapeUtils::IsMatrix(labels_indices->shape()),
                errors::InvalidArgument("labels_indices is not a matrix"));
    OP_REQUIRES(ctx, TensorShapeUtils::IsVector(labels_values->shape()),
                errors::InvalidArgument("labels_values is not a vector"));

    const TensorShape& inputs_shape = inputs->shape();
    const int64 max_time_raw = inputs_shape.dim_size(0);
    const int64 batch_size_raw = inputs_shape.dim_size(1);
    const int64 num_classes_raw = inputs_shape.dim_size(2);
    OP_REQUIRES(ctx,
                FastBoundsCheck(max_time_raw, std::numeric_limits<int>::max()),
                errors::InvalidArgument("max_time_ cannot exceed max int"));
    OP_REQUIRES(
        ctx, FastBoundsCheck(batch_size_raw, std::numeric_limits<int>::max()),
        errors::InvalidArgument("batch_size cannot exceed max int"));
    OP_REQUIRES(
        ctx, FastBoundsCheck(num_classes_raw, std::numeric_limits<int>::max()),
        errors::InvalidArgument("num_classes cannot exceed max int"));
    const int max_time = static_cast<const int>(max_time_raw);
    const int batch_size = static_cast<const int>(batch_size_raw);
    const int num_classes = static_cast<const int>(num_classes_raw);

    OP_REQUIRES(
        ctx, batch_size == seq_len->dim_size(0),
        errors::InvalidArgument("len(sequence_length) != batch_size.  ",
                                "len(sequence_length):  ", seq_len->dim_size(0),
                                " batch_size: ", batch_size));

    OP_REQUIRES(ctx, labels_indices->dim_size(0) == labels_values->dim_size(0),
                errors::InvalidArgument(
                    "labels_indices and labels_values must contain the "
                    "same number of rows, but saw shapes: ",
                    labels_indices->shape().DebugString(), " vs. ",
                    labels_values->shape().DebugString()));
    auto num_indices = labels_indices->dim_size(0);

    OP_REQUIRES(ctx, batch_size != 0,
                errors::InvalidArgument("batch_size must not be 0"));

    Tensor* loss = nullptr;
    OP_REQUIRES_OK(ctx, ctx->allocate_output("loss", seq_len->shape(), &loss));

    Tensor* gradient = nullptr;
    OP_REQUIRES_OK(ctx,
                   ctx->allocate_output("gradient", inputs_shape, &gradient));

    // Convert the labels_indices to labels_lengths.
    std::vector<int> labels_lengths(batch_size, 0);
    DoHistogram<int64>(ctx, labels_indices, num_indices, batch_size,
                       &labels_lengths);

    StreamExecutor* executor = ctx->op_device_context()->stream()->parent();
    se::dnn::DataType data_type = ToDataType<float>::value;

    auto probs_desc_s = executor->createRnnStateTensorDescriptor(
        max_time, batch_size, num_classes, data_type);
    OP_REQUIRES_OK(ctx, probs_desc_s.status());
    std::unique_ptr<RnnStateTensorDescriptor> probs_desc =
        probs_desc_s.ConsumeValueOrDie();

    auto grads_desc_s = executor->createRnnStateTensorDescriptor(
        max_time, batch_size, num_classes, data_type);
    OP_REQUIRES_OK(ctx, grads_desc_s.status());
    std::unique_ptr<RnnStateTensorDescriptor> grads_desc =
        grads_desc_s.ConsumeValueOrDie();

    absl::Span<const int32> labels_data(labels_values->flat<int32>().data(),
                                        num_indices);
    absl::Span<const int32> labels_lengths_data(labels_lengths.data(),
                                                batch_size);
    absl::Span<const int32> input_lengths_data(seq_len->flat<int32>().data(),
                                               batch_size);

    auto probs_data = StreamExecutorUtil::AsDeviceMemory<float>(*inputs);
    auto costs_data = StreamExecutorUtil::AsDeviceMemory<float>(*loss);
    auto grads_data = StreamExecutorUtil::AsDeviceMemory<float>(*gradient);

    // Set the memory limitation to 4GB for workspace memory.
    DnnScratchAllocator workspace_allocator(1LL << 32, ctx);

    Stream* stream = ctx->op_device_context()->stream();
    bool cudnn_launch_status =
        stream
            ->ThenCtcLoss(*probs_desc, probs_data, labels_data,
                          labels_lengths_data, input_lengths_data, &costs_data,
                          *grads_desc, &grads_data, &workspace_allocator)
            .ok();

    if (!cudnn_launch_status) {
      ctx->SetStatus(errors::Internal("cuDNN CTCLoss launch failure"));
    }
  }