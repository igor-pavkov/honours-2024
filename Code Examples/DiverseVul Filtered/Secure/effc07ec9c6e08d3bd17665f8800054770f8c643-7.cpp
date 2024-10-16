static LogEst estLog(LogEst N){
  return N<=10 ? 0 : sqlite3LogEst(N) - 33;
}