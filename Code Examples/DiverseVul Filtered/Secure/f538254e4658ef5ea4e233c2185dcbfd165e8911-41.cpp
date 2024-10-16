void decoder_context::add_task_decode_slice_segment(thread_context* tctx, bool firstSliceSubstream,
                                                    int ctbx,int ctby)
{
  thread_task_slice_segment* task = new thread_task_slice_segment;
  task->firstSliceSubstream = firstSliceSubstream;
  task->tctx = tctx;
  task->debug_startCtbX = ctbx;
  task->debug_startCtbY = ctby;
  tctx->task = task;

  add_task(&thread_pool_, task);

  tctx->imgunit->tasks.push_back(task);
}