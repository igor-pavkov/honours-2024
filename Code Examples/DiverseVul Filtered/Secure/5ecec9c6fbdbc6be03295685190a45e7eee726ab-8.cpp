  void Compute(OpKernelContext* context) override {
    core::RefCountPtr<BoostedTreesEnsembleResource> tree_ensemble_resource;
    OP_REQUIRES_OK(context, LookupResource(context, HandleFromInput(context, 0),
                                           &tree_ensemble_resource));
    mutex_lock l(*tree_ensemble_resource->get_mutex());

    // Get the stamp token.
    const Tensor* stamp_token_t;
    OP_REQUIRES_OK(context, context->input("stamp_token", &stamp_token_t));
    int64_t stamp_token = stamp_token_t->scalar<int64>()();

    // Get the tree ensemble proto.
    const Tensor* tree_ensemble_serialized_t;
    OP_REQUIRES_OK(context, context->input("tree_ensemble_serialized",
                                           &tree_ensemble_serialized_t));
    // Deallocate all the previous objects on the resource.
    tree_ensemble_resource->Reset();
    OP_REQUIRES(
        context,
        tree_ensemble_resource->InitFromSerialized(
            tree_ensemble_serialized_t->scalar<tstring>()(), stamp_token),
        errors::InvalidArgument("Unable to parse tree ensemble proto."));
  }