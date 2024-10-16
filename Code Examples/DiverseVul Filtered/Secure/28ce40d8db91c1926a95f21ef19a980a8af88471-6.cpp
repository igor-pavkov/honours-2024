static int get_fsgid(void)
{
    return get_proc_fs_id(/*GID*/'G');
}