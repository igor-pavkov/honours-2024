void enc_freeifaddrs(struct ifaddrs *ifa) {
  asylo::host_call::FreeDeserializedIfAddrs(ifa);
}