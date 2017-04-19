#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <string.h>

// AP Always Pile
// AF Always Face
// RND Random Player
// INV_AVG Inverse of Average of previous plies
enum struct ptype_t { AP, AF, RND, INV_AVG };

struct pf_player_t {
  ptype_t mytype;
  unsigned int myseed = 1;
  std::deque<int> mydeq; 
  int myhistory = 10;
  int nb_win = 0;

  void init(ptype_t _p, unsigned int _s) {
    mytype = _p; myseed = _s;
  }
  void rinit(unsigned int _s) {
    myseed = _s;
  }
  char* getstrtype() {
    if(mytype == ptype_t::AP) return (char*)"AP";
    if(mytype == ptype_t::AF) return (char*)"AF";
    if(mytype == ptype_t::RND) return (char*)"RND";
    if(mytype == ptype_t::INV_AVG) return (char*)"INV_AGV";
    return (char*)"NONE";
  }
  int play_rand() {
    return rand_r(&myseed)%2;
  }
  void res(int _r) {
    mydeq.push_back(_r);
    while(mydeq.size() >= myhistory) mydeq.pop_front();
  }
  void move(int _r) {
    // what to do with opponent move info ?
  }
  int play_inv_avg() {
    int nb_pile = 0;
    for(int i = 0; i  < mydeq.size(); i++) 
      if(mydeq[i] == 0) nb_pile ++;
    double ratio = ((double)nb_pile)/mydeq.size();
    if(ratio >= 0.5) return 1;
    return 0;
  }
  int genmove() {
    if(mytype == ptype_t::AP) return 0;
    if(mytype == ptype_t::AF) return 1;
    if(mytype == ptype_t::RND) return play_rand();
    if(mytype == ptype_t::INV_AVG) return play_inv_avg();
    return 0;
  }
};

// AP Always Pile
// AF Always Face
// RND Random Player
// INV_AVG Inverse of Average of previous plies
enum struct pf_parser_type_t { PLAYER, ARBITER};

struct pf_parser_t {
  bool pf_alive;
  pf_parser_type_t parser;
  pf_player_t player;
  char playername[512];
  
  pf_parser_t(pf_parser_type_t _pfp, ptype_t _pt, char* _name) {
    pf_alive = true;
    parser = _pfp;
    strncpy(playername, _name, 512);
    player.init(_pt, 1);
  }
  bool vcmd_common_parser(std::vector<std::string> _v) {
    if(_v[0].compare("help") == 0) {
      fprintf(stderr, "  quit\n");
      fprintf(stderr, "  name\n");
      fprintf(stderr, "  version\n");
      fprintf(stderr, "  genmove\n");
      fprintf(stderr, "  endgame SOL\n");
      fflush(stderr);
      return true;
    }
    if(_v[0].compare("quit") == 0) {
      fprintf(stdout, "= \n\n"); pf_alive = false; return true;
    } 
    if(_v[0].compare("name") == 0) {
      fprintf(stdout, "= %s\n\n", playername); return true;
    } 
    if(_v[0].compare("version") == 0) {
      fprintf(stdout, "= 1.0\n\n"); return true;
    }
    if(_v[0].compare("genmove") == 0) {
      fprintf(stdout, "= %d\n\n", player.genmove()); return true;
    }
    if(_v[0].compare("endgame") == 0) {
      int end_res = -1;
      if(_v.size() != 2) { fprintf(stdout, "= ?\n\n"); return true; }
      if(_v.size() == 2) { end_res = strtol(_v[1].c_str(), 0, 10); }
      player.res(end_res); fprintf(stdout, "= \n\n"); return true;
    }
    return false;
  }
  bool vcmd_player_parser(std::vector<std::string> _v) {
    if(_v[0].compare("help") == 0) {
      fprintf(stderr, "  move VAL\n");
      fflush(stderr);
      fprintf(stdout, "=\n\n");
      fflush(stdout);
      return true;
    }
    if(_v[0].compare("move") == 0) {
      int new_move = -1;
      if(_v.size() != 2) { fprintf(stdout, "= ?\n\n"); return true; }
      if(_v.size() == 2) { new_move = strtol(_v[1].c_str(), 0, 10); }
      player.move(new_move); fprintf(stdout, "= \n\n"); return true;
    }
    return false;
  }
  bool vcmd_arbiter_parser(std::vector<std::string> _v) {
    if(_v[0].compare("help") == 0) {
      fprintf(stderr, "  init VAL\n");
      fflush(stderr);
      fprintf(stdout, "=\n\n");
      fflush(stdout);
      return true;
    }
    if(_v[0].compare("init") == 0) {
      int new_init = -1;
      if(_v.size() != 2) { fprintf(stdout, "= ?\n\n"); return true; }
      if(_v.size() == 2) { new_init = strtol(_v[1].c_str(), 0, 10); }
      player.rinit(new_init); fprintf(stdout, "= \n\n"); return true;
    }
    return false;
  }
  void vcmd_parser(std::vector<std::string> _v) {
    if(parser == pf_parser_type_t::PLAYER) {
      vcmd_common_parser(_v);
      vcmd_player_parser(_v);
    } else if(parser == pf_parser_type_t::ARBITER) {
      vcmd_common_parser(_v);
      vcmd_arbiter_parser(_v);
    } else {
      fprintf(stderr, "vcmd_parser parser_id %d ERROR\n", (int)parser);
    }
  }
  void stdin_text_parser() {
    std::string command;
    while(pf_alive == true) {
      getline(std::cin, command);
      if(0 != (int)command.length()) {
	if(command.c_str()[0] != '#') {
	  char* cstr = new char [command.size()+1];
	  strcpy (cstr, command.c_str());
	  char* cstr2 = strtok(cstr, " ");
	  std::vector<std::string> vcmd;
	  while(cstr2 != 0) {
	    vcmd.push_back(std::string(cstr2));
	    cstr2 = strtok(0, " ");
	  }
	  delete cstr;
	  vcmd_parser(vcmd);
	}
      }
    }
  }
};

/* 
 * g++ player-prg.cpp -std=c++11 -DAP_PLAYER
 * g++ player-prg.cpp -std=c++11 -DRND_PLAYER
 * g++ player-prg.cpp -std=c++11 -DARB
 */
int main(int _ac, char** _av) {
  setbuf(stdout, 0);
  setbuf(stderr, 0);
#ifdef AP_PLAYER
  pf_parser_t P(pf_parser_type_t::PLAYER, ptype_t::AP, _av[0]);
  P.stdin_text_parser();
#endif
#ifdef RND_PLAYER
  pf_parser_t P(pf_parser_type_t::PLAYER, ptype_t::RND, _av[0]);
  P.stdin_text_parser();
#endif
#ifdef ARB
  pf_parser_t P(pf_parser_type_t::ARBITER, ptype_t::AP, _av[0]);
  P.stdin_text_parser();
#endif
  return 0;
}
