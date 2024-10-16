void select_soundhw(const char *optarg)
{
    struct soundhw *c;

    if (is_help_option(optarg)) {
    show_valid_cards:

        if (soundhw_count) {
             printf("Valid sound card names (comma separated):\n");
             for (c = soundhw; c->name; ++c) {
                 printf ("%-11s %s\n", c->name, c->descr);
             }
             printf("\n-soundhw all will enable all of the above\n");
        } else {
             printf("Machine has no user-selectable audio hardware "
                    "(it may or may not have always-present audio hardware).\n");
        }
        exit(!is_help_option(optarg));
    }
    else {
        size_t l;
        const char *p;
        char *e;
        int bad_card = 0;

        if (!strcmp(optarg, "all")) {
            for (c = soundhw; c->name; ++c) {
                c->enabled = 1;
            }
            return;
        }

        p = optarg;
        while (*p) {
            e = strchr(p, ',');
            l = !e ? strlen(p) : (size_t) (e - p);

            for (c = soundhw; c->name; ++c) {
                if (!strncmp(c->name, p, l) && !c->name[l]) {
                    c->enabled = 1;
                    break;
                }
            }

            if (!c->name) {
                if (l > 80) {
                    error_report("Unknown sound card name (too big to show)");
                }
                else {
                    error_report("Unknown sound card name `%.*s'",
                                 (int) l, p);
                }
                bad_card = 1;
            }
            p += l + (e != NULL);
        }

        if (bad_card) {
            goto show_valid_cards;
        }
    }
}