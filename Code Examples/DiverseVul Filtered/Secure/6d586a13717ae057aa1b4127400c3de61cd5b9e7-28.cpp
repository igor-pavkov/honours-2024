static int bus_socket_auth_write_ok(sd_bus *b) {
        char t[3 + 32 + 2 + 1];

        assert(b);

        xsprintf(t, "OK " SD_ID128_FORMAT_STR "\r\n", SD_ID128_FORMAT_VAL(b->server_id));

        return bus_socket_auth_write(b, t);
}