TfLiteStatus CreateLedgerTensor(const TfLiteSparsity* sparsity,
                                TfLiteContext* context, TfLiteTensor* ledger) {
  TF_LITE_ENSURE(context, sparsity != nullptr);
  ledger->type = kTfLiteUInt8;
  ledger->allocation_type = kTfLiteArenaRwPersistent;
  TfLiteIntArray* ledger_size = TfLiteIntArrayCreate(1);
  ledger_size->data[0] = sparsity->dim_metadata[1].array_indices->size +
                         sparsity->dim_metadata[1].array_segments->size - 1;
  return context->ResizeTensor(context, ledger, ledger_size);
}