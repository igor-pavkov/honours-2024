void printStats(uint64_t origWaitingFor=0, uint64_t weWaitingFor=0)
{
  format headerfmt   ("%|9t|Questions - Pend. - Drop = Answers = (On time + Late) = (Err + Ok)\n");
  format datafmt("%s%|9t|%d %|21t|%d %|29t|%d %|36t|%d %|47t|%d %|57t|%d %|66t|%d %|72t|%d\n");

  cerr<<headerfmt;
  cerr<<(datafmt % "Orig"   % s_questions % origWaitingFor  % s_orignever  % s_origanswers % 0 % s_origtimedout  % 0 % 0);
  cerr<<(datafmt % "Refer." % s_questions % weWaitingFor    % s_wenever    % s_weanswers   % 0 % s_wetimedout    % 0 % 0);

  cerr<<weWaitingFor<<" queries that could still come in on time, "<<qids.size()<<" outstanding"<<endl;
  
  cerr<<"we late: "<<s_wetimedout<<", orig late: "<< s_origtimedout<<", "<<s_questions<<" questions sent, "<<s_origanswers
      <<" original answers, "<<s_perfect<<" perfect, "<<s_mostly<<" mostly correct"<<", "<<s_webetter<<" we better, "<<s_origbetter<<" orig better ("<<s_origbetterset.size()<<" diff)"<<endl;
  cerr<<"we never: "<<s_wenever<<", orig never: "<<s_orignever<<endl;
  cerr<<"original questions from IP addresses for which recursion was not available: "<<s_norecursionavailable<<endl;
  cerr<<"Unmatched from us: "<<s_weunmatched<<", unmatched from original: "<<s_origunmatched << " ( - decoding err: "<<s_origunmatched-s_origdnserrors<<")"<<endl;
  cerr<<"DNS decoding errors from us: "<<s_wednserrors<<", from original: "<<s_origdnserrors<<", exact duplicates from client: "<<s_duplicates<<endl<<endl;

}