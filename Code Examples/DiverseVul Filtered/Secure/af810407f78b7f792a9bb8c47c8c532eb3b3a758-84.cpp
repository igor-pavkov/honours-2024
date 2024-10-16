void handler::set_lock_type(enum thr_lock_type lock)
{
  table->reginfo.lock_type= lock;
}