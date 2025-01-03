  void Compute(OpKernelContext* const context) override {
    // node_id_range
    const Tensor* node_id_range_t;
    OP_REQUIRES_OK(context, context->input("node_id_range", &node_id_range_t));
    const auto node_id_range = node_id_range_t->vec<int32>();
    const int32_t node_id_first = node_id_range(0);  // inclusive
    const int32_t node_id_last = node_id_range(1);   // exclusive

    const Tensor* stats_summary_t;
    OP_REQUIRES_OK(context, context->input("stats_summary", &stats_summary_t));
    TTypes<float, 4>::ConstTensor stats_summary =
        stats_summary_t->tensor<float, 4>();
    const int32_t feature_dims = stats_summary_t->dim_size(1);
    // The last bucket is for default/missing value.
    const int32_t num_buckets = stats_summary_t->dim_size(2) - 1;
    const int32_t logits_dim = logits_dim_;
    const int32_t hessian_dim = stats_summary_t->dim_size(3) - logits_dim;
    DCHECK_GT(hessian_dim, 0);
    DCHECK_LE(hessian_dim, logits_dim * logits_dim);

    const Tensor* l1_t;
    OP_REQUIRES_OK(context, context->input("l1", &l1_t));
    const auto l1 = l1_t->scalar<float>()();
    DCHECK_GE(l1, 0);
    if (logits_dim_ > 1) {
      // Multi-class L1 regularization not supported yet.
      DCHECK_EQ(l1, 0);
    }

    const Tensor* l2_t;
    OP_REQUIRES_OK(context, context->input("l2", &l2_t));
    const auto l2 = l2_t->scalar<float>()();
    DCHECK_GE(l2, 0);

    const Tensor* tree_complexity_t;
    OP_REQUIRES_OK(context,
                   context->input("tree_complexity", &tree_complexity_t));
    const auto tree_complexity = tree_complexity_t->scalar<float>()();

    const Tensor* min_node_weight_t;
    OP_REQUIRES_OK(context,
                   context->input("min_node_weight", &min_node_weight_t));
    const auto min_node_weight = min_node_weight_t->scalar<float>()();

    std::vector<int32> output_node_ids;
    std::vector<float> output_gains;
    std::vector<int32> output_feature_dimensions;
    std::vector<int32> output_thresholds;
    std::vector<Eigen::VectorXf> output_left_node_contribs;
    std::vector<Eigen::VectorXf> output_right_node_contribs;
    std::vector<string> output_split_types;

    // TODO(tanzheny) parallelize the computation.
    // Iterate each node and find the best gain per node.
    for (int32_t node_id = node_id_first; node_id < node_id_last; ++node_id) {
      float best_gain = std::numeric_limits<float>::lowest();
      int32_t best_bucket = 0;
      int32_t best_f_dim = 0;
      string best_split_type;
      Eigen::VectorXf best_contrib_for_left(logits_dim);
      Eigen::VectorXf best_contrib_for_right(logits_dim);
      float parent_gain;

      // Including default bucket.
      ConstMatrixMap stats_mat(&stats_summary(node_id, 0, 0, 0),
                               num_buckets + 1, logits_dim + hessian_dim);
      const Eigen::VectorXf total_grad =
          stats_mat.leftCols(logits_dim).colwise().sum();
      const Eigen::VectorXf total_hess =
          stats_mat.rightCols(hessian_dim).colwise().sum();
      if (total_hess.norm() < min_node_weight) {
        continue;
      }
      Eigen::VectorXf parent_weight(logits_dim);
      CalculateWeightsAndGains(total_grad, total_hess, l1, l2, &parent_weight,
                               &parent_gain);

      if (split_type_ == "inequality") {
        CalculateBestInequalitySplit(
            stats_summary, node_id, feature_dims, logits_dim, hessian_dim,
            num_buckets, min_node_weight, l1, l2, &best_gain, &best_bucket,
            &best_f_dim, &best_split_type, &best_contrib_for_left,
            &best_contrib_for_right);
      } else {
        CalculateBestEqualitySplit(
            stats_summary, total_grad, total_hess, node_id, feature_dims,
            logits_dim, hessian_dim, num_buckets, l1, l2, &best_gain,
            &best_bucket, &best_f_dim, &best_split_type, &best_contrib_for_left,
            &best_contrib_for_right);
      }

      if (best_gain == std::numeric_limits<float>::lowest()) {
        // Do not add the node if not split if found.
        continue;
      }
      output_node_ids.push_back(node_id);
      // Remove the parent gain for the parent node.
      output_gains.push_back(best_gain - parent_gain);
      output_feature_dimensions.push_back(best_f_dim);
      // default direction is fixed for dense splits.
      // TODO(tanzheny) account for default values.
      output_split_types.push_back(best_split_type);
      output_thresholds.push_back(best_bucket);
      output_left_node_contribs.push_back(best_contrib_for_left);
      output_right_node_contribs.push_back(best_contrib_for_right);
    }  // for node id
    const int num_nodes = output_node_ids.size();
    // output_node_ids
    Tensor* output_node_ids_t = nullptr;
    OP_REQUIRES_OK(context, context->allocate_output("node_ids", {num_nodes},
                                                     &output_node_ids_t));
    auto output_node_ids_vec = output_node_ids_t->vec<int32>();

    // output_gains
    Tensor* output_gains_t;
    OP_REQUIRES_OK(context, context->allocate_output("gains", {num_nodes},
                                                     &output_gains_t));
    auto output_gains_vec = output_gains_t->vec<float>();

    // output_feature_dimensions
    Tensor* output_feature_dimension_t;
    OP_REQUIRES_OK(context,
                   context->allocate_output("feature_dimensions", {num_nodes},
                                            &output_feature_dimension_t));
    auto output_feature_dimensions_vec =
        output_feature_dimension_t->vec<int32>();

    // output_thresholds
    Tensor* output_thresholds_t;
    OP_REQUIRES_OK(context, context->allocate_output("thresholds", {num_nodes},
                                                     &output_thresholds_t));
    auto output_thresholds_vec = output_thresholds_t->vec<int32>();

    // output_left_node_contribs
    Tensor* output_left_node_contribs_t;
    OP_REQUIRES_OK(context, context->allocate_output(
                                "left_node_contribs", {num_nodes, logits_dim},
                                &output_left_node_contribs_t));
    auto output_left_node_contribs_matrix =
        output_left_node_contribs_t->matrix<float>();

    // output_right_node_contribs
    Tensor* output_right_node_contribs_t;
    OP_REQUIRES_OK(context, context->allocate_output(
                                "right_node_contribs", {num_nodes, logits_dim},
                                &output_right_node_contribs_t));
    auto output_right_node_contribs_matrix =
        output_right_node_contribs_t->matrix<float>();

    // split type
    Tensor* output_split_types_t;
    OP_REQUIRES_OK(
        context, context->allocate_output("split_with_default_directions",
                                          {num_nodes}, &output_split_types_t));
    auto output_split_types_vec = output_split_types_t->vec<tstring>();

    // Sets output tensors from vectors.
    for (int i = 0; i < num_nodes; ++i) {
      output_node_ids_vec(i) = output_node_ids[i];
      // Adjust the gains to penalize by tree complexity.
      output_gains_vec(i) = output_gains[i] - tree_complexity;
      output_feature_dimensions_vec(i) = output_feature_dimensions[i];
      output_thresholds_vec(i) = output_thresholds[i];
      for (int j = 0; j < logits_dim; ++j) {
        output_left_node_contribs_matrix(i, j) =
            output_left_node_contribs[i][j];
        output_right_node_contribs_matrix(i, j) =
            output_right_node_contribs[i][j];
      }
      output_split_types_vec(i) = output_split_types[i];
    }
  }