static void imap_get_message(char *buffer, char **outptr)
{
  size_t len = 0;
  char *message = NULL;

  /* Find the start of the message */
  for(message = buffer + 2; *message == ' ' || *message == '\t'; message++)
    ;

  /* Find the end of the message */
  for(len = strlen(message); len--;)
    if(message[len] != '\r' && message[len] != '\n' && message[len] != ' ' &&
        message[len] != '\t')
      break;

  /* Terminate the message */
  if(++len) {
    message[len] = '\0';
  }

  *outptr = message;
}