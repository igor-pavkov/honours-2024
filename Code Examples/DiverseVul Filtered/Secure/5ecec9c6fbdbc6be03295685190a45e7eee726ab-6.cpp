  void Compute(OpKernelContext* context) override {
    // Looks up the resource.
    core::RefCountPtr<BoostedTreesEnsembleResource> tree_ensemble_resource;
    OP_REQUIRES_OK(context, LookupResource(context, HandleFromInput(context, 0),
                                           &tree_ensemble_resource));
    tf_shared_lock l(*tree_ensemble_resource->get_mutex());

    // Sets the outputs.
    const int num_trees = tree_ensemble_resource->num_trees();
    const int num_finalized_trees =
        (num_trees <= 0 ||
         tree_ensemble_resource->IsTreeFinalized(num_trees - 1))
            ? num_trees
            : num_trees - 1;
    const int num_attempted_layers =
        tree_ensemble_resource->GetNumLayersAttempted();

    // growing_metadata
    Tensor* output_stamp_token_t = nullptr;
    Tensor* output_num_trees_t = nullptr;
    Tensor* output_num_finalized_trees_t = nullptr;
    Tensor* output_num_attempted_layers_t = nullptr;
    Tensor* output_last_layer_nodes_range_t = nullptr;

    OP_REQUIRES_OK(context, context->allocate_output(0, TensorShape(),
                                                     &output_stamp_token_t));
    OP_REQUIRES_OK(context, context->allocate_output(1, TensorShape(),
                                                     &output_num_trees_t));
    OP_REQUIRES_OK(context,
                   context->allocate_output(2, TensorShape(),
                                            &output_num_finalized_trees_t));
    OP_REQUIRES_OK(context,
                   context->allocate_output(3, TensorShape(),
                                            &output_num_attempted_layers_t));
    OP_REQUIRES_OK(context, context->allocate_output(
                                4, {2}, &output_last_layer_nodes_range_t));

    output_stamp_token_t->scalar<int64>()() = tree_ensemble_resource->stamp();
    output_num_trees_t->scalar<int32>()() = num_trees;
    output_num_finalized_trees_t->scalar<int32>()() = num_finalized_trees;
    output_num_attempted_layers_t->scalar<int32>()() = num_attempted_layers;

    int32_t range_start;
    int32_t range_end;
    tree_ensemble_resource->GetLastLayerNodesRange(&range_start, &range_end);

    output_last_layer_nodes_range_t->vec<int32>()(0) = range_start;
    // For a completely empty ensemble, this will be 0. To make it a valid range
    // we add this max cond.
    output_last_layer_nodes_range_t->vec<int32>()(1) = std::max(1, range_end);
  }