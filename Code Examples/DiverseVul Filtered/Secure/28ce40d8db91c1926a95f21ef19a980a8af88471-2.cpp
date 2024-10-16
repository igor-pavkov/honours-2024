static int get_fsuid(void)
{
    return get_proc_fs_id(/*UID*/'U');
}