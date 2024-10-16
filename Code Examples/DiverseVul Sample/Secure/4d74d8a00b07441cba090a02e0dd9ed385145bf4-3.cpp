  void operator()(const Device& d,
                  const Eigen::DSizes<Eigen::DenseIndex, 2>& shape,
                  const Eigen::array<Eigen::DenseIndex, 2>& logits_bcast,
                  const Eigen::array<Eigen::DenseIndex, 2>& labels_bcast,
                  typename TTypes<T>::ConstMatrix logits,
                  typename TTypes<T>::ConstMatrix labels,
                  typename TTypes<T>::Matrix scratch,
                  typename TTypes<T>::Vec loss,
                  typename TTypes<T>::Matrix backprop) {
    XentEigenImpl<Device, T>::Compute(d, shape, logits_bcast, labels_bcast,
                                      logits, labels, scratch, loss, backprop);
  }