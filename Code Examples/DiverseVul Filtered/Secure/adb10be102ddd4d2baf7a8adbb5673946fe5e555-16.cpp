shared_ptr<DNSRecordContent> DNSRecordContent::unserialize(const string& qname, uint16_t qtype, const string& serialized)
{
  dnsheader dnsheader;
  memset(&dnsheader, 0, sizeof(dnsheader));
  dnsheader.qdcount=htons(1);
  dnsheader.ancount=htons(1);

  vector<uint8_t> packet; // build pseudo packet

  /* will look like: dnsheader, 5 bytes, encoded qname, dns record header, serialized data */

  string encoded=EncodeDNSLabel(qname);

  packet.resize(sizeof(dnsheader) + 5 + encoded.size() + sizeof(struct dnsrecordheader) + serialized.size());

  uint16_t pos=0;

  memcpy(&packet[0], &dnsheader, sizeof(dnsheader)); pos+=sizeof(dnsheader);

  char tmp[6]="\x0" "\x0\x1" "\x0\x1"; // root question for ns_t_a
  memcpy(&packet[pos], &tmp, 5); pos+=5;

  memcpy(&packet[pos], encoded.c_str(), encoded.size()); pos+=(uint16_t)encoded.size();

  struct dnsrecordheader drh;
  drh.d_type=htons(qtype);
  drh.d_class=htons(1);
  drh.d_ttl=0;
  drh.d_clen=htons(serialized.size());

  memcpy(&packet[pos], &drh, sizeof(drh)); pos+=sizeof(drh);
  memcpy(&packet[pos], serialized.c_str(), serialized.size()); pos+=(uint16_t)serialized.size();

  MOADNSParser mdp((char*)&*packet.begin(), (unsigned int)packet.size());
  shared_ptr<DNSRecordContent> ret= mdp.d_answers.begin()->first.d_content;
  ret->header.d_type=ret->d_qtype;
  ret->label=mdp.d_answers.begin()->first.d_label;
  ret->header.d_ttl=mdp.d_answers.begin()->first.d_ttl;
  return ret;
}