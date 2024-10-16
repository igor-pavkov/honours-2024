static int get_proc_fs_id(char type)
{
    const char *scanf_format = "%*cid:\t%d\t%d\t%d\t%d\n";
    char id_type[] = "_id";
    id_type[0] = type;

    int real, e_id, saved;
    int fs_id = 0;

    char *line = proc_pid_status; /* never NULL */
    for (;;)
    {
        if (strncmp(line, id_type, 3) == 0)
        {
            int n = sscanf(line, scanf_format, &real, &e_id, &saved, &fs_id);
            if (n != 4)
            {
                perror_msg_and_die("Can't parse %cid: line", type);
            }

            return fs_id;
        }
        line = strchr(line, '\n');
        if (!line)
            break;
        line++;
    }

    perror_msg_and_die("Failed to get file system %cID of the crashed process", type);
}