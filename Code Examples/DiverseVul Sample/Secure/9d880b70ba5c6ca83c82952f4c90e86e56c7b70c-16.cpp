static bool security_info_runs_privileged(const struct security_info *i)  {
        assert(i);

        if (STRPTR_IN_SET(i->user, "0", "root"))
                return true;

        if (i->dynamic_user)
                return false;

        return isempty(i->user);
}