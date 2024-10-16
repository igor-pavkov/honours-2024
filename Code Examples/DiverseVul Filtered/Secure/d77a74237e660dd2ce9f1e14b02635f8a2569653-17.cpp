uint32 unix_dev_major(SMB_DEV_T dev)
{
#if defined(HAVE_DEVICE_MAJOR_FN)
        return (uint32)major(dev);
#else
        return (uint32)(dev >> 8);
#endif
}