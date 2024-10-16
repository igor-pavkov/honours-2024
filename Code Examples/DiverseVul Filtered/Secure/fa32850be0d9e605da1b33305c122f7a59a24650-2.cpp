daemon_go(void)
{
struct passwd *pw;
int *listen_sockets = NULL;
int listen_socket_count = 0;
ip_address_item *addresses = NULL;

/* If any debugging options are set, turn on the D_pid bit so that all
debugging lines get the pid added. */

DEBUG(D_any|D_v) debug_selector |= D_pid;


/* Do the preparation for setting up a listener on one or more interfaces, and
possible on various ports. This is controlled by the combination of
local_interfaces (which can set IP addresses and ports) and daemon_smtp_port
(which is a list of default ports to use for those items in local_interfaces
that do not specify a port). The -oX command line option can be used to
override one or both of these options.

If local_interfaces is not set, the default is to listen on all interfaces.
When it is set, it can include "all IPvx interfaces" as an item. This is useful
when different ports are in use.

It turns out that listening on all interfaces is messy in an IPv6 world,
because several different implementation approaches have been taken. This code
is now supposed to work with all of them. The point of difference is whether an
IPv6 socket that is listening on all interfaces will receive incoming IPv4
calls or not. We also have to cope with the case when IPv6 libraries exist, but
there is no IPv6 support in the kernel.

. On Solaris, an IPv6 socket will accept IPv4 calls, and give them as mapped
  addresses. However, if an IPv4 socket is also listening on all interfaces,
  calls are directed to the appropriate socket.

. On (some versions of) Linux, an IPv6 socket will accept IPv4 calls, and
  give them as mapped addresses, but an attempt also to listen on an IPv4
  socket on all interfaces causes an error.

. On OpenBSD, an IPv6 socket will not accept IPv4 calls. You have to set up
  two sockets if you want to accept both kinds of call.

. FreeBSD is like OpenBSD, but it has the IPV6_V6ONLY socket option, which
  can be turned off, to make it behave like the versions of Linux described
  above.

. I heard a report that the USAGI IPv6 stack for Linux has implemented
  IPV6_V6ONLY.

So, what we do when IPv6 is supported is as follows:

 (1) After it is set up, the list of interfaces is scanned for wildcard
     addresses. If an IPv6 and an IPv4 wildcard are both found for the same
     port, the list is re-arranged so that they are together, with the IPv6
     wildcard first.

 (2) If the creation of a wildcard IPv6 socket fails, we just log the error and
     carry on if an IPv4 wildcard socket for the same port follows later in the
     list. This allows Exim to carry on in the case when the kernel has no IPv6
     support.

 (3) Having created an IPv6 wildcard socket, we try to set IPV6_V6ONLY if that
     option is defined. However, if setting fails, carry on regardless (but log
     the incident).

 (4) If binding or listening on an IPv6 wildcard socket fails, it is a serious
     error.

 (5) If binding or listening on an IPv4 wildcard socket fails with the error
     EADDRINUSE, and a previous interface was an IPv6 wildcard for the same
     port (which must have succeeded or we wouldn't have got this far), we
     assume we are in the situation where just a single socket is permitted,
     and ignore the error.

Phew!

The preparation code decodes options and sets up the relevant data. We do this
first, so that we can return non-zero if there are any syntax errors, and also
write to stderr. */

if (daemon_listen)
  {
  int *default_smtp_port;
  int sep;
  int pct = 0;
  uschar *s;
  uschar *list;
  uschar *local_iface_source = US"local_interfaces";
  ip_address_item *ipa;
  ip_address_item **pipa;

  /* If any option requiring a load average to be available during the
  reception of a message is set, call os_getloadavg() while we are root
  for those OS for which this is necessary the first time it is called (in
  order to perform an "open" on the kernel memory file). */

  #ifdef LOAD_AVG_NEEDS_ROOT
  if (queue_only_load >= 0 || smtp_load_reserve >= 0 ||
       (deliver_queue_load_max >= 0 && deliver_drop_privilege))
    (void)os_getloadavg();
  #endif

  /* If -oX was used, disable the writing of a pid file unless -oP was
  explicitly used to force it. Then scan the string given to -oX. Any items
  that contain neither a dot nor a colon are used to override daemon_smtp_port.
  Any other items are used to override local_interfaces. */

  if (override_local_interfaces != NULL)
    {
    uschar *new_smtp_port = NULL;
    uschar *new_local_interfaces = NULL;
    int portsize = 0;
    int portptr = 0;
    int ifacesize = 0;
    int ifaceptr = 0;

    if (override_pid_file_path == NULL) write_pid = FALSE;

    list = override_local_interfaces;
    sep = 0;
    while ((s = string_nextinlist(&list,&sep,big_buffer,big_buffer_size))
           != NULL)
      {
      uschar joinstr[4];
      uschar **ptr;
      int *sizeptr;
      int *ptrptr;

      if (Ustrpbrk(s, ".:") == NULL)
        {
        ptr = &new_smtp_port;
        sizeptr = &portsize;
        ptrptr = &portptr;
        }
      else
        {
        ptr = &new_local_interfaces;
        sizeptr = &ifacesize;
        ptrptr = &ifaceptr;
        }

      if (*ptr == NULL)
        {
        joinstr[0] = sep;
        joinstr[1] = ' ';
        *ptr = string_cat(*ptr, sizeptr, ptrptr, US"<", 1);
        }

      *ptr = string_cat(*ptr, sizeptr, ptrptr, joinstr, 2);
      *ptr = string_cat(*ptr, sizeptr, ptrptr, s, Ustrlen(s));
      }

    if (new_smtp_port != NULL)
      {
      new_smtp_port[portptr] = 0;
      daemon_smtp_port = new_smtp_port;
      DEBUG(D_any) debug_printf("daemon_smtp_port overridden by -oX:\n  %s\n",
        daemon_smtp_port);
      }

    if (new_local_interfaces != NULL)
      {
      new_local_interfaces[ifaceptr] = 0;
      local_interfaces = new_local_interfaces;
      local_iface_source = US"-oX data";
      DEBUG(D_any) debug_printf("local_interfaces overridden by -oX:\n  %s\n",
        local_interfaces);
      }
    }

  /* Create a list of default SMTP ports, to be used if local_interfaces
  contains entries without explict ports. First count the number of ports, then
  build a translated list in a vector. */

  list = daemon_smtp_port;
  sep = 0;
  while ((s = string_nextinlist(&list,&sep,big_buffer,big_buffer_size)) != NULL)
    pct++;
  default_smtp_port = store_get((pct+1) * sizeof(int));
  list = daemon_smtp_port;
  sep = 0;
  for (pct = 0;
       (s = string_nextinlist(&list,&sep,big_buffer,big_buffer_size)) != NULL;
       pct++)
    {
    if (isdigit(*s))
      {
      uschar *end;
      default_smtp_port[pct] = Ustrtol(s, &end, 0);
      if (end != s + Ustrlen(s))
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG, "invalid SMTP port: %s", s);
      }
    else
      {
      struct servent *smtp_service = getservbyname(CS s, "tcp");
      if (smtp_service == NULL)
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG, "TCP port \"%s\" not found", s);
      default_smtp_port[pct] = ntohs(smtp_service->s_port);
      }
    }
  default_smtp_port[pct] = 0;

  /* Create the list of local interfaces, possibly with ports included. This
  list may contain references to 0.0.0.0 and ::0 as wildcards. These special
  values are converted below. */

  addresses = host_build_ifacelist(local_interfaces, local_iface_source);

  /* In the list of IP addresses, convert 0.0.0.0 into an empty string, and ::0
  into the string ":". We use these to recognize wildcards in IPv4 and IPv6. In
  fact, many IP stacks recognize 0.0.0.0 and ::0 and handle them as wildcards
  anyway, but we need to know which are the wildcard addresses, and the shorter
  strings are neater.

  In the same scan, fill in missing port numbers from the default list. When
  there is more than one item in the list, extra items are created. */

  for (ipa = addresses; ipa != NULL; ipa = ipa->next)
    {
    int i;

    if (Ustrcmp(ipa->address, "0.0.0.0") == 0) ipa->address[0] = 0;
    else if (Ustrcmp(ipa->address, "::0") == 0)
      {
      ipa->address[0] = ':';
      ipa->address[1] = 0;
      }

    if (ipa->port > 0) continue;

    if (daemon_smtp_port[0] <= 0)
      log_write(0, LOG_MAIN|LOG_PANIC_DIE, "no port specified for interface "
        "%s and daemon_smtp_port is unset; cannot start daemon",
        (ipa->address[0] == 0)? US"\"all IPv4\"" :
        (ipa->address[1] == 0)? US"\"all IPv6\"" : ipa->address);
    ipa->port = default_smtp_port[0];
    for (i = 1; default_smtp_port[i] > 0; i++)
      {
      ip_address_item *new = store_get(sizeof(ip_address_item));
      memcpy(new->address, ipa->address, Ustrlen(ipa->address) + 1);
      new->port = default_smtp_port[i];
      new->next = ipa->next;
      ipa->next = new;
      ipa = new;
      }
    }

  /* Scan the list of addresses for wildcards. If we find an IPv4 and an IPv6
  wildcard for the same port, ensure that (a) they are together and (b) the
  IPv6 address comes first. This makes handling the messy features easier, and
  also simplifies the construction of the "daemon started" log line. */

  pipa = &addresses;
  for (ipa = addresses; ipa != NULL; pipa = &(ipa->next), ipa = ipa->next)
    {
    ip_address_item *ipa2;

    /* Handle an IPv4 wildcard */

    if (ipa->address[0] == 0)
      {
      for (ipa2 = ipa; ipa2->next != NULL; ipa2 = ipa2->next)
        {
        ip_address_item *ipa3 = ipa2->next;
        if (ipa3->address[0] == ':' &&
            ipa3->address[1] == 0 &&
            ipa3->port == ipa->port)
          {
          ipa2->next = ipa3->next;
          ipa3->next = ipa;
          *pipa = ipa3;
          break;
          }
        }
      }

    /* Handle an IPv6 wildcard. */

    else if (ipa->address[0] == ':' && ipa->address[1] == 0)
      {
      for (ipa2 = ipa; ipa2->next != NULL; ipa2 = ipa2->next)
        {
        ip_address_item *ipa3 = ipa2->next;
        if (ipa3->address[0] == 0 && ipa3->port == ipa->port)
          {
          ipa2->next = ipa3->next;
          ipa3->next = ipa->next;
          ipa->next = ipa3;
          ipa = ipa3;
          break;
          }
        }
      }
    }

  /* Get a vector to remember all the sockets in */

  for (ipa = addresses; ipa != NULL; ipa = ipa->next)
    listen_socket_count++;
  listen_sockets = store_get(sizeof(int *) * listen_socket_count);

  /* Do a sanity check on the max connects value just to save us from getting
  a huge amount of store. */

  if (smtp_accept_max > 4095) smtp_accept_max = 4096;

  /* There's no point setting smtp_accept_queue unless it is less than the max
  connects limit. The configuration reader ensures that the max is set if the
  queue-only option is set. */

  if (smtp_accept_queue > smtp_accept_max) smtp_accept_queue = 0;

  /* Get somewhere to keep the list of SMTP accepting pids if we are keeping
  track of them for total number and queue/host limits. */

  if (smtp_accept_max > 0)
    {
    int i;
    smtp_slots = store_get(smtp_accept_max * sizeof(smtp_slot));
    for (i = 0; i < smtp_accept_max; i++) smtp_slots[i] = empty_smtp_slot;
    }
  }

/* The variable background_daemon is always false when debugging, but
can also be forced false in order to keep a non-debugging daemon in the
foreground. If background_daemon is true, close all open file descriptors that
we know about, but then re-open stdin, stdout, and stderr to /dev/null.

This is protection against any called functions (in libraries, or in
Perl, or whatever) that think they can write to stderr (or stdout). Before this
was added, it was quite likely that an SMTP connection would use one of these
file descriptors, in which case writing random stuff to it caused chaos.

Then disconnect from the controlling terminal, Most modern Unixes seem to have
setsid() for getting rid of the controlling terminal. For any OS that doesn't,
setsid() can be #defined as a no-op, or as something else. */

if (background_daemon)
  {
  log_close_all();    /* Just in case anything was logged earlier */
  search_tidyup();    /* Just in case any were used in reading the config. */
  (void)close(0);           /* Get rid of stdin/stdout/stderr */
  (void)close(1);
  (void)close(2);
  exim_nullstd();     /* Connect stdin/stdout/stderr to /dev/null */
  log_stderr = NULL;  /* So no attempt to copy paniclog output */

  /* If the parent process of this one has pid == 1, we are re-initializing the
  daemon as the result of a SIGHUP. In this case, there is no need to do
  anything, because the controlling terminal has long gone. Otherwise, fork, in
  case current process is a process group leader (see 'man setsid' for an
  explanation) before calling setsid(). */

  if (getppid() != 1)
    {
    pid_t pid = fork();
    if (pid < 0) log_write(0, LOG_MAIN|LOG_PANIC_DIE,
      "fork() failed when starting daemon: %s", strerror(errno));
    if (pid > 0) exit(EXIT_SUCCESS);      /* in parent process, just exit */
    (void)setsid();                       /* release controlling terminal */
    }
  }

/* We are now in the disconnected, daemon process (unless debugging). Set up
the listening sockets if required. */

if (daemon_listen)
  {
  int sk;
  int on = 1;
  ip_address_item *ipa;

  /* For each IP address, create a socket, bind it to the appropriate port, and
  start listening. See comments above about IPv6 sockets that may or may not
  accept IPv4 calls when listening on all interfaces. We also have to cope with
  the case of a system with IPv6 libraries, but no IPv6 support in the kernel.
  listening, provided a wildcard IPv4 socket for the same port follows. */

  for (ipa = addresses, sk = 0; sk < listen_socket_count; ipa = ipa->next, sk++)
    {
    BOOL wildcard;
    ip_address_item *ipa2;
    int af;

    if (Ustrchr(ipa->address, ':') != NULL)
      {
      af = AF_INET6;
      wildcard = ipa->address[1] == 0;
      }
    else
      {
      af = AF_INET;
      wildcard = ipa->address[0] == 0;
      }

    listen_sockets[sk] = ip_socket(SOCK_STREAM, af);
    if (listen_sockets[sk] < 0)
      {
      if (check_special_case(0, addresses, ipa, FALSE))
        {
        log_write(0, LOG_MAIN, "Failed to create IPv6 socket for wildcard "
          "listening (%s): will use IPv4", strerror(errno));
        goto SKIP_SOCKET;
        }
      log_write(0, LOG_PANIC_DIE, "IPv%c socket creation failed: %s",
        (af == AF_INET6)? '6' : '4', strerror(errno));
      }

    /* If this is an IPv6 wildcard socket, set IPV6_V6ONLY if that option is
    available. Just log failure (can get protocol not available, just like
    socket creation can). */

    #ifdef IPV6_V6ONLY
    if (af == AF_INET6 && wildcard &&
        setsockopt(listen_sockets[sk], IPPROTO_IPV6, IPV6_V6ONLY, (char *)(&on),
          sizeof(on)) < 0)
      log_write(0, LOG_MAIN, "Setting IPV6_V6ONLY on daemon's IPv6 wildcard "
        "socket failed (%s): carrying on without it", strerror(errno));
    #endif  /* IPV6_V6ONLY */

    /* Set SO_REUSEADDR so that the daemon can be restarted while a connection
    is being handled.  Without this, a connection will prevent reuse of the
    smtp port for listening. */

    if (setsockopt(listen_sockets[sk], SOL_SOCKET, SO_REUSEADDR,
                   (uschar *)(&on), sizeof(on)) < 0)
      log_write(0, LOG_MAIN|LOG_PANIC_DIE, "setting SO_REUSEADDR on socket "
        "failed when starting daemon: %s", strerror(errno));

    /* Set TCP_NODELAY; Exim does its own buffering. There is a switch to
    disable this because it breaks some broken clients. */

    if (tcp_nodelay) setsockopt(listen_sockets[sk], IPPROTO_TCP, TCP_NODELAY,
      (uschar *)(&on), sizeof(on));

    /* Now bind the socket to the required port; if Exim is being restarted
    it may not always be possible to bind immediately, even with SO_REUSEADDR
    set, so try 10 times, waiting between each try. After 10 failures, we give
    up. In an IPv6 environment, if bind () fails with the error EADDRINUSE and
    we are doing wildcard IPv4 listening and there was a previous IPv6 wildcard
    address for the same port, ignore the error on the grounds that we must be
    in a system where the IPv6 socket accepts both kinds of call. This is
    necessary for (some release of) USAGI Linux; other IP stacks fail at the
    listen() stage instead. */

    for(;;)
      {
      uschar *msg, *addr;
      if (ip_bind(listen_sockets[sk], af, ipa->address, ipa->port) >= 0) break;
      if (check_special_case(errno, addresses, ipa, TRUE))
        {
        DEBUG(D_any) debug_printf("wildcard IPv4 bind() failed after IPv6 "
          "listen() success; EADDRINUSE ignored\n");
        (void)close(listen_sockets[sk]);
        goto SKIP_SOCKET;
        }
      msg = US strerror(errno);
      addr = wildcard? ((af == AF_INET6)? US"(any IPv6)" : US"(any IPv4)") :
        ipa->address;
      if (daemon_startup_retries <= 0)
        log_write(0, LOG_MAIN|LOG_PANIC_DIE,
          "socket bind() to port %d for address %s failed: %s: "
          "daemon abandoned", ipa->port, addr, msg);
      log_write(0, LOG_MAIN, "socket bind() to port %d for address %s "
        "failed: %s: waiting %s before trying again (%d more %s)",
        ipa->port, addr, msg, readconf_printtime(daemon_startup_sleep),
        daemon_startup_retries, (daemon_startup_retries > 1)? "tries" : "try");
      daemon_startup_retries--;
      sleep(daemon_startup_sleep);
      }

    DEBUG(D_any)
      {
      if (wildcard)
        debug_printf("listening on all interfaces (IPv%c) port %d\n",
          (af == AF_INET6)? '6' : '4', ipa->port);
      else
        debug_printf("listening on %s port %d\n", ipa->address, ipa->port);
      }

    /* Start listening on the bound socket, establishing the maximum backlog of
    connections that is allowed. On success, continue to the next address. */

    if (listen(listen_sockets[sk], smtp_connect_backlog) >= 0) continue;

    /* Listening has failed. In an IPv6 environment, as for bind(), if listen()
    fails with the error EADDRINUSE and we are doing IPv4 wildcard listening
    and there was a previous successful IPv6 wildcard listen on the same port,
    we want to ignore the error on the grounds that we must be in a system
    where the IPv6 socket accepts both kinds of call. */

    if (!check_special_case(errno, addresses, ipa, TRUE))
      log_write(0, LOG_PANIC_DIE, "listen() failed on interface %s: %s",
        wildcard? ((af == AF_INET6)? US"(any IPv6)" : US"(any IPv4)") :
        ipa->address,
        strerror(errno));

    DEBUG(D_any) debug_printf("wildcard IPv4 listen() failed after IPv6 "
      "listen() success; EADDRINUSE ignored\n");
    (void)close(listen_sockets[sk]);

    /* Come here if there has been a problem with the socket which we
    are going to ignore. We remove the address from the chain, and back up the
    counts. */

    SKIP_SOCKET:
    sk--;                          /* Back up the count */
    listen_socket_count--;         /* Reduce the total */
    if (ipa == addresses) addresses = ipa->next; else
      {
      for (ipa2 = addresses; ipa2->next != ipa; ipa2 = ipa2->next);
      ipa2->next = ipa->next;
      ipa = ipa2;
      }
    }          /* End of bind/listen loop for each address */
  }            /* End of setup for listening */


/* If we are not listening, we want to write a pid file only if -oP was
explicitly given. */

else if (override_pid_file_path == NULL) write_pid = FALSE;

/* Write the pid to a known file for assistance in identification, if required.
We do this before giving up root privilege, because on some systems it is
necessary to be root in order to write into the pid file directory. There's
nothing to stop multiple daemons running, as long as no more than one listens
on a given TCP/IP port on the same interface(s). However, in these
circumstances it gets far too complicated to mess with pid file names
automatically. Consequently, Exim 4 writes a pid file only

  (a) When running in the test harness, or
  (b) When -bd is used and -oX is not used, or
  (c) When -oP is used to supply a path.

The variable daemon_write_pid is used to control this. */

if (running_in_test_harness || write_pid)
  {
  FILE *f;

  if (override_pid_file_path != NULL)
    pid_file_path = override_pid_file_path;

  if (pid_file_path[0] == 0)
    pid_file_path = string_sprintf("%s/exim-daemon.pid", spool_directory);

  f = modefopen(pid_file_path, "wb", 0644);
  if (f != NULL)
    {
    (void)fprintf(f, "%d\n", (int)getpid());
    (void)fclose(f);
    DEBUG(D_any) debug_printf("pid written to %s\n", pid_file_path);
    }
  else
    {
    DEBUG(D_any)
      debug_printf("%s\n", string_open_failed(errno, "pid file %s",
        pid_file_path));
    }
  }

/* Set up the handler for SIGHUP, which causes a restart of the daemon. */

sighup_seen = FALSE;
signal(SIGHUP, sighup_handler);

/* Give up root privilege at this point (assuming that exim_uid and exim_gid
are not root). The third argument controls the running of initgroups().
Normally we do this, in order to set up the groups for the Exim user. However,
if we are not root at this time - some odd installations run that way - we
cannot do this. */

exim_setugid(exim_uid, exim_gid, geteuid()==root_uid, US"running as a daemon");

/* Update the originator_xxx fields so that received messages as listed as
coming from Exim, not whoever started the daemon. */

originator_uid = exim_uid;
originator_gid = exim_gid;
originator_login = ((pw = getpwuid(exim_uid)) != NULL)?
  string_copy_malloc(US pw->pw_name) : US"exim";

/* Get somewhere to keep the list of queue-runner pids if we are keeping track
of them (and also if we are doing queue runs). */

if (queue_interval > 0 && queue_run_max > 0)
  {
  int i;
  queue_pid_slots = store_get(queue_run_max * sizeof(pid_t));
  for (i = 0; i < queue_run_max; i++) queue_pid_slots[i] = 0;
  }

/* Set up the handler for termination of child processes. */

sigchld_seen = FALSE;
os_non_restarting_signal(SIGCHLD, main_sigchld_handler);

/* If we are to run the queue periodically, pretend the alarm has just gone
off. This will cause the first queue-runner to get kicked off straight away. */

sigalrm_seen = (queue_interval > 0);

/* Log the start up of a daemon - at least one of listening or queue running
must be set up. */

if (daemon_listen)
  {
  int i, j;
  int smtp_ports = 0;
  int smtps_ports = 0;
  ip_address_item *ipa;
  uschar *p = big_buffer;
  uschar *qinfo = (queue_interval > 0)?
    string_sprintf("-q%s", readconf_printtime(queue_interval))
    :
    US"no queue runs";

  /* Build a list of listening addresses in big_buffer, but limit it to 10
  items. The style is for backwards compatibility.

  It is now possible to have some ports listening for SMTPS (the old,
  deprecated protocol that starts TLS without using STARTTLS), and others
  listening for standard SMTP. Keep their listings separate. */

  for (j = 0; j < 2; j++)
    {
    for (i = 0, ipa = addresses; i < 10 && ipa != NULL; i++, ipa = ipa->next)
       {
       /* First time round, look for SMTP ports; second time round, look for
       SMTPS ports. For the first one of each, insert leading text. */

       if (host_is_tls_on_connect_port(ipa->port) == (j > 0))
         {
         if (j == 0)
           {
           if (smtp_ports++ == 0)
             {
             memcpy(p, "SMTP on", 8);
             p += 7;
             }
           }
         else
           {
           if (smtps_ports++ == 0)
             {
             (void)sprintf(CS p, "%sSMTPS on",
               (smtp_ports == 0)? "":" and for ");
             while (*p != 0) p++;
             }
           }

         /* Now the information about the port (and sometimes interface) */

         if (ipa->address[0] == ':' && ipa->address[1] == 0)
           {
           if (ipa->next != NULL && ipa->next->address[0] == 0 &&
               ipa->next->port == ipa->port)
             {
             (void)sprintf(CS p, " port %d (IPv6 and IPv4)", ipa->port);
             ipa = ipa->next;
             }
           else if (ipa->v6_include_v4)
             (void)sprintf(CS p, " port %d (IPv6 with IPv4)", ipa->port);
           else
             (void)sprintf(CS p, " port %d (IPv6)", ipa->port);
           }
         else if (ipa->address[0] == 0)
           (void)sprintf(CS p, " port %d (IPv4)", ipa->port);
         else
           (void)sprintf(CS p, " [%s]:%d", ipa->address, ipa->port);
         while (*p != 0) p++;
         }
       }

    if (ipa != NULL)
      {
      memcpy(p, " ...", 5);
      p += 4;
      }
    }

  log_write(0, LOG_MAIN,
    "exim %s daemon started: pid=%d, %s, listening for %s",
    version_string, getpid(), qinfo, big_buffer);
  set_process_info("daemon: %s, listening for %s", qinfo, big_buffer);
  }

else
  {
  log_write(0, LOG_MAIN,
    "exim %s daemon started: pid=%d, -q%s, not listening for SMTP",
    version_string, getpid(), readconf_printtime(queue_interval));
  set_process_info("daemon: -q%s, not listening",
    readconf_printtime(queue_interval));
  }


/* Close the log so it can be renamed and moved. In the few cases below where
this long-running process writes to the log (always exceptional conditions), it
closes the log afterwards, for the same reason. */

log_close_all();

DEBUG(D_any) debug_print_ids(US"daemon running with");

/* Any messages accepted via this route are going to be SMTP. */

smtp_input = TRUE;

/* Enter the never-ending loop... */

for (;;)
  {
  #if HAVE_IPV6
  struct sockaddr_in6 accepted;
  #else
  struct sockaddr_in accepted;
  #endif

  EXIM_SOCKLEN_T len;
  pid_t pid;

  /* This code is placed first in the loop, so that it gets obeyed at the
  start, before the first wait. This causes the first queue-runner to be
  started immediately. */

  if (sigalrm_seen)
    {
    DEBUG(D_any) debug_printf("SIGALRM received\n");

    /* Do a full queue run in a child process, if required, unless we already
    have enough queue runners on the go. If we are not running as root, a
    re-exec is required. */

    if (queue_interval > 0 &&
       (queue_run_max <= 0 || queue_run_count < queue_run_max))
      {
      if ((pid = fork()) == 0)
        {
        int sk;

        DEBUG(D_any) debug_printf("Starting queue-runner: pid %d\n",
          (int)getpid());

        /* Disable debugging if it's required only for the daemon process. We
        leave the above message, because it ties up with the "child ended"
        debugging messages. */

        if (debug_daemon) debug_selector = 0;

        /* Close any open listening sockets in the child */

        for (sk = 0; sk < listen_socket_count; sk++)
          (void)close(listen_sockets[sk]);

        /* Reset SIGHUP and SIGCHLD in the child in both cases. */

        signal(SIGHUP,  SIG_DFL);
        signal(SIGCHLD, SIG_DFL);

        /* Re-exec if privilege has been given up, unless deliver_drop_
        privilege is set. Reset SIGALRM before exec(). */

        if (geteuid() != root_uid && !deliver_drop_privilege)
          {
          uschar opt[8];
          uschar *p = opt;
          uschar *extra[5];
          int extracount = 1;

          signal(SIGALRM, SIG_DFL);
          *p++ = '-';
          *p++ = 'q';
          if (queue_2stage) *p++ = 'q';
          if (queue_run_first_delivery) *p++ = 'i';
          if (queue_run_force) *p++ = 'f';
          if (deliver_force_thaw) *p++ = 'f';
          if (queue_run_local) *p++ = 'l';
          *p = 0;
          extra[0] = opt;

          /* If -R or -S were on the original command line, ensure they get
          passed on. */

          if (deliver_selectstring != NULL)
            {
            extra[extracount++] = deliver_selectstring_regex? US"-Rr" : US"-R";
            extra[extracount++] = deliver_selectstring;
            }

          if (deliver_selectstring_sender != NULL)
            {
            extra[extracount++] = deliver_selectstring_sender_regex?
              US"-Sr" : US"-S";
            extra[extracount++] = deliver_selectstring_sender;
            }

          /* Overlay this process with a new execution. */

          (void)child_exec_exim(CEE_EXEC_PANIC, FALSE, NULL, TRUE, extracount,
            extra[0], extra[1], extra[2], extra[3], extra[4]);

          /* Control never returns here. */
          }

        /* No need to re-exec; SIGALRM remains set to the default handler */

        queue_run(NULL, NULL, FALSE);
        _exit(EXIT_SUCCESS);
        }

      if (pid < 0)
        {
        log_write(0, LOG_MAIN|LOG_PANIC, "daemon: fork of queue-runner "
          "process failed: %s", strerror(errno));
        log_close_all();
        }
      else
        {
        int i;
        for (i = 0; i < queue_run_max; ++i)
          {
          if (queue_pid_slots[i] <= 0)
            {
            queue_pid_slots[i] = pid;
            queue_run_count++;
            break;
            }
          }
        DEBUG(D_any) debug_printf("%d queue-runner process%s running\n",
          queue_run_count, (queue_run_count == 1)? "" : "es");
        }
      }

    /* Reset the alarm clock */

    sigalrm_seen = FALSE;
    alarm(queue_interval);
    }


  /* Sleep till a connection happens if listening, and handle the connection if
  that is why we woke up. The FreeBSD operating system requires the use of
  select() before accept() because the latter function is not interrupted by
  a signal, and we want to wake up for SIGCHLD and SIGALRM signals. Some other
  OS do notice signals in accept() but it does no harm to have the select()
  in for all of them - and it won't then be a lurking problem for ports to
  new OS. In fact, the later addition of listening on specific interfaces only
  requires this way of working anyway. */

  if (daemon_listen)
    {
    int sk, lcount, select_errno;
    int max_socket = 0;
    BOOL select_failed = FALSE;
    fd_set select_listen;

    FD_ZERO(&select_listen);
    for (sk = 0; sk < listen_socket_count; sk++)
      {
      FD_SET(listen_sockets[sk], &select_listen);
      if (listen_sockets[sk] > max_socket) max_socket = listen_sockets[sk];
      }

    DEBUG(D_any) debug_printf("Listening...\n");

    /* In rare cases we may have had a SIGCHLD signal in the time between
    setting the handler (below) and getting back here. If so, pretend that the
    select() was interrupted so that we reap the child. This might still leave
    a small window when a SIGCHLD could get lost. However, since we use SIGCHLD
    only to do the reaping more quickly, it shouldn't result in anything other
    than a delay until something else causes a wake-up. */

    if (sigchld_seen)
      {
      lcount = -1;
      errno = EINTR;
      }
    else
      {
      lcount = select(max_socket + 1, (SELECT_ARG2_TYPE *)&select_listen,
        NULL, NULL, NULL);
      }

    if (lcount < 0)
      {
      select_failed = TRUE;
      lcount = 1;
      }

    /* Clean up any subprocesses that may have terminated. We need to do this
    here so that smtp_accept_max_per_host works when a connection to that host
    has completed, and we are about to accept a new one. When this code was
    later in the sequence, a new connection could be rejected, even though an
    old one had just finished. Preserve the errno from any select() failure for
    the use of the common select/accept error processing below. */

    select_errno = errno;
    handle_ending_processes();
    errno = select_errno;

    /* Loop for all the sockets that are currently ready to go. If select
    actually failed, we have set the count to 1 and select_failed=TRUE, so as
    to use the common error code for select/accept below. */

    while (lcount-- > 0)
      {
      int accept_socket = -1;
      if (!select_failed)
        {
        for (sk = 0; sk < listen_socket_count; sk++)
          {
          if (FD_ISSET(listen_sockets[sk], &select_listen))
            {
            len = sizeof(accepted);
            accept_socket = accept(listen_sockets[sk],
              (struct sockaddr *)&accepted, &len);
            FD_CLR(listen_sockets[sk], &select_listen);
            break;
            }
          }
        }

      /* If select or accept has failed and this was not caused by an
      interruption, log the incident and try again. With asymmetric TCP/IP
      routing errors such as "No route to network" have been seen here. Also
      "connection reset by peer" has been seen. These cannot be classed as
      disastrous errors, but they could fill up a lot of log. The code in smail
      crashes the daemon after 10 successive failures of accept, on the grounds
      that some OS fail continuously. Exim originally followed suit, but this
      appears to have caused problems. Now it just keeps going, but instead of
      logging each error, it batches them up when they are continuous. */

      if (accept_socket < 0 && errno != EINTR)
        {
        if (accept_retry_count == 0)
          {
          accept_retry_errno = errno;
          accept_retry_select_failed = select_failed;
          }
        else
          {
          if (errno != accept_retry_errno ||
              select_failed != accept_retry_select_failed ||
              accept_retry_count >= 50)
            {
            log_write(0, LOG_MAIN | ((accept_retry_count >= 50)? LOG_PANIC : 0),
              "%d %s() failure%s: %s",
              accept_retry_count,
              accept_retry_select_failed? "select" : "accept",
              (accept_retry_count == 1)? "" : "s",
              strerror(accept_retry_errno));
            log_close_all();
            accept_retry_count = 0;
            accept_retry_errno = errno;
            accept_retry_select_failed = select_failed;
            }
          }
        accept_retry_count++;
        }

      else
        {
        if (accept_retry_count > 0)
          {
          log_write(0, LOG_MAIN, "%d %s() failure%s: %s",
            accept_retry_count,
            accept_retry_select_failed? "select" : "accept",
            (accept_retry_count == 1)? "" : "s",
            strerror(accept_retry_errno));
          log_close_all();
          accept_retry_count = 0;
          }
        }

      /* If select/accept succeeded, deal with the connection. */

      if (accept_socket >= 0)
        handle_smtp_call(listen_sockets, listen_socket_count, accept_socket,
          (struct sockaddr *)&accepted);
      }
    }

  /* If not listening, then just sleep for the queue interval. If we woke
  up early the last time for some other signal, it won't matter because
  the alarm signal will wake at the right time. This code originally used
  sleep() but it turns out that on the FreeBSD system, sleep() is not inter-
  rupted by signals, so it wasn't waking up for SIGALRM or SIGCHLD. Luckily
  select() can be used as an interruptible sleep() on all versions of Unix. */

  else
    {
    struct timeval tv;
    tv.tv_sec = queue_interval;
    tv.tv_usec = 0;
    select(0, NULL, NULL, NULL, &tv);
    handle_ending_processes();
    }

  /* Re-enable the SIGCHLD handler if it has been run. It can't do it
  for itself, because it isn't doing the waiting itself. */

  if (sigchld_seen)
    {
    sigchld_seen = FALSE;
    os_non_restarting_signal(SIGCHLD, main_sigchld_handler);
    }

  /* Handle being woken by SIGHUP. We know at this point that the result
  of accept() has been dealt with, so we can re-exec exim safely, first
  closing the listening sockets so that they can be reused. Cancel any pending
  alarm in case it is just about to go off, and set SIGHUP to be ignored so
  that another HUP in quick succession doesn't clobber the new daemon before it
  gets going. All log files get closed by the close-on-exec flag; however, if
  the exec fails, we need to close the logs. */

  if (sighup_seen)
    {
    int sk;
    log_write(0, LOG_MAIN, "pid %d: SIGHUP received: re-exec daemon",
      getpid());
    for (sk = 0; sk < listen_socket_count; sk++)
      (void)close(listen_sockets[sk]);
    alarm(0);
    signal(SIGHUP, SIG_IGN);
    sighup_argv[0] = exim_path;
    exim_nullstd();
    execv(CS exim_path, (char *const *)sighup_argv);
    log_write(0, LOG_MAIN|LOG_PANIC_DIE, "pid %d: exec of %s failed: %s",
      getpid(), exim_path, strerror(errno));
    log_close_all();
    }

  }   /* End of main loop */

/* Control never reaches here */
}