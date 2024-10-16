bool FromkLinuxUtsName(const struct klinux_utsname *input,
                       struct utsname *output) {
  if (!input || !output) {
    return false;
  }

  if (!CStringCopy(input->sysname, output->sysname, sizeof(output->sysname)) ||
      !CStringCopy(input->nodename, output->nodename,
                   sizeof(output->nodename)) ||
      !CStringCopy(input->release, output->release, sizeof(output->release)) ||
      !CStringCopy(input->version, output->version, sizeof(output->version)) ||
      !CStringCopy(input->machine, output->machine, sizeof(output->machine))) {
    return false;
  }

#if (defined(__USE_GNU) && __USE_GNU) || \
    (defined(__GNU_VISIBLE) && __GNU_VISIBLE)
  if (!CStringCopy(input->domainname, output->domainname,
                   sizeof(output->domainname))) {
    return false;
  }
#else
  if (!CStringCopy(input->__domainname, output->domainname,
                   sizeof(output->domainname))) {
    return false;
  }
#endif
  return true;
}