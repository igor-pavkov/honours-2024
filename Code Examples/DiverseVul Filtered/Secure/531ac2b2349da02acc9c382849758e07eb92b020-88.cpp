static int manager_connect_bus(Manager *m, bool reexecuting) {
        bool try_bus_connect;

        assert(m);

        if (m->test_run)
                return 0;

        try_bus_connect =
                reexecuting ||
                (MANAGER_IS_USER(m) && getenv("DBUS_SESSION_BUS_ADDRESS"));

        /* Try to connect to the buses, if possible. */
        return bus_init(m, try_bus_connect);
}