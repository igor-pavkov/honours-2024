uint32 unix_dev_minor(SMB_DEV_T dev)
{
#if defined(HAVE_DEVICE_MINOR_FN)
        return (uint32)minor(dev);
#else
        return (uint32)(dev & 0xff);
#endif
}