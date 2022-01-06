//
// rndstream.cpp
//
// Command line tool that generates customizable random text.
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
#include <chrono>
#include <thread>
#include "tools.hpp"
#include "jconfig.hpp"
#include "options.hpp"
#include "commands.hpp"
#include "sorters.hpp"

using namespace tools;

void cmd_gen(vector<string>& argv);
void cmd_stream();
void cmd_set(vector<string>& argv);
void cmd_env();

string  pn;
jconfig cfg("/etc/rndstream.json");

int main(int argc, char *argv[]) {
   vector<string> Argv(0);
   string         cmd;
   options        opt;
   commands       cmds;
   Error          e;
   pn = argv[0];

   signal(SIGINT, signals_callback_handler);

   struct winsize ws;
   ioctl(0, TIOCGWINSZ, &ws);

   cfg.define_uint("s", time(NULL));
   cfg.define_uint("l", 0);
   cfg.define_uint("w", 1);
   cfg.define_str("o", " ~");
   cfg.define_dbl("t", 0.5);
   cfg.define_str("c", cfg.file_path);
   cfg.define_btn("x");

   opt.handle('c', cfg.m["c"].set, cfg.m["c"].vstr);
   opt.handle('s', cfg.m["s"].set, cfg.m["s"].vstr);
   opt.handle('l', cfg.m["l"].set, cfg.m["l"].vstr);
   opt.handle('w', cfg.m["w"].set, cfg.m["w"].vstr);
   opt.handle('o', cfg.m["o"].set, cfg.m["o"].vstr);
   opt.handle('t', cfg.m["t"].set, cfg.m["t"].vstr);
   opt.handle('x', cfg.m["x"].set);

   for (int i = 1; i < argc; i++)
      Argv.push_back(string(argv[i]));

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

   e = opt.evaluate(Argv);
   if (e != NULL) {
      cout << pn << ": Error: " << e << endl;
      return 1;
   }

   if (cfg.m["c"].set || cfg.file_path != cfg.get_str("c")) {
      cfg.file_path = cfg.get_str("c");
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
   }

   e = cfg.convert();
   if (e != NULL) {
      cout << pn << ": Error: " << e << endl;
      return 1;
   }

//   if (cfg.get_btn("x")) {
//      cfg.m["l"].vuint[0] = ws.ws_row;
//      cfg.m["w"].vuint[0] = ws.ws_col;
//   }

   e = cfg.save();
   if (e != NULL) {
      cout << pn << ": Error: " << e << endl;
      return 1;
   }

   if (Argv.size() == 0) {
      cmd = "gen";
      Argv = {"str"};
   }
   else {
      cmd = Argv[0];
      Argv.erase(Argv.begin());
   }

   cmds.set_program_name(pn);
   cmds.set_max_line_width(ws.ws_col);
   cmds.set_cmds_help(
      "\n   Rndstream generates customized random text.\n\n"
      "Usage:\n\n   rndstream <command> [arguments]\n");

   cmds.handle(
      "gen",
      cmd_gen,
      "Generate random text.",
      "gen <command> [-options]");
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

   cmds.run(cmd, Argv);

   return 0;
}
 void cmd_gen(vector<string>& argv) {
   commands cmds2;
   cmds2.set_program_name(pn + " gen");
   string cmd2 = "help";

   if (argv.size() > 0) {
      cmd2 = argv[0];
      argv.erase(argv.begin());
   }

   cmds2.set_cmds_help("\nUsage:\n\n   " + pn + " gen <command> [-options]\n");
   cmds2.handle(
         "str",
         cmd_stream,
         "Generate a stream of random text and print to stdout.",
         "str [-options]");

   cmds2.run(cmd2, argv);
   return;
}

void cmd_stream() {

   srand(cfg.get_uint("s"));
   double wt = cfg.get_dbl("t");
   unsigned int ms = abs(wt * 1000);
   unsigned int lmax = cfg.get_uint("l");
   unsigned int wmax = cfg.get_uint("w");
   struct winsize ws;
   ioctl(0, TIOCGWINSZ, &ws);

   if (cfg.get_btn("x")) {
      lmax = ws.ws_row;
      wmax = ws.ws_col;
   }

   while(true) {
      for (unsigned int l = 0; l <= lmax; l++) {
         if (l > 0 && l == lmax) {continue;}

         for (unsigned int w = 0; w < wmax; w++) {
            cout << char(rand() 
                  % (cfg.get_str("o")[1] - cfg.get_str("o")[0])
                  + cfg.get_str("o")[0]);
         }

         if (lmax != 0) {cout << endl;}
      }
      cout.flush();

      this_thread::sleep_for(chrono::milliseconds(ms));
   }

   cout << "wait time: " << wt << endl;
   return;
}

void cmd_set(vector<string>& argv) {
   if (argv.size() == 2) {
      cfg.define_str(argv[0], argv[1]);
      tools::Error e = cfg.save();
      if (e != NULL) {
         cout << e << endl;
      }
      else {
         cout << cfg.getJSON() << endl;
      }
   }
   return;
}

void cmd_env() {
   cout << cfg.file_path << ":\n" << cfg.getJSON() << "\n";
   return;
}
