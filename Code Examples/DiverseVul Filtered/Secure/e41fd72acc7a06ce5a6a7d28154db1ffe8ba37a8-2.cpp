int sqlite3_rtree_query_callback(
  sqlite3 *db,                 /* Register SQL function on this connection */
  const char *zQueryFunc,      /* Name of new SQL function */
  int (*xQueryFunc)(sqlite3_rtree_query_info*), /* Callback */
  void *pContext,              /* Extra data passed into the callback */
  void (*xDestructor)(void*)   /* Destructor for the extra data */
){
  RtreeGeomCallback *pGeomCtx;      /* Context object for new user-function */

  /* Allocate and populate the context object. */
  pGeomCtx = (RtreeGeomCallback *)sqlite3_malloc(sizeof(RtreeGeomCallback));
  if( !pGeomCtx ) return SQLITE_NOMEM;
  pGeomCtx->xGeom = 0;
  pGeomCtx->xQueryFunc = xQueryFunc;
  pGeomCtx->xDestructor = xDestructor;
  pGeomCtx->pContext = pContext;
  return sqlite3_create_function_v2(db, zQueryFunc, -1, SQLITE_ANY, 
      (void *)pGeomCtx, geomCallback, 0, 0, rtreeFreeCallback
  );
}