//
// rndstream.cpp
//
//    Command line tool that generates customizable random text.
//
// CLI functionality:
//
//    - commands are evaluated from left to right with functionality
//      partially defined by config.
//    - all options are set in config file. an option only needs to be set once
//      to change the functionality of rndstream.
//    - 'button' options must be defined on the command line each time.
//
//  Configuration:
//
//     Use json file /etc/rndstream.json to configure. You may also need to set
//     the permissions for this file.
//
// Example:
//
//     generate seeds
//
//        rndstream gen str -t 1 -w 10 -l 1 -o 09
//
//     generate random 6-letter strings
//
//        rndstream gen str -w 6 -l 1 -o az
//
//     fill screen every half second with any character
//
//        rndstream gen str -t 0.5 -x -o " ~"
//
//     get the environment
//
//        rndstream env
//
//     commands usage allows nested help menus
//
//        rndstream gen help str
//
//  TODO: multiple rand functions
//
//  crand - re-seeds every n calls
//  qrand - makes frequent calls to srand
//  grand - filters output of rand by calling srand when finding 'bad' numbers
//  mrand - keeps record of seeds and calls to rand. has accessible json index.
//  lrand - uses dual calls to rand to produce long uints.
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
void cmd_env();

string  pn;
jconfig cfg("/etc/rndstream.json");

int main(int argc, char *argv[]) {
   vector<string> Argv(0);
   string         cmd;
   options        opt;
   commands       cmds;
   unsigned int   t = time(NULL);
   Error          e;
   pn = argv[0];

   signal(SIGINT, signals_callback_handler);

   struct winsize ws;
   ioctl(0, TIOCGWINSZ, &ws);

   cfg.define_uint("s", t);
   cfg.define_uint("l", 0);
   cfg.define_uint("w", 1);
   cfg.define_uint("f", 0);
   cfg.define_uint("i", 0);
   cfg.define_dbl("t", 0.5);
   cfg.define_str("c", cfg.file_path);
   cfg.define_str("o", " ~");
   cfg.define_int("v", 0);
   cfg.define_btn("r");
   cfg.define_btn("x");

   opt.handle('c', cfg.m["c"].set, cfg.m["c"].vstr);
   opt.handle('s', cfg.m["s"].set, cfg.m["s"].vstr);
   opt.handle('l', cfg.m["l"].set, cfg.m["l"].vstr);
   opt.handle('w', cfg.m["w"].set, cfg.m["w"].vstr);
   opt.handle('f', cfg.m["f"].set, cfg.m["f"].vstr);
   opt.handle('o', cfg.m["o"].set, cfg.m["o"].vstr);
   opt.handle('t', cfg.m["t"].set, cfg.m["t"].vstr);
   opt.handle('v', cfg.m["v"].set, cfg.m["v"].vstr);
   opt.handle('i', cfg.m["i"].set, cfg.m["i"].vstr);
   opt.handle('r', cfg.m["r"].set);
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

   if (cfg.get_btn("r")) {
      cfg.set("s", t);
   }

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
      "Usage:\n\n   rndstream <command> [arguments]\n\n"
      "Options:\n\n"
      "   -s <seed>   Set the seed passed to srand.\n"
      "   -w <width>  Set the number of characters printed per line.\n"
      "   -l <lines>  Set the number of lines printed per frame.\n"
      "   -f <frames> Set the number of frames printed.\n"
      "   -t <delay>  Set the time to wait between frames (seconds).\n"
      "   -o <output> Define the output characters. Ex: ' ~' for all,\n"
      "               'az' for lowercase alphabet, '09' for digits.\n"
      "   -c <config> Set a custom config file location. (set every time)\n"
      "   -r          Re-set the seed using current time.\n"
      "   -x          Set width and lines to terminal size.\n"
   );

   cmds.handle(
      "gen",
      cmd_gen,
      "Generate random text. Use 'gen help' for more info.",
      "gen <command> [-options]");
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
   double       wt    =  cfg.get_dbl("t");
   int          v     = cfg.get_int("v");
   unsigned int ms    = abs(wt * 1000);
   unsigned int lmax  = cfg.get_uint("l");
   unsigned int wmax  = cfg.get_uint("w");
   unsigned int frame = 1;
   unsigned int fmax  = cfg.get_uint("f");
   unsigned int i     = cfg.get_uint("i");
   struct winsize ws;
   bool skipframe = false;
   bool nolines = false;

   if (lmax == 0) {
      lmax = 1;
      nolines = true;
   }

   while (true) {
      if (i != 0 && i >= frame) {skipframe = true;}
      else {skipframe = false;}

      ioctl(0, TIOCGWINSZ, &ws);
      if (cfg.get_btn("x")) {
         lmax = ws.ws_row;
         wmax = ws.ws_col;
      }
      if (wmax == 0) {wmax = 1;}

      for (unsigned int l = 0; l < lmax; l++) {
         if (!skipframe && v >= 1 && l == 0 && !nolines) {
            cout << "frame: " << frame << endl;
         }

         if (skipframe) {
            for (unsigned int w = 0; w < wmax; w++) {rand();}
            if (l == lmax - 1) {frame++;}
            continue;
         }

         for (unsigned int w = 0; w < wmax; w++) {
            cout << char(rand() 
                  % (cfg.get_str("o")[1] - cfg.get_str("o")[0] + 1)
                  + cfg.get_str("o")[0]);

            if ((w == wmax - 1)
                && (l == lmax - 1)) {
               if (nolines == false) {cout << endl;}
               cout.flush();
               this_thread::sleep_for(chrono::milliseconds(ms));
               frame++;
            }
         }

         if (fmax != 0 && frame > fmax) {break;}
      }
      if (fmax != 0 && frame > fmax) {break;}
   }

   return;
}

void cmd_env() {
   cout << cfg.file_path << ":\n" << cfg.getJSON();
   return;
}
