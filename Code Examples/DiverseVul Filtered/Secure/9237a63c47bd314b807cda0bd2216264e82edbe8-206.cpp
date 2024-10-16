ofpact_type_from_name(const char *name, enum ofpact_type *type)
{
#define OFPACT(ENUM, STRUCT, MEMBER, NAME)                            \
    if (!strcasecmp(name, NAME)) {                                    \
        *type = OFPACT_##ENUM;                                          \
        return true;                                                    \
    }
    OFPACTS
#undef OFPACT

    return false;
}