void manager_set_first_boot(Manager *m, bool b) {
        assert(m);

        if (!MANAGER_IS_SYSTEM(m))
                return;

        if (m->first_boot != (int) b) {
                if (b)
                        (void) touch("/run/systemd/first-boot");
                else
                        (void) unlink("/run/systemd/first-boot");
        }

        m->first_boot = b;
}