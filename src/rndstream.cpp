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
//  build rand substitute that generates true random values.
//  reads output of call to rand and makes call to srand based on that.
//  can be configured to use time or seed-gen app
//
//  multiple rand functions
//
//  grand - filters output of rand by calling srand when finding 'bad' numbers
//  crand - re-seeds every n calls
//  qrand - makes frequent calls to srand
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
//  brand
//
//     Substitute for rand that re-seeds itself using time after n calls.
//
// Created by Daniel Kozitza
//

#include <csignal>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sys/ioctl.h>
#include <omp.h>
#include "tools.hpp"
#include "jconfig.hpp"
#include "options.hpp"
#include "commands.hpp"
#include "sorters.hpp"

using namespace tools;

typedef map<string, string>::iterator map_iter;

void cmd_set(vector<string>& argv);
void cmd_get(vector<string>& argv);
void cnt();
void env();

jconfig cfg("~/.rndstream.json");

int main(int argc, char *argv[]) {
   vector<string> Argv(0);
   string         cmd_str;
   options        opt;

   signal(SIGINT, signals_callback_handler);

   struct winsize ws;
   ioctl(0, TIOCGWINSZ, &ws);

   cfg.set("opt_w", 40);

   //opt.handle('w', cfg.m["opt_w"].set, cfg.m["opt_w"].sval);

   commands cmds;
   cmds.set_program_name(string(argv[0]));
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
      "get",
      cmd_get,
      "Get environment variables.",
      "get [key]");
   cmds.handle(
      "env",
      env,
      "Displays the variables read from vfnmkrndstream.conf.",
      "env");
   cmds.handle(
      "cnt",
      cnt,
      "Counts the number of lines in each of the source files.",
      "cnt");

   if (argc < 2)
      cmd_str = "help";
   else
      cmd_str = argv[1];

   for (int i = 2; i < argc; i++)
      Argv.push_back(string(argv[i]));

   Error e;
   e = opt.evaluate(Argv);
   if (e != NULL) {
      cout << string(argv[0]) << ": ERROR: " << e << endl;
      return 1;
   }

   cmds.run(cmd_str, Argv);

   return 0;
}

void cmd_set(vector<string>& argv) {

   return;
}

void cmd_get(vector<string>& argv) {
   
   return;
}

//void runtime(vector<string>& argv) {
//   double start, end;
//   vector<string> empty_v;
//   map<string, string> vfnconf;
//   require(get_vfnmkrndstream_conf(vfnconf));
//
//   string system_call = "./";
//   system_call += vfnconf["name"];
//   for (int i = 0; i < argv.size(); i++)
//      system_call += " \"" + argv[i] + "\"";
//   cout << "rndstream::runtime: calling `" << system_call << "`.\n";
//
//   start = omp_get_wtime();
//   system(system_call.c_str());
//   end = omp_get_wtime();
//   
//   cout << "rndstream::runtime: execution time (seconds): `";
//   cout << end - start << "`.\n";
//}

void env() {
   map<string, string> vfnconf;
   cout << "rndstream::env: reading vfnmkrndstream.conf." << endl;
   require(get_vfnmkmc_conf(vfnconf));

   cout << "\nvfnmkmc.conf:\n\n";
   //for (const auto item : vfnconf) {
   //   cout << item.first << ": " << item.second << "\n";
   //
   //}

   for (map_iter it = vfnconf.begin(); it != vfnconf.end(); ++it)
      cout << it->first << ": " << it->second << "\n";

   cout << "\n";
}

void cnt() {
   map<string, string> vfnconf;
   vector<string> contents(0);
   unsigned int total_lines = 0;
   string src_dir;

   if (get_vfnmkmc_conf(vfnconf))
      src_dir = vfnconf["src_directory"];
   else
      src_dir = ".";

   if (!list_dir_r(src_dir, contents)) {
      cerr << "rndstream::cnt: vfnmkrndstream src_directory `" + src_dir;
      cerr << "` does not exist\n";
      return;
   }

   vector<string> new_contents(0);
   for (int i = 0; i < contents.size(); ++i) {
      if (pmatches(contents[i], "(\\.cpp|\\.c|\\.hpp|\\.h)$")) {
         new_contents.push_back(contents[i]);
      }
   }
   contents = new_contents;
   new_contents.clear();

   int longest = 0;
   for (int i = 0; i < contents.size(); ++i) {
      string fname = src_dir + "/" + contents[i];
      if (fname.size() > longest)
         longest = fname.size() + 1;
   }

   sorters::radix(contents);

   ifstream fh;
   for (int i = 0; i < contents.size(); ++i) {
      string fname = src_dir + "/" + contents[i];

      fh.open(fname.c_str(), ifstream::in);
      if (!fh.is_open()) {
         cout << "rndstream::cnt: could not open file: `" << fname << "`\n";
         continue;
      }

      char c;
      int file_lines = 0;
      while (fh.peek() != EOF) {
         fh.get(c);
         if (c == '\n')
            ++file_lines;
      }
      fh.close();
      total_lines += file_lines;

      fname += ":";
      cout << left << setw(longest) << fname;
      cout << " " << file_lines << endl;
   }

   cout << endl << left << setw(longest) << "total_loc:";
   cout << " " << total_lines << "\n\n";
}
