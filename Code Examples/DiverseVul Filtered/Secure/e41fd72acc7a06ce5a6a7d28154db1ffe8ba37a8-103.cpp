int sqlite3_rtree_geometry_callback(
  sqlite3 *db,                  /* Register SQL function on this connection */
  const char *zGeom,            /* Name of the new SQL function */
  int (*xGeom)(sqlite3_rtree_geometry*,int,RtreeDValue*,int*), /* Callback */
  void *pContext                /* Extra data associated with the callback */
){
  RtreeGeomCallback *pGeomCtx;      /* Context object for new user-function */

  /* Allocate and populate the context object. */
  pGeomCtx = (RtreeGeomCallback *)sqlite3_malloc(sizeof(RtreeGeomCallback));
  if( !pGeomCtx ) return SQLITE_NOMEM;
  pGeomCtx->xGeom = xGeom;
  pGeomCtx->xQueryFunc = 0;
  pGeomCtx->xDestructor = 0;
  pGeomCtx->pContext = pContext;
  return sqlite3_create_function_v2(db, zGeom, -1, SQLITE_ANY, 
      (void *)pGeomCtx, geomCallback, 0, 0, rtreeFreeCallback
  );
}