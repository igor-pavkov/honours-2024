static void list_engines (const struct curl_slist *engines)
{
  puts ("Build-time engines:");
  if (!engines) {
    puts ("  <none>");
    return;
  }
  for ( ; engines; engines = engines->next)
    printf ("  %s\n", engines->data);
}