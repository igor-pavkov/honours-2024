static void pptp_init(void)
{
	struct sockaddr_in addr;
	char *opt;
	int fd;

	fd = socket(AF_PPPOX, SOCK_STREAM, PX_PROTO_PPTP);
	if (fd >= 0)
		close(fd);
	else if (system("modprobe -q pptp"))
		log_warn("failed to load pptp kernel module\n");

	serv.hnd.fd = socket(PF_INET, SOCK_STREAM, 0);
	if (serv.hnd.fd < 0) {
		log_emerg("pptp: failed to create server socket: %s\n", strerror(errno));
		return;
	}

	fcntl(serv.hnd.fd, F_SETFD, fcntl(serv.hnd.fd, F_GETFD) | FD_CLOEXEC);

	addr.sin_family = AF_INET;

	opt = conf_get_opt("pptp", "bind");
	if (opt)
		addr.sin_addr.s_addr = inet_addr(opt);
	else
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

	opt = conf_get_opt("pptp", "port");
	if (opt && atoi(opt) > 0)
		addr.sin_port = htons(atoi(opt));
	else
		addr.sin_port = htons(PPTP_PORT);

  setsockopt(serv.hnd.fd, SOL_SOCKET, SO_REUSEADDR, &serv.hnd.fd, 4);
  if (bind (serv.hnd.fd, (struct sockaddr *) &addr, sizeof (addr)) < 0) {
    log_emerg("pptp: failed to bind socket: %s\n", strerror(errno));
		close(serv.hnd.fd);
    return;
  }

  if (listen (serv.hnd.fd, 100) < 0) {
    log_emerg("pptp: failed to listen socket: %s\n", strerror(errno));
		close(serv.hnd.fd);
    return;
  }

	if (fcntl(serv.hnd.fd, F_SETFL, O_NONBLOCK)) {
    log_emerg("pptp: failed to set nonblocking mode: %s\n", strerror(errno));
		close(serv.hnd.fd);
    return;
	}

	conn_pool = mempool_create(sizeof(struct pptp_conn_t));

	load_config();

	triton_context_register(&serv.ctx, NULL);
	triton_md_register_handler(&serv.ctx, &serv.hnd);
	triton_md_enable_handler(&serv.hnd, MD_MODE_READ);
	triton_context_wakeup(&serv.ctx);

	cli_register_simple_cmd2(show_stat_exec, NULL, 2, "show", "stat");

	triton_event_register_handler(EV_CONFIG_RELOAD, (triton_event_func)load_config);
}