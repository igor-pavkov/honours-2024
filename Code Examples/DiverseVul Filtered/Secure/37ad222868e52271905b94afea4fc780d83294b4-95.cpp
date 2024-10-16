void doopts(char *args)
{
    char *cmdopts;

    if ((cmdopts = strchr(args, ' ')) != NULL) {
        cmdopts++;
        (void) cmdopts;
    }
    if (strncasecmp("mlst ", args, 5) == 0) {
        addreply_noformat(200, " MLST OPTS "
                          "type;size;sizd;modify;UNIX.mode;UNIX.uid;"
                          "UNIX.gid;unique;");
        return;
    }
    addreply_noformat(504, MSG_UNKNOWN_COMMAND);
}