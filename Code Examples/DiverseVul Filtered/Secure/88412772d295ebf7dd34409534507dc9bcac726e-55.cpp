void XMLRPC_ServerDestroy(XMLRPC_SERVER server) {
   if(server) {
      doc_method* dm = Q_Head(&server->docslist);
      server_method* sm = Q_Head(&server->methodlist);
      while( dm ) {
         my_free(dm);
         dm = Q_Next(&server->docslist);
      }
      while( sm ) {
         if(sm->name) {
            my_free(sm->name);
         }
         if(sm->desc) {
            XMLRPC_CleanupValue(sm->desc);
         }
         my_free(sm);
         sm = Q_Next(&server->methodlist);
      }
      if(server->xIntrospection) {
         XMLRPC_CleanupValue(server->xIntrospection);
      }

      Q_Destroy(&server->methodlist);
      Q_Destroy(&server->docslist);
      my_free(server);
   }
}