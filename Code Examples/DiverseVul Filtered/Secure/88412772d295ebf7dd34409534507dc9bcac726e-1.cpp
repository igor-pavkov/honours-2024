int XMLRPC_ServerRegisterMethod(XMLRPC_SERVER server, const char *name, XMLRPC_Callback cb) {
   if(server && name && cb) {

      server_method* sm = malloc(sizeof(server_method));
      
      if(sm) {
         sm->name = strdup(name);
         sm->method = cb;
         sm->desc = NULL;

         return Q_PushTail(&server->methodlist, sm);
      }
   }
   return 0;
}