Status UnbatchRaggedZerothDim(
    const RaggedTensorVariant& batched_ragged,
    std::vector<RaggedTensorVariant>* ragged_components) {
  // Set up the component Ragged Tensors.
  int ragged_rank = batched_ragged.ragged_rank();
  auto batched_splits_top_vec = batched_ragged.splits(0).vec<SPLIT_TYPE>();
  int num_components = batched_splits_top_vec.size() - 1;
  int num_splits = ragged_rank - 1;
  ragged_components->resize(num_components);
  for (RaggedTensorVariant& ragged_component : *ragged_components) {
    ragged_component.mutable_nested_splits()->reserve(num_splits);
  }
  const auto& batched_flat = batched_ragged.values().flat<VALUE_TYPE>();
  int num_inner_elems = batched_ragged.values().NumElements();
  if (batched_ragged.values().dim_size(0) > 1) {
    num_inner_elems /= batched_ragged.values().dim_size(0);
  }
  TensorShape values_shape = batched_ragged.values().shape();

  // Corner case: ragged_rank == 1, e.g. [[1, 2, 3], [4, 5]]
  if (num_splits == 0) {
    for (int i = 0; i < num_components; i++) {
      int start = batched_splits_top_vec(i);
      int limit = batched_splits_top_vec(i + 1);
      int num_values = limit - start;
      values_shape.set_dim(0, num_values);
      (*ragged_components)[i].set_values(
          Tensor(DataTypeToEnum<VALUE_TYPE>::value, values_shape));
      auto ragged_component_values_flat =
          (*ragged_components)[i].mutable_values()->flat<VALUE_TYPE>();
      for (int j = 0; j < num_values * num_inner_elems; j++) {
        ragged_component_values_flat(j) =
            batched_flat(j + start * num_inner_elems);
      }
    }
    return Status::OK();
  }

  // Unbatch nested splits.
  std::vector<typename TTypes<SPLIT_TYPE>::ConstVec> batched_splits_vec;
  batched_splits_vec.reserve(ragged_rank);
  for (int i = 0; i < ragged_rank; i++) {
    batched_splits_vec.push_back(batched_ragged.splits(i).vec<SPLIT_TYPE>());
  }
  std::vector<int> index(num_splits, 1);
  std::vector<int> ragged_component_values_size(num_components, 0);
  for (int i = 0; i < num_components; i++) {
    std::vector<typename TTypes<SPLIT_TYPE>::Vec> ragged_component_splits_vec;
    ragged_component_splits_vec.reserve(num_splits);
    int split_size = -1;
    for (int j = 0; j < num_splits; j++) {
      if (j == 0) {
        split_size =
            batched_splits_top_vec(i + 1) - batched_splits_top_vec(i) + 1;
      } else {
        // Update split size based on previous split.
        int last_index = ragged_component_splits_vec[j - 1].size() - 1;
        split_size = ragged_component_splits_vec[j - 1](last_index) + 1;
      }
      (*ragged_components)[i].append_splits(
          Tensor(DataTypeToEnum<SPLIT_TYPE>::value, TensorShape({split_size})));
      ragged_component_splits_vec.push_back(
          (*ragged_components)[i].mutable_splits(j)->vec<SPLIT_TYPE>());
      SPLIT_TYPE last_split_value = batched_splits_vec[j + 1](index[j] - 1);
      ragged_component_splits_vec[j](0) = 0;
      for (int k = 1; k < split_size; k++, index[j]++) {
        ragged_component_splits_vec[j](k) =
            batched_splits_vec[j + 1](index[j]) - last_split_value;
      }
    }
    int last_split_size = ragged_component_splits_vec[num_splits - 1].size();
    ragged_component_values_size[i] =
        ragged_component_splits_vec[num_splits - 1](last_split_size - 1);
  }

  // Unbatch values.
  int value_index = 0;
  for (int i = 0; i < num_components; i++) {
    int num_values = ragged_component_values_size[i];
    values_shape.set_dim(0, num_values);
    (*ragged_components)[i].set_values(
        Tensor(DataTypeToEnum<VALUE_TYPE>::value, values_shape));
    auto ragged_component_values_flat =
        (*ragged_components)[i].mutable_values()->flat<VALUE_TYPE>();
    for (int j = 0; j < num_values * num_inner_elems; j++, value_index++) {
      ragged_component_values_flat(j) = batched_flat(value_index);
    }
  }

  return Status::OK();
}