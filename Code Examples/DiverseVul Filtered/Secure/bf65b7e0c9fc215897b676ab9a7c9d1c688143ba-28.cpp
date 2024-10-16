static void unit_update_on_console(Unit *u) {
        bool b;

        assert(u);

        b = unit_needs_console(u);
        if (u->on_console == b)
                return;

        u->on_console = b;
        if (b)
                manager_ref_console(u->manager);
        else
                manager_unref_console(u->manager);
}