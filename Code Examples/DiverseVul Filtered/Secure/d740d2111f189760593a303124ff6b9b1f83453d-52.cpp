tpt_parallel_check(transport_instance * tp, address_item * addr, uschar ** key)
{
unsigned max_parallel;

if (!tp->max_parallel) return FALSE;

max_parallel = (unsigned) expand_string_integer(tp->max_parallel, TRUE);
if (expand_string_message)
  {
  log_write(0, LOG_MAIN|LOG_PANIC, "Failed to expand max_parallel option "
	"in %s transport (%s): %s", tp->name, addr->address,
	expand_string_message);
  return TRUE;
  }

if (max_parallel > 0)
  {
  uschar * serialize_key = string_sprintf("tpt-serialize-%s", tp->name);
  if (!enq_start(serialize_key, max_parallel))
    {
    address_item * next;
    DEBUG(D_transport)
      debug_printf("skipping tpt %s because concurrency limit %u reached\n",
		  tp->name, max_parallel);
    do
      {
      next = addr->next;
      addr->message = US"concurrency limit reached for transport";
      addr->basic_errno = ERRNO_TRETRY;
      post_process_one(addr, DEFER, LOG_MAIN, EXIM_DTYPE_TRANSPORT, 0);
      } while ((addr = next));
    return TRUE;
    }
  *key = serialize_key;
  }
return FALSE;
}