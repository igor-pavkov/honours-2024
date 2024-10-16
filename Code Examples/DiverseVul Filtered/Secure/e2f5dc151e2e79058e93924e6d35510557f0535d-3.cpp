readconf_acl(BOOL skip)
{
uschar *p;

/* Not receiving messages, don't need to parse the ACL data */

if (skip)
  {
  DEBUG(D_acl) debug_printf("skipping ACL configuration - not needed\n");
  while ((p = get_config_line()) != NULL);
  return;
  }

/* Read each ACL and add it into the tree. Macro (re)definitions are allowed
between ACLs. */

acl_line = get_config_line();

while(acl_line != NULL)
  {
  uschar name[64];
  tree_node *node;
  uschar *error;

  p = readconf_readname(name, sizeof(name), acl_line);
  if (isupper(*name) && *p == '=')
    {
    read_macro_assignment(acl_line);
    acl_line = get_config_line();
    continue;
    }

  if (*p != ':' || name[0] == 0)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "missing or malformed ACL name");

  node = store_get(sizeof(tree_node) + Ustrlen(name));
  Ustrcpy(node->name, name);
  if (!tree_insertnode(&acl_anchor, node))
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
      "there are two ACLs called \"%s\"", name);

  node->data.ptr = acl_read(acl_callback, &error);

  if (node->data.ptr == NULL && error != NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "error in ACL: %s", error);
  }
}