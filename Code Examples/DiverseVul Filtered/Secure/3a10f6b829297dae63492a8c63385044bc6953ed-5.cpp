int createPostgresTimeCompareSimple(const char *timecol, const char *timestring, char *dest, size_t destsize)
{
  int timeresolution = msTimeGetResolution(timestring);
  char timeStamp[100];
  char *interval;
  if (timeresolution < 0)
    return MS_FALSE;
  postgresTimeStampForTimeString(timestring,timeStamp,100);

  switch(timeresolution) {
    case TIME_RESOLUTION_YEAR:
      interval = "year";
      break;
    case TIME_RESOLUTION_MONTH:
      interval = "month";
      break;
    case TIME_RESOLUTION_DAY:
      interval = "day";
      break;
    case TIME_RESOLUTION_HOUR:
      interval = "hour";
      break;
    case TIME_RESOLUTION_MINUTE:
      interval = "minute";
      break;
    case TIME_RESOLUTION_SECOND:
      interval = "second";
      break;
    default:
      return MS_FAILURE;
  }
  snprintf(dest, destsize,"(%s >= date_trunc('%s',%s) and %s < date_trunc('%s',%s) + interval '1 %s')",
           timecol, interval, timeStamp, timecol, interval, timeStamp, interval);
  return MS_SUCCESS;
}