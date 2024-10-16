static void complain(int err, const char *dev) {

  if (complained_err == err && complained_dev && dev &&
		  strcmp(dev, complained_dev) == 0)
    return;

  complained_err = err;
  free(complained_dev);
  complained_dev = xstrdup(dev);

  switch (err) {
    case ENXIO:
      error (_("umount: %s: invalid block device"), dev); break;
    case EINVAL:
      error (_("umount: %s: not mounted"), dev); break;
    case EIO:
      error (_("umount: %s: can't write superblock"), dev); break;
    case EBUSY:
     /* Let us hope fstab has a line "proc /proc ..."
	and not "none /proc ..."*/
      error (_("umount: %s: device is busy.\n"
	       "        (In some cases useful info about processes that use\n"
	       "         the device is found by lsof(8) or fuser(1))"), dev);
      break;
    case ENOENT:
      error (_("umount: %s: not found"), dev); break;
    case EPERM:
      error (_("umount: %s: must be superuser to umount"), dev); break;
    case EACCES:
      error (_("umount: %s: block devices not permitted on fs"), dev); break;
    default:
      error (_("umount: %s: %s"), dev, strerror (err)); break;
  }
}