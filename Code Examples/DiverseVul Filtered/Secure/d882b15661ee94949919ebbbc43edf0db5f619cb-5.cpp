static void* server_func(void* arg)
{
	int server_fds[PORT_COUNT];
	int client_fd = -1;
	enum { CLIENT_UNKNOWN, CLIENT_HUB, CLIENT_GG, CLIENT_GG_SSL, CLIENT_PROXY } ctype = CLIENT_UNKNOWN;
	int i;
	char buf[4096];
	int len = 0;
	const char welcome_packet[] = { 1, 0, 0, 0, 4, 0, 0, 0, 1, 2, 3, 4 };
	const char login_ok_packet[] = { 3, 0, 0, 0, 0, 0, 0, 0 };
	const char hub_reply[] = "HTTP/1.0 200 OK\r\n\r\n0 0 " HOST_LOCAL ":8074 " HOST_LOCAL "\r\n";
	const char hub_ssl_reply[] = "HTTP/1.0 200 OK\r\n\r\n0 0 " HOST_LOCAL ":443 " HOST_LOCAL "\r\n";
	const char proxy_reply[] = "HTTP/1.0 200 OK\r\n\r\n";
	const char proxy_error[] = "HTTP/1.0 404 Not Found\r\n\r\n404 Not Found\r\n";
#ifdef SERVER_TIMEOUT
	time_t started = 0;
#endif
#ifdef GG_CONFIG_HAVE_GNUTLS
	gnutls_session_t session = NULL;
#endif

	for (i = 0; i < PORT_COUNT; i++) {
		struct sockaddr_in sin;
		socklen_t sin_len = sizeof(sin);
		int value = 1;

		server_fds[i] = socket(AF_INET, SOCK_STREAM, 0);

		if (server_fds[i] == -1) {
			perror("socket");
			failure();
		}

		if (setsockopt(server_fds[i], SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)) == -1) {
			perror("setsockopt");
			failure();
		}

		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = inet_addr(HOST_LOCAL);

		if (bind(server_fds[i], (struct sockaddr*) &sin, sizeof(sin)) == -1) {
			perror("bind");
			failure();
		}

		if (getsockname(server_fds[i], (struct sockaddr*) &sin, &sin_len) == -1) {
			perror("getsockname");
			failure();
		}

		server_ports[i] = ntohs(sin.sin_port);

		if (i != PORT_CLOSED) {
			if (listen(server_fds[i], 1) == -1) {
				perror("listen");
				failure();
			}
		}
	}

	if (pthread_mutex_lock(&server_mutex) != 0) {
		fprintf(stderr, "pthread_mutex_lock failed!\n");
		failure();
	}
	server_init = true;
	if (pthread_cond_signal(&server_cond) != 0) {
		fprintf(stderr, "pthread_cond_signal failed!\n");
		failure();
	}
	if (pthread_mutex_unlock(&server_mutex) != 0) {
		fprintf(stderr, "pthread_mutex_unlock failed!\n");
		failure();
	}

	for (;;) {
		struct timeval tv;
		fd_set rd, wr;
		int max_fd = -1;
		int res;

		tv.tv_sec = 1;
		tv.tv_usec = 0;

		FD_ZERO(&rd);
		FD_ZERO(&wr);

		for (i = 0; i < PORT_COUNT; i++) {
			if (i == PORT_CLOSED)
				continue;

			FD_SET(server_fds[i], &rd);

			if (server_fds[i] > max_fd)
				max_fd = server_fds[i];
		}

		if (client_fd != -1) {
			FD_SET(client_fd, &rd);
				
			if (client_fd > max_fd)
				max_fd = client_fd;
		}

		FD_SET(server_pipe[0], &rd);

		if (server_pipe[0] > max_fd)
			max_fd = server_pipe[0];

		res = select(max_fd + 1, &rd, &wr, NULL, &tv);

		if (res == -1 && errno != EINTR) {
			perror("select");
			failure();
		}
		if (res == -1)
			continue;

#ifdef SERVER_TIMEOUT
		if (client_fd != -1) {
			if (time(NULL) - started > SERVER_TIMEOUT) {
				debug("Server timeout!\n");
#ifdef GG_CONFIG_HAVE_GNUTLS
				server_ssl_deinit(&session);
#endif
				if (close(client_fd) == -1) {
					perror("close");
					failure();
				}
				client_fd = -1;
				continue;
			}
		}
#endif

		if (client_fd != -1 && FD_ISSET(client_fd, &rd)) {
			int res;
			test_param_t *test;

			test = get_test_param();

#ifdef GG_CONFIG_HAVE_GNUTLS
			if (ctype == CLIENT_GG_SSL)
				res = gnutls_record_recv(session, buf + len, sizeof(buf) - len - 1);
			else
#endif
				res = recv(client_fd, buf + len, sizeof(buf) - len - 1, 0);

			if (res < 1) {
#ifdef GG_CONFIG_HAVE_GNUTLS
				server_ssl_deinit(&session);
#endif
				if (close(client_fd) == -1) {
					perror("close");
					failure();
				}
				client_fd = -1;
				continue;
			}

			buf[len + res] = 0;
			len += res;

			switch (ctype) {
				case CLIENT_UNKNOWN:
					break;

				case CLIENT_HUB:
					if (strstr(buf, "\r\n\r\n") != NULL) {
						if (!test->ssl_mode) {
							if (send(client_fd, hub_reply, strlen(hub_reply), 0) != strlen(hub_reply)) {
								fprintf(stderr, "send() not completed\n");
								failure();
							}
						} else {
							if (send(client_fd, hub_ssl_reply, strlen(hub_ssl_reply), 0) != strlen(hub_ssl_reply)) {
								fprintf(stderr, "send() not completed\n");
								failure();
							}
						}
						if (close(client_fd) == -1) {
							perror("close");
							failure();
						}
						client_fd = -1;
					}
					break;

				case CLIENT_GG:
					if (len > 8 && len >= get32(buf + 4)) {
						if (send(client_fd, login_ok_packet, sizeof(login_ok_packet), 0) != sizeof(login_ok_packet)) {
							fprintf(stderr, "send() not completed\n");
							failure();
						}
					}
					break;

				case CLIENT_GG_SSL:
#ifdef GG_CONFIG_HAVE_GNUTLS
					if (len > 8 && len >= get32(buf + 4)) {
						if (gnutls_record_send(session, login_ok_packet, sizeof(login_ok_packet)) != sizeof(login_ok_packet)) {
							fprintf(stderr, "gnutls_record_send() not completed\n");
							failure();
						}
					}
#endif
					break;

				case CLIENT_PROXY:
					if (strstr(buf, "\r\n\r\n") != NULL) {
						test_param_t *test;

						test = get_test_param();

						if (strncmp(buf, "GET http://" GG_APPMSG_HOST, strlen("GET http://" GG_APPMSG_HOST)) == 0) {
							test->tried_80 = 1;
							if (test->plug_80 == PLUG_NONE) {
								if (!test->ssl_mode) {
									if (send(client_fd, hub_reply, strlen(hub_reply), 0) != strlen(hub_reply)) {
										fprintf(stderr, "send() not completed\n");
										failure();
									}
								} else {
									if (send(client_fd, hub_ssl_reply, strlen(hub_ssl_reply), 0) != strlen(hub_ssl_reply)) {
										fprintf(stderr, "send() not completed\n");
										failure();
									}
								}
							} else {
								if (send(client_fd, proxy_error, strlen(proxy_error), 0) != strlen(proxy_error)) {
									fprintf(stderr, "send() not completed\n");
									failure();
								}
							}
							if (close(client_fd) == -1) {
								perror("close");
								failure();
							}
							client_fd = -1;
						} else if (strncmp(buf, "CONNECT " HOST_LOCAL ":443 ", strlen("CONNECT " HOST_LOCAL ":443 ")) == 0) {
							test->tried_443 = 1;

							if (test->plug_443 == PLUG_NONE) {
								if (send(client_fd, proxy_reply, strlen(proxy_reply), 0) != strlen(proxy_reply)) {
									fprintf(stderr, "send() not completed\n");
									failure();
								}

#ifdef GG_CONFIG_HAVE_GNUTLS
								if (test->ssl_mode) {
									int res;

									res = server_ssl_init(&session, client_fd);
									if (res != GNUTLS_E_SUCCESS) {
										debug("Handshake failed: %d, %s\n", res, gnutls_strerror(res));
										if (close(client_fd) == -1) {
											perror("close");
											failure();
										}
										client_fd = -1;
										continue;
									}

									if (gnutls_record_send(session, welcome_packet, sizeof(welcome_packet)) != sizeof(welcome_packet)) {
										fprintf(stderr, "gnutls_record_send() not completed\n");
										failure();
									}

									ctype = CLIENT_GG_SSL;
								} else
#endif
								{
									if (send(client_fd, welcome_packet, sizeof(welcome_packet), 0) != sizeof(welcome_packet)) {
										fprintf(stderr, "send() not completed\n");
										failure();
									}
									ctype = CLIENT_GG;
								}
							} else {
								if (send(client_fd, proxy_error, strlen(proxy_error), 0) != strlen(proxy_error)) {
									fprintf(stderr, "send() not completed\n");
									failure();
								}
							}
							len = 0;
						} else {
							debug("Invalid proxy request");
							if (send(client_fd, proxy_error, strlen(proxy_error), 0) != strlen(proxy_error)) {
								fprintf(stderr, "send() not completed\n");
								failure();
							}
							if (close(client_fd) == -1) {
								perror("close");
								failure();
							}
							client_fd = -1;
						}
					}
					break;
			}
		}

		for (i = 0; i < PORT_COUNT; i++) {
			if (i == PORT_CLOSED)
				continue;

			if (FD_ISSET(server_fds[i], &rd)) {
				struct sockaddr_in sin;
				socklen_t sin_len = sizeof(sin);
				int new_fd;

				if ((new_fd = accept(server_fds[i], (struct sockaddr*) &sin, &sin_len)) == -1) {
					perror("accept");
					failure();
				}

				if (client_fd != -1) {
					debug("Overlapping connections\n");
					if (close(new_fd) == -1 || close(client_fd) == -1) {
						perror("close");
						failure();
					}
					client_fd = -1;
					continue;
				}

				client_fd = new_fd;
				memset(buf, 0, sizeof(buf));
				len = 0;
#ifdef SERVER_TIMEOUT
				started = time(NULL);
#endif

				if (i == PORT_80)
					ctype = CLIENT_HUB;
#ifdef GG_CONFIG_HAVE_GNUTLS
				else if (i == PORT_443 && get_test_param()->ssl_mode) {
					int res;

					ctype = CLIENT_GG_SSL;
					res = server_ssl_init(&session, client_fd);

					if (res != GNUTLS_E_SUCCESS) {
						debug("Handshake failed: %d, %s\n", res, gnutls_strerror(res));
						if (close(client_fd) == -1) {
							perror("close");
							failure();
						}
						client_fd = -1;
						continue;
					}
						
					if (gnutls_record_send(session, welcome_packet, sizeof(welcome_packet)) != sizeof(welcome_packet)) {
						fprintf(stderr, "gnutls_record_send() not completed\n");
						failure();
					}
				}
#endif 
				else if (i == PORT_443 || i == PORT_8074) {
					ctype = CLIENT_GG;
					if (send(client_fd, welcome_packet, sizeof(welcome_packet), 0) != sizeof(welcome_packet)) {
						fprintf(stderr, "send() not completed\n");
						failure();
					}
				} else if (i == PORT_8080)
					ctype = CLIENT_PROXY;
			}
		}

		if (FD_ISSET(server_pipe[0], &rd))
			break;
	}

	for (i = 0; i < PORT_COUNT; i++)
		if (close(server_fds[i]) == -1) {
			perror("close");
			failure();
		}

	if (client_fd != -1)
		if (close(client_fd) == -1) {
			perror("close");
			failure();
		}

	return NULL;
}