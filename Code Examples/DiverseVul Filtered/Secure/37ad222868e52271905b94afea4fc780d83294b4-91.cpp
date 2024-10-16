void domlst(const char * const file)
{
    char line[PATH_MAX + 256U] = MLST_BEGIN;

    if (modernformat(file, line + (sizeof MLST_BEGIN - 1U),
                     sizeof line - (sizeof MLST_BEGIN - 1U), " ") < 0) {
        addreply_noformat(550, MSG_STAT_FAILURE2);
        return;
    }
    addreply_noformat(0, line);
    addreply_noformat(250, "End.");
}