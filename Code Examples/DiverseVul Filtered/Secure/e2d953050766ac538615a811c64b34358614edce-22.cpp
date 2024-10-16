int main(int argc, char **argv)
{
	darray(struct tcmulib_handler) handlers = darray_new();
	struct tcmulib_context *tcmulib_context;
	struct tcmur_handler **tmp_r_handler;
	GMainLoop *loop;
	GIOChannel *libtcmu_gio;
	guint reg_id;
	int ret;

	tcmu_cfg = tcmu_config_new();
	if (!tcmu_cfg)
		exit(1);
	ret = tcmu_load_config(tcmu_cfg, NULL);
	if (ret == -1)
		goto err_out;

	while (1) {
		int option_index = 0;
		int c;

		c = getopt_long(argc, argv, "dhlV",
				long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 0:
			if (option_index == 1)
				handler_path = strdup(optarg);
			break;
		case 'l':
			if (strlen(optarg) > PATH_MAX - TCMU_LOG_FILENAME_MAX) {
				tcmu_err("--tcmu-log-dir='%s' cannot exceed %d characters\n",
				         optarg, PATH_MAX - TCMU_LOG_FILENAME_MAX);
			}
			if (!tcmu_logdir_create(optarg)) {
				goto err_out;
			}
			tcmu_log_dir = strdup(optarg);
			break;
		case 'd':
			tcmu_set_log_level(TCMU_CONF_LOG_DEBUG_SCSI_CMD);
			break;
		case 'V':
			printf("tcmu-runner %s\n", TCMUR_VERSION);
			goto err_out;
		default:
		case 'h':
			usage();
			goto err_out;
		}
	}

	tcmu_dbg("handler path: %s\n", handler_path);

	ret = load_our_module();
	if (ret < 0) {
		tcmu_err("couldn't load module\n");
		goto err_out;
	}

	ret = open_handlers();
	if (ret < 0) {
		tcmu_err("couldn't open handlers\n");
		goto err_out;
	}
	tcmu_dbg("%d runner handlers found\n", ret);

	/*
	 * Convert from tcmu-runner's handler struct to libtcmu's
	 * handler struct, an array of which we pass in, below.
	 */
	darray_foreach(tmp_r_handler, g_runner_handlers) {
		struct tcmulib_handler tmp_handler;

		tmp_handler.name = (*tmp_r_handler)->name;
		tmp_handler.subtype = (*tmp_r_handler)->subtype;
		tmp_handler.cfg_desc = (*tmp_r_handler)->cfg_desc;
		tmp_handler.check_config = (*tmp_r_handler)->check_config;
		tmp_handler.reconfig = (*tmp_r_handler)->reconfig;
		tmp_handler.added = dev_added;
		tmp_handler.removed = dev_removed;

		/*
		 * Can hand out a ref to an internal pointer to the
		 * darray b/c handlers will never be added or removed
		 * once open_handlers() is done.
		 */
		tmp_handler.hm_private = *tmp_r_handler;

		darray_append(handlers, tmp_handler);
	}

	tcmulib_context = tcmulib_initialize(handlers.item, handlers.size);
	if (!tcmulib_context) {
		tcmu_err("tcmulib_initialize failed\n");
		goto err_out;
	}

	loop = g_main_loop_new(NULL, FALSE);
	if (g_unix_signal_add(SIGINT, sighandler, loop) <= 0 ||
	    g_unix_signal_add(SIGTERM, sighandler, loop) <= 0) {
		tcmu_err("couldn't setup signal handlers\n");
		goto err_tcmulib_close;
	}

	/* Set up event for libtcmu */
	libtcmu_gio = g_io_channel_unix_new(tcmulib_get_master_fd(tcmulib_context));
	g_io_add_watch(libtcmu_gio, G_IO_IN, tcmulib_callback, tcmulib_context);

	/* Set up DBus name, see callback */
	reg_id = g_bus_own_name(G_BUS_TYPE_SYSTEM,
				"org.kernel.TCMUService1",
				G_BUS_NAME_OWNER_FLAGS_NONE,
				dbus_bus_acquired,
				dbus_name_acquired, // name acquired
				dbus_name_lost, // name lost
				NULL, // user data
				NULL  // user date free func
		);

	g_main_loop_run(loop);

	tcmu_dbg("Exiting...\n");
	g_bus_unown_name(reg_id);
	g_main_loop_unref(loop);
	tcmulib_close(tcmulib_context);
	tcmu_config_destroy(tcmu_cfg);

	return 0;

err_tcmulib_close:
	tcmulib_close(tcmulib_context);
err_out:
	tcmu_config_destroy(tcmu_cfg);
	exit(1);
}