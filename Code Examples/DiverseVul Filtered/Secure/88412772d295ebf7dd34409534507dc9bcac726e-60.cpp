XMLRPC_SERVER XMLRPC_ServerCreate() {
   XMLRPC_SERVER server = calloc(1, sizeof(STRUCT_XMLRPC_SERVER));
   if(server) {
      Q_Init(&server->methodlist);
      Q_Init(&server->docslist);

      /* register system methods */
      xsm_register(server);
   }
   return server;
}