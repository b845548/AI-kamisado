#! /usr/bin/env pike

//pike pfsnntp.pike -f ./ap_player -s ./rnd_player -a ./arb -b 1 -n 10 -v 1

#define DUMP_GTP_PIPES		0

class pfsnntp_server {
  int server_is_up;
  private Stdio.File file_out;
  private Stdio.FILE file_in;
  string command_line;
  string full_engine_name;

  void create(string _command_line) {
    file_out = Stdio.File();
    file_in = Stdio.FILE();
    command_line = _command_line;
    array error = catch { 
	Process.create_process(command_line / " ",
			       ([ "stdin" : file_out->pipe(),
				  "stdout" : file_in->pipe() ])); };
    if (error) {
      werror(error[0]); werror("Command line was `%s'.\n", command_line);
      destruct(this_object());
    } else {
      array error = catch {
	  full_engine_name = get_full_engine_name(); server_is_up = 1; };
      if (error) {
	werror("Engine `%s' crashed at startup.\nPerhaps command line is wrong.\n",
	       command_line);
	destruct(this_object());
      }
    }
  }
  
  array send_command(string command) {
#if DUMP_GTP_PIPES
    werror("[%s%s] %s\n", full_engine_name ? full_engine_name + ", " : "", command);
#endif
    command = String.trim_all_whites(command);
    sscanf(command, "%[0-9]", string id);
    if (command[0] == '#' || command == id) return ({ 0, "" });
    file_out->write("%s\n", command);
    string response = file_in->gets();
    if (!response) {
      server_is_up = 0;
      error("Engine `%s' playing crashed!", command_line);
    }
#if DUMP_GTP_PIPES
    werror("%s\n", response);
#endif
    array result;
    int id_length = strlen(id);
    if (response && response[..id_length] == "=" + id)
      result = ({ 0, response[id_length + 1 ..] });
    else if (response && response[..id_length] == "?" + id)
      result = ({ 1, response[id_length + 1 ..] });
    else
      result = ({ -1, response });
    result[1] = String.trim_all_whites(result[1]);
    while (1) {
      response = file_in->gets();
#if DUMP_GTP_PIPES
      werror("%s\n", response);
#endif
      if (response == "") {
	if (result[0] < 0) {
	  werror("Warning, unrecognized response to command `%s':\n", command);
	  werror("%s\n", result[1]);
	}
	return result;
      }
      result[1] += "\n" + response;
    }
  }
  string get_full_engine_name() {
    return send_command("name")[1] + " " + send_command("version")[1];
  }
  string generate_move() {
    return send_command("genmove ")[1];
  }
  void init(int _val) {
    send_command("init "+_val);
  }
  void move(string _movestr) {
    send_command("move " +_movestr);
  }
  void endgame(int _res) {
    send_command("endgame "+_res);
  }
  void quit() {
    send_command("quit");
  }
};


class pfsnntp_game {
  private pfsnntp_server p0;
  private pfsnntp_server p1;
  public pfsnntp_server arbiter;
  private int verbose;

  public int nb_try;

  public string p0_name;
  public int p0_result;
  public int p0_wins;

  public string p1_name;
  public int p1_result;
  public int p1_wins;

  void create(string command_line_player0, string command_line_player1,
	      string command_line_arbiter, int _verbose) {
    verbose = _verbose;
    p0 = pfsnntp_server(command_line_player0);
    if (p0) p1 = pfsnntp_server(command_line_player1);
    if (p0 && p1) arbiter = pfsnntp_server(command_line_arbiter);
    if (!p0 || !p1 || !arbiter) {
      werror("!p0 || !p1 || !arbiter"); finalize(); exit(0);
    }
    
    nb_try = 0; 
    p0_name = command_line_player0; p0_result = 0; p0_wins = 0;
    p1_name = command_line_player1; p1_result = 0; p1_wins = 0;
  }

  void print(string file_name) {
    Stdio.File o = Stdio.File();
    if(!o->open(file_name,"wct")) {
        write("Failed to open file.\n"); return;
    }

    o->write("  nb_try             "+nb_try+"\n"); 
    o->write("--- p0 :\n");
    o->write("      name     : "+p0_name+"\n");
    o->write("      wins     : "+p0_wins+"\n");
    o->write("--- p1 :\n");
    o->write("      name     : "+p1_name+"\n");
    o->write("      wins     : "+p1_wins+"\n");
    o->close();
  }

  void printScore(string file_name) {
    Stdio.File o = Stdio.File();
    if(!o->open(file_name,"wac")) {
        write("Failed to open file.\n");
        return;
    }
    if(p0_result == 1) {
      o->write(" "+p0_name+" win\n");
    } else {
      o->write(" "+p0_name+" loss\n");
    }
    if(p1_result == 1) {
      o->write(" "+p1_name+" win\n");
    } else {
      o->write(" "+p1_name+" loss\n");
    }
    o->write("p0_win: "+p0_wins+" p1_wins: "+p1_wins+"\n");
    o->close();
  }

  void start_new_game(int _i) {
    p0_result = 0;
    p1_result = 0;
    arbiter->init(_i);
  }

  void play() {
    if (verbose)
      werror("\nBeginning a new game.\n");

    // perform a match
    string a_move = arbiter->generate_move();
    string p0_move = p0->generate_move();
    string p1_move = p1->generate_move();
    if(verbose)
      werror("moves  === arb:"+a_move+"   p0:"+p0_move+"   p1:"+p1_move+"\n");
    // communicate opponent move to player
    p0->move(p1_move);
    p1->move(p0_move);
    sleep(1);
    
    int a_val = -1;
    sscanf(a_move, "%d", a_val);
    int p0_val = -1;
    sscanf(p0_move, "%d", p0_val);
    int p1_val = -1;
    sscanf(p1_move, "%d", p1_val);
    
    if(a_val == p0_val) p0_result = 1;
    if(a_val == p1_val) p1_result = 1;

    p0->move(p1_move); p0->endgame(a_val);
    p1->move(p0_move); p1->endgame(a_val);
    nb_try ++;
  }

  void finalize() {
    p0->quit(); p1->quit(); arbiter->quit();
  }
}

void run_match(pfsnntp_game game, int nb_games, int verbose, int base_game) {

  for (int k = 0; k < nb_games; k++) {
    game->start_new_game(k+base_game);
    game->play();
    if(game->p0_result == 1) {
      werror("================= player1 WIN\n");
      game->p0_wins ++;
    } 
    if(game->p1_result == 1) {
      werror("================= player2 WIN\n");
      game->p1_wins ++;
    } 
    if(game->p0_result == 0 && game->p1_result == 0) {
      werror("================= noone WIN\n");
    }
    game->printScore("data/scores.txt");
    game->print("data/result.txt");
  }
  game->finalize();
}

string help_message =
  "Usage: %s [OPTION]... [FILE]...\n\n"
  "Runs either a match or endgame contest between two GTP engines.\n"
  "`--white' and `--black' options are mandatory.\n\n"
  "Options:\n"
  "  -b, --board=BOARD_NUMBER      the board on with programs are playing\n"
  "  -n, --number=NB_GAMES         the number of games to play\n"
  "  -f, --first=COMMAND_LINE\n"
  "  -s, --second=COMMAND_LINE     command lines to run the two engines with.\n\n"
  "  -a, --arbiter=COMMAND_LINE    command line to run arbiter--program that will\n"
  "                                score disputed games--with.\n"
  "      --help                    display this help and exit.\n"
  "  -v, --verbose=LEVEL           1 - print moves, 2 and higher - draw boards.\n";

int main(int argc, array(string) argv) {
  string hint = sprintf("Try `%s --help' for more information.\n",
			basename(argv[0]));
  if (Getopt.find_option(argv, UNDEFINED, "help")) {
    write(help_message, basename(argv[0]));
    return 0;
  }
  string str_p0 = Getopt.find_option(argv, "f", "first", UNDEFINED, "");
  if (str_p0 == "") {
    werror("First player is not specified.\n" + hint);
    return 1;
  }
  string str_p1 = Getopt.find_option(argv, "s", "second", UNDEFINED, "");
  if (str_p1 == "") {
    werror("Second player is not specified.\n" + hint);
    return 1;
  }
  string arbiter = Getopt.find_option(argv, "a", "arbiter", UNDEFINED, "");
  int verbose = (int) Getopt.find_option(argv, "v", "verbose", UNDEFINED, "0");

  string str_nb_games = Getopt.find_option(argv, "n", "games", UNDEFINED, "");
  string str_base_id = Getopt.find_option(argv, "b", "base", UNDEFINED, "");

  int nb_games = 1;
  if (str_nb_games != "") {
    sscanf(str_nb_games, "%d", nb_games);
    if(nb_games <= 0) nb_games = 1;
  }
  int base_id = 0;
  if(str_base_id != "") {
    sscanf(str_base_id, "%d", base_id);
    if(base_id < 0) base_id = 0;
  }
  pfsnntp_game game = pfsnntp_game(str_p0, str_p1, arbiter, verbose);
  if (game) {
    run_match(game, nb_games, verbose, base_id);
  }
  return 0;
}
