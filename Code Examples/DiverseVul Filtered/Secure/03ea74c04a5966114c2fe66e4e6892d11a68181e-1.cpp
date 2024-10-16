static auth_result url_add_listener (auth_client *auth_user)
{
    client_t *client = auth_user->client;
    auth_t *auth = client->auth;
    auth_url *url = auth->state;
    int res = 0, port;
    const char *agent;
    char *user_agent, *username, *password;
    const char *mountreq;
    char *mount, *ipaddr, *server;
    ice_config_t *config;
    char *userpwd = NULL, post [4096];
    ssize_t post_offset;
    char *pass_headers, *cur_header, *next_header;
    const char *header_val;
    char *header_valesc;

    if (url->addurl == NULL)
        return AUTH_OK;

    config = config_get_config ();
    server = util_url_escape (config->hostname);
    port = config->port;
    config_release_config ();

    agent = httpp_getvar (client->parser, "user-agent");
    if (agent)
        user_agent = util_url_escape (agent);
    else
        user_agent = strdup ("-");

    if (client->username)
        username = util_url_escape (client->username);
    else
        username = strdup ("");

    if (client->password)
        password = util_url_escape (client->password);
    else
        password = strdup ("");

    /* get the full uri (with query params if available) */
    mountreq = httpp_getvar (client->parser, HTTPP_VAR_RAWURI);
    if (mountreq == NULL)
        mountreq = httpp_getvar (client->parser, HTTPP_VAR_URI);
    mount = util_url_escape (mountreq);
    ipaddr = util_url_escape (client->con->ip);

    post_offset = snprintf (post, sizeof (post),
            "action=listener_add&server=%s&port=%d&client=%lu&mount=%s"
            "&user=%s&pass=%s&ip=%s&agent=%s",
            server, port, client->con->id, mount, username,
            password, ipaddr, user_agent);
    free (server);
    free (mount);
    free (user_agent);
    free (username);
    free (password);
    free (ipaddr);

    if (post_offset <= 0 || post_offset >= sizeof(post)) {
        ICECAST_LOG_ERROR("POST body too long for buffer on mount point \"%H\" client %p.", auth_user->mount, client);
        return AUTH_FAILED;
    }

    pass_headers = NULL;
    if (url->pass_headers)
        pass_headers = strdup (url->pass_headers);
    if (pass_headers)
    {
        cur_header = pass_headers;
        while (cur_header)
        {
	    next_header = strstr (cur_header, ",");
	    if (next_header)
	    {
		*next_header=0;
                next_header++;
	    }

            header_val = httpp_getvar (client->parser, cur_header);
            if (header_val)
            {
                size_t left = sizeof(post) - post_offset;
                int ret;
                header_valesc = util_url_escape (header_val);
                ret = snprintf (post+post_offset, left, "&%s%s=%s",
                                         url->prefix_headers ? url->prefix_headers : "",
                                         cur_header, header_valesc);
                free (header_valesc);

                if (ret <= 0 || ret >= left) {
                    ICECAST_LOG_ERROR("POST body too long for buffer on mount point \"%H\" client %p.", auth_user->mount, client);
                    free(pass_headers);
                    return AUTH_FAILED;
                } else {
                    post_offset += ret;
                }
            }

	    cur_header = next_header;
        }
        free(pass_headers);
    }

    if (strchr (url->addurl, '@') == NULL)
    {
        if (url->userpwd)
            curl_easy_setopt (url->handle, CURLOPT_USERPWD, url->userpwd);
        else
        {
            /* auth'd requests may not have a user/pass, but may use query args */
            if (client->username && client->password)
            {
                size_t len = strlen (client->username) + strlen (client->password) + 2;
                userpwd = malloc (len);
                snprintf (userpwd, len, "%s:%s", client->username, client->password);
                curl_easy_setopt (url->handle, CURLOPT_USERPWD, userpwd);
            }
            else
                curl_easy_setopt (url->handle, CURLOPT_USERPWD, "");
        }
    }
    else
    {
        /* url has user/pass but libcurl may need to clear any existing settings */
        curl_easy_setopt (url->handle, CURLOPT_USERPWD, "");
    }
    curl_easy_setopt (url->handle, CURLOPT_URL, url->addurl);
    curl_easy_setopt (url->handle, CURLOPT_POSTFIELDS, post);
    curl_easy_setopt (url->handle, CURLOPT_WRITEHEADER, auth_user);
    url->errormsg[0] = '\0';

    res = curl_easy_perform (url->handle);

    free (userpwd);

    if (res)
    {
        ICECAST_LOG_WARN("auth to server %s failed with %s", url->addurl, url->errormsg);
        return AUTH_FAILED;
    }
    /* we received a response, lets see what it is */
    if (client->authenticated)
        return AUTH_OK;
    ICECAST_LOG_INFO("client auth (%s) failed with \"%s\"", url->addurl, url->errormsg);
    return AUTH_FAILED;
}