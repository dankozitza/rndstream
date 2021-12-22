//
// rndstream.cpp
//
// Utility function that generates customizable random text.
//
// Example:
//
//  use commands within the scope of each command
//
//       rndstream gen file newfile.txt -s 100MB -o A-z
//       rndstream gen stream -t 5s -w 10 -l 1 -o 0-9
//       rndstream gen stream -t 300ms -w 30 -l 10 -o any
//       rndstream gen seedfile seedcrypt.sf -n 1000
//       rndstream gen seed
//       rndstream set rand_function crand
//       rndstream set seed time|file|<uint>
//       rndstream set seedfile seedcrypt.sf
//       rndstream set saltprompt true
//       rndstream get [env_key]
//
//       // commands usage allows for nested help menus
//       rndstream gen help stream
//
//  CLI functionality:
//
//    - options are removed from command line and placed in configuration file.
//    - commands are evaluated from left to right with functionality
//      partially defined by config.
//    - all options are set in config file. an option only needs to be set once
//      to change the functionality of rndstream.
//    - boolean options must be defined on the command line each time.
//
//  Configuration:
//
//     Use json file /etc/rndstream.json to configure.
//
//  multiple rand functions
//
//  crand - re-seeds every n calls
//  qrand - makes frequent calls to srand
//  grand - filters output of rand by calling srand when finding 'bad' numbers
//  mrand - keeps record of seeds and calls to rand. has accessible json index.
//  lrand - uses dual calls to rand to produce long ints.
//
//  store unsigned int values to be used as seeds
//
//  srandtp
//
//     Substitute for srand that uses time and some extra logic to produce a
//     unique time-based seed.
//     check constants RAND_MAX and UINT_MAX in cstdlib
//
//  unsigned int srandtp() {
//    srand(time);
//    while (rand() % 100 != 1) {}
//    unsigned int rv1 = rand();
//    srand(time + rv1 + salt);
//    return rv1;
//
//    need customized time input to make rand unique to time-based rand.
//    build seed-salt prompt
//    srand(time + 1032883);
//    unsigned int rv1 = rand();
//    while (rand() & 100 < rv1) {}
//    srand(time);
//    unsigned int rv2 = rand();
//    rv1 = rand();
//    srand(rv1);
//    return rv1;
//  }
//
// Created by Daniel Kozitza
//

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <sys/ioctl.h>
#include "tools.hpp"
#include "jconfig.hpp"
#include "options.hpp"
#include "commands.hpp"
#include "sorters.hpp"

using namespace tools;

typedef map<string, string>::iterator map_iter;

void cmd_set(vector<string>& argv);
void cmd_env();

string  pn;
jconfig cfg("rndstream.json");

int main(int argc, char *argv[]) {
   vector<string> Argv(0);
   string         cmd_str;
   options        opt;
   commands       cmds;
   Error          e;
   pn = argv[0];

   signal(SIGINT, signals_callback_handler);

   struct winsize ws;
   ioctl(0, TIOCGWINSZ, &ws);

   cfg.define_uint("opt_s", time(NULL));
   cfg.define_uint("opt_l", ws.ws_row);
   cfg.define_uint("opt_w", ws.ws_col);

   e = cfg.load();
   if (e != NULL) {
      cout << pn << ": Making jconfig file '" << cfg.file_path;
      cout << "'.\n";
      e = cfg.save();
      if (e != NULL) {
         cout << pn << ": Error: " << e << endl;
         return 1;
      }
   }

   opt.handle('s', cfg.m["opt_s"].set, cfg.m["opt_s"].vstr);
   opt.handle('l', cfg.m["opt_l"].set, cfg.m["opt_l"].vstr);
   opt.handle('w', cfg.m["opt_w"].set, cfg.m["opt_w"].vstr);

   for (int i = 1; i < argc; i++)
      Argv.push_back(string(argv[i]));

   e = opt.evaluate(Argv);
   if (e != NULL) {
      cout << pn << ": Error: " << e << endl;
      return 1;
   }
   e = cfg.convert();
   if (e != NULL) {
      cout << pn << ": Error: " << e << endl;
      return 1;
   }
   e = cfg.save();
   if (e != NULL) {
      cout << pn << ": Error: " << e << endl;
      return 1;
   }

   if (Argv.size() == 0) {
      cmd_str = "help";
   }
   else {
      cmd_str = Argv[0];
      Argv.erase(Argv.begin());
   }

   cmds.set_program_name(pn);
   cmds.set_max_line_width(ws.ws_col);
   cmds.set_cmds_help(
      "\n   rndstream is a utility that outputs random text.\n\n"
      "Usage:\n\n   rndstream command [arguments]\n");

   cmds.handle(
      "set",
      cmd_set,
      "Set environment variables.",
      "set <key> <value>");
   cmds.handle(
      "env",
      cmd_env,
      "Displays the configuration file.",
      "env [key]");

   cmds.run(cmd_str, Argv);

   return 0;
}

void cmd_set(vector<string>& argv) {

   return;
}

void cmd_env() {
   cout << cfg.file_path << ":\n" << cfg.getJSON() << "\n";
   return;
}
