par_wait(void)
{
int poffset, status;
address_item *addr, *addrlist;
pid_t pid;

set_process_info("delivering %s: waiting for a remote delivery subprocess "
  "to finish", message_id);

/* Loop until either a subprocess completes, or there are no subprocesses in
existence - in which case give an error return. We cannot proceed just by
waiting for a completion, because a subprocess may have filled up its pipe, and
be waiting for it to be emptied. Therefore, if no processes have finished, we
wait for one of the pipes to acquire some data by calling select(), with a
timeout just in case.

The simple approach is just to iterate after reading data from a ready pipe.
This leads to non-ideal behaviour when the subprocess has written its final Z
item, closed the pipe, and is in the process of exiting (the common case). A
call to waitpid() yields nothing completed, but select() shows the pipe ready -
reading it yields EOF, so you end up with busy-waiting until the subprocess has
actually finished.

To avoid this, if all the data that is needed has been read from a subprocess
after select(), an explicit wait() for it is done. We know that all it is doing
is writing to the pipe and then exiting, so the wait should not be long.

The non-blocking waitpid() is to some extent just insurance; if we could
reliably detect end-of-file on the pipe, we could always know when to do a
blocking wait() for a completed process. However, because some systems use
NDELAY, which doesn't distinguish between EOF and pipe empty, it is easier to
use code that functions without the need to recognize EOF.

There's a double loop here just in case we end up with a process that is not in
the list of remote delivery processes. Something has obviously gone wrong if
this is the case. (For example, a process that is incorrectly left over from
routing or local deliveries might be found.) The damage can be minimized by
looping back and looking for another process. If there aren't any, the error
return will happen. */

for (;;)   /* Normally we do not repeat this loop */
  {
  while ((pid = waitpid(-1, &status, WNOHANG)) <= 0)
    {
    struct timeval tv;
    fd_set select_pipes;
    int maxpipe, readycount;

    /* A return value of -1 can mean several things. If errno != ECHILD, it
    either means invalid options (which we discount), or that this process was
    interrupted by a signal. Just loop to try the waitpid() again.

    If errno == ECHILD, waitpid() is telling us that there are no subprocesses
    in existence. This should never happen, and is an unexpected error.
    However, there is a nasty complication when running under Linux. If "strace
    -f" is being used under Linux to trace this process and its children,
    subprocesses are "stolen" from their parents and become the children of the
    tracing process. A general wait such as the one we've just obeyed returns
    as if there are no children while subprocesses are running. Once a
    subprocess completes, it is restored to the parent, and waitpid(-1) finds
    it. Thanks to Joachim Wieland for finding all this out and suggesting a
    palliative.

    This does not happen using "truss" on Solaris, nor (I think) with other
    tracing facilities on other OS. It seems to be specific to Linux.

    What we do to get round this is to use kill() to see if any of our
    subprocesses are still in existence. If kill() gives an OK return, we know
    it must be for one of our processes - it can't be for a re-use of the pid,
    because if our process had finished, waitpid() would have found it. If any
    of our subprocesses are in existence, we proceed to use select() as if
    waitpid() had returned zero. I think this is safe. */

    if (pid < 0)
      {
      if (errno != ECHILD) continue;   /* Repeats the waitpid() */

      DEBUG(D_deliver)
        debug_printf("waitpid() returned -1/ECHILD: checking explicitly "
          "for process existence\n");

      for (poffset = 0; poffset < remote_max_parallel; poffset++)
        {
        if ((pid = parlist[poffset].pid) != 0 && kill(pid, 0) == 0)
          {
          DEBUG(D_deliver) debug_printf("process %d still exists: assume "
            "stolen by strace\n", (int)pid);
          break;   /* With poffset set */
          }
        }

      if (poffset >= remote_max_parallel)
        {
        DEBUG(D_deliver) debug_printf("*** no delivery children found\n");
        return NULL;   /* This is the error return */
        }
      }

    /* A pid value greater than 0 breaks the "while" loop. A negative value has
    been handled above. A return value of zero means that there is at least one
    subprocess, but there are no completed subprocesses. See if any pipes are
    ready with any data for reading. */

    DEBUG(D_deliver) debug_printf("selecting on subprocess pipes\n");

    maxpipe = 0;
    FD_ZERO(&select_pipes);
    for (poffset = 0; poffset < remote_max_parallel; poffset++)
      if (parlist[poffset].pid != 0)
        {
        int fd = parlist[poffset].fd;
        FD_SET(fd, &select_pipes);
        if (fd > maxpipe) maxpipe = fd;
        }

    /* Stick in a 60-second timeout, just in case. */

    tv.tv_sec = 60;
    tv.tv_usec = 0;

    readycount = select(maxpipe + 1, (SELECT_ARG2_TYPE *)&select_pipes,
         NULL, NULL, &tv);

    /* Scan through the pipes and read any that are ready; use the count
    returned by select() to stop when there are no more. Select() can return
    with no processes (e.g. if interrupted). This shouldn't matter.

    If par_read_pipe() returns TRUE, it means that either the terminating Z was
    read, or there was a disaster. In either case, we are finished with this
    process. Do an explicit wait() for the process and break the main loop if
    it succeeds.

    It turns out that we have to deal with the case of an interrupted system
    call, which can happen on some operating systems if the signal handling is
    set up to do that by default. */

    for (poffset = 0;
         readycount > 0 && poffset < remote_max_parallel;
         poffset++)
      {
      if (  (pid = parlist[poffset].pid) != 0
         && FD_ISSET(parlist[poffset].fd, &select_pipes)
	 )
        {
        readycount--;
        if (par_read_pipe(poffset, FALSE))    /* Finished with this pipe */
          for (;;)                            /* Loop for signals */
            {
            pid_t endedpid = waitpid(pid, &status, 0);
            if (endedpid == pid) goto PROCESS_DONE;
            if (endedpid != (pid_t)(-1) || errno != EINTR)
              log_write(0, LOG_MAIN|LOG_PANIC_DIE, "Unexpected error return "
                "%d (errno = %d) from waitpid() for process %d",
                (int)endedpid, errno, (int)pid);
            }
        }
      }

    /* Now go back and look for a completed subprocess again. */
    }

  /* A completed process was detected by the non-blocking waitpid(). Find the
  data block that corresponds to this subprocess. */

  for (poffset = 0; poffset < remote_max_parallel; poffset++)
    if (pid == parlist[poffset].pid) break;

  /* Found the data block; this is a known remote delivery process. We don't
  need to repeat the outer loop. This should be what normally happens. */

  if (poffset < remote_max_parallel) break;

  /* This situation is an error, but it's probably better to carry on looking
  for another process than to give up (as we used to do). */

  log_write(0, LOG_MAIN|LOG_PANIC, "Process %d finished: not found in remote "
    "transport process list", pid);
  }  /* End of the "for" loop */

/* Come here when all the data was completely read after a select(), and
the process in pid has been wait()ed for. */

PROCESS_DONE:

DEBUG(D_deliver)
  {
  if (status == 0)
    debug_printf("remote delivery process %d ended\n", (int)pid);
  else
    debug_printf("remote delivery process %d ended: status=%04x\n", (int)pid,
      status);
  }

set_process_info("delivering %s", message_id);

/* Get the chain of processed addresses */

addrlist = parlist[poffset].addrlist;

/* If the process did not finish cleanly, record an error and freeze (except
for SIGTERM, SIGKILL and SIGQUIT), and also ensure the journal is not removed,
in case the delivery did actually happen. */

if ((status & 0xffff) != 0)
  {
  uschar *msg;
  int msb = (status >> 8) & 255;
  int lsb = status & 255;
  int code = (msb == 0)? (lsb & 0x7f) : msb;

  msg = string_sprintf("%s transport process returned non-zero status 0x%04x: "
    "%s %d",
    addrlist->transport->driver_name,
    status,
    (msb == 0)? "terminated by signal" : "exit code",
    code);

  if (msb != 0 || (code != SIGTERM && code != SIGKILL && code != SIGQUIT))
    addrlist->special_action = SPECIAL_FREEZE;

  for (addr = addrlist; addr; addr = addr->next)
    {
    addr->transport_return = DEFER;
    addr->message = msg;
    }

  remove_journal = FALSE;
  }

/* Else complete reading the pipe to get the result of the delivery, if all
the data has not yet been obtained. */

else if (!parlist[poffset].done) (void)par_read_pipe(poffset, TRUE);

/* Put the data count and return path into globals, mark the data slot unused,
decrement the count of subprocesses, and return the address chain. */

transport_count = parlist[poffset].transport_count;
used_return_path = parlist[poffset].return_path;
parlist[poffset].pid = 0;
parcount--;
return addrlist;
}