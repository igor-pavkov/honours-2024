  void Compute(OpKernelContext* context) override {
    core::RefCountPtr<BoostedTreesEnsembleResource> tree_ensemble_resource;
    OP_REQUIRES_OK(context, LookupResource(context, HandleFromInput(context, 0),
                                           &tree_ensemble_resource));
    tf_shared_lock l(*tree_ensemble_resource->get_mutex());
    Tensor* output_stamp_token_t = nullptr;
    OP_REQUIRES_OK(context, context->allocate_output(0, TensorShape(),
                                                     &output_stamp_token_t));
    output_stamp_token_t->scalar<int64>()() = tree_ensemble_resource->stamp();
    Tensor* output_proto_t = nullptr;
    OP_REQUIRES_OK(context,
                   context->allocate_output(1, TensorShape(), &output_proto_t));
    output_proto_t->scalar<tstring>()() =
        tree_ensemble_resource->SerializeAsString();
  }