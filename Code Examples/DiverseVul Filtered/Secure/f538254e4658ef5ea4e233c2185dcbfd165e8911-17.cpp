void decoder_context::add_task_decode_CTB_row(thread_context* tctx,
                                              bool firstSliceSubstream,
                                              int ctbRow)
{
  thread_task_ctb_row* task = new thread_task_ctb_row;
  task->firstSliceSubstream = firstSliceSubstream;
  task->tctx = tctx;
  task->debug_startCtbRow = ctbRow;
  tctx->task = task;

  add_task(&thread_pool_, task);

  tctx->imgunit->tasks.push_back(task);
}