static void print_unit_dependency_mask(FILE *f, const char *kind, UnitDependencyMask mask, bool *space) {
        const struct {
                UnitDependencyMask mask;
                const char *name;
        } table[] = {
                { UNIT_DEPENDENCY_FILE,               "file"               },
                { UNIT_DEPENDENCY_IMPLICIT,           "implicit"           },
                { UNIT_DEPENDENCY_DEFAULT,            "default"            },
                { UNIT_DEPENDENCY_UDEV,               "udev"               },
                { UNIT_DEPENDENCY_PATH,               "path"               },
                { UNIT_DEPENDENCY_MOUNTINFO_IMPLICIT, "mountinfo-implicit" },
                { UNIT_DEPENDENCY_MOUNTINFO_DEFAULT,  "mountinfo-default"  },
                { UNIT_DEPENDENCY_PROC_SWAP,          "proc-swap"          },
        };
        size_t i;

        assert(f);
        assert(kind);
        assert(space);

        for (i = 0; i < ELEMENTSOF(table); i++) {

                if (mask == 0)
                        break;

                if (FLAGS_SET(mask, table[i].mask)) {
                        if (*space)
                                fputc(' ', f);
                        else
                                *space = true;

                        fputs(kind, f);
                        fputs("-", f);
                        fputs(table[i].name, f);

                        mask &= ~table[i].mask;
                }
        }

        assert(mask == 0);
}