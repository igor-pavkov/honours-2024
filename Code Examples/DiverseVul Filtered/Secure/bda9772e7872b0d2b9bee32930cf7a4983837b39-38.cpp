int TokLinuxSyslogPriority(int input) {
  int syslog_level = input & 0x07;
  int syslog_facility = input & ~0x07;
  return TokLinuxSyslogLevel(syslog_level) |
         TokLinuxSyslogFacility(syslog_facility);
}