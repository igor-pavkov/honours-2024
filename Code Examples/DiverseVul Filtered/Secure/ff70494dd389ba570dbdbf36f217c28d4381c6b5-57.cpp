void reset_term() {
  struct termios oldt,
                 newt;
  tcgetattr( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag |= ( ICANON | ECHO );
  tcsetattr( STDIN_FILENO, TCSANOW, &newt );
}