static void generateOptRR(const std::string& optRData, string& res)
{
  const uint8_t name = 0;
  dnsrecordheader dh;
  EDNS0Record edns0;
  edns0.extRCode = 0;
  edns0.version = 0;
  edns0.Z = 0;
  
  dh.d_type = htons(QType::OPT);
  dh.d_class = htons(1280);
  memcpy(&dh.d_ttl, &edns0, sizeof edns0);
  dh.d_clen = htons((uint16_t) optRData.length());
  res.assign((const char *) &name, sizeof name);
  res.append((const char *) &dh, sizeof dh);
  res.append(optRData.c_str(), optRData.length());
}