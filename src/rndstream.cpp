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
//  Verbose level 2:
//     frames are printed as json objects that contain an array of strings
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

void callback_func_prompt(int sig);

void cmd_gen(vector<string>& argv);
void cmd_stream();
void cmd_env();

string  pn;
unsigned int lframe = 1;
jconfig cfg("/etc/rndstream.json");

int main(int argc, char *argv[]) {
   vector<string> Argv(0);
   string         cmd;
   options        opt;
   commands       cmds;
   unsigned int   t = time(NULL);
   Error          e;
   pn = argv[0];

   signal(SIGINT, scbh_return_quiet);
   signals(SIGINT, callback_func_prompt);

   struct winsize ws;
   ioctl(0, TIOCGWINSZ, &ws);

   cfg.define_uint("seed",   t);
   cfg.define_uint("lines",  0);
   cfg.define_uint("width",  1);
   cfg.define_uint("frames", 0);
   cfg.define_uint("print",  0);
   cfg.define_uint("ignore", 0);
   cfg.define_dbl("delay",   0.5);
   cfg.define_str("config",  cfg.file_path);
   cfg.define_str("output",  " ~");
   cfg.define_int("verbose", 0);
   cfg.define_btn("r");
   cfg.define_btn("R");
   cfg.define_btn("x");
   cfg.define_btn("h");
   cfg.define_btn("d");

   opt.handle('c', cfg.m["config"].set,  cfg.m["config"].vstr);
   opt.handle('s', cfg.m["seed"].set,    cfg.m["seed"].vstr);
   opt.handle('l', cfg.m["lines"].set,   cfg.m["lines"].vstr);
   opt.handle('w', cfg.m["width"].set,   cfg.m["width"].vstr);
   opt.handle('f', cfg.m["frames"].set,  cfg.m["frames"].vstr);
   opt.handle('p', cfg.m["print"].set,   cfg.m["print"].vstr);
   opt.handle('o', cfg.m["output"].set,  cfg.m["output"].vstr);
   opt.handle('t', cfg.m["delay"].set,   cfg.m["delay"].vstr);
   opt.handle('i', cfg.m["ignore"].set,  cfg.m["ignore"].vstr);
   opt.handle('v', cfg.m["verbose"].set, cfg.m["verbose"].vstr);
   opt.handle('r', cfg.m["r"].set);
   opt.handle('R', cfg.m["R"].set);
   opt.handle('x', cfg.m["x"].set);
   opt.handle('h', cfg.m["h"].set);
   opt.handle('d', cfg.m["d"].set);

   for (int i = 1; i < argc; i++) {
      Argv.push_back(string(argv[i]));
   }

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

   if (cfg.m["config"].set || cfg.file_path != cfg.get_str("config")) {

      Argv.clear();
      for (int i = 1; i < argc; i++) {
         Argv.push_back(string(argv[i]));
      }

      cfg.file_path = cfg.get_str("config");
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
   }

   e = cfg.convert();
   if (e != NULL) {
      cout << pn << ": Error: " << e << endl;
      return 1;
   }

   if (cfg.get_btn("h")) {
      Argv.push_back("help");
   }

   if (cfg.get_btn("r")) {
      cfg.set("seed", t);
   }

   if (cfg.get_btn("R")) {
      srand(cfg.get_uint("seed"));
      unsigned int li = 0; li--;
      cfg.set("seed", (unsigned int) (rand() % li));
   }

   if (cfg.m["print"].set || cfg.get_uint("print") != 0) {
      if (cfg.get_uint("print") != 0) {
         cfg.set("frames", cfg.get_uint("ignore") + cfg.get_uint("print"));
      }
      else {
         cfg.set("frames", 0);
      }
   }

   if (cfg.get_btn("d") == false) {
      e = cfg.save();
      if (e != NULL) {
         cout << pn << ": Error: " << e << endl;
         return 1;
      }
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
      "   -p <frames> Set the number of frames to print.\n"
      "   -f <frame>  Set the last frame to be printed.\n"
      "   -i <ignore> Set the number of frames to skip.\n"
      "   -t <delay>  Set the time to wait between frames (seconds).\n"
      "   -o <output> Define the output characters. Ex: ' ~' for all,\n"
      "               'az' for lowercase alphabet, '09' for digits.\n"
      "   -c <config> Set a custom config file location. (set every time)\n"
      "   -r          Re-set the seed using current time.\n"
      "   -R          Re-set the seed using current seed.\n"
      "   -d          Do not overwrite the config file.\n"
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
      "Display the configuration file.",
      "env");

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

   srand(cfg.get_uint("seed"));
   double       wt    = cfg.get_dbl("delay");
   int          v     = cfg.get_int("verbose");
   unsigned int ms    = abs(wt * 1000);
   unsigned int lmax  = cfg.get_uint("lines");
   unsigned int wmax  = cfg.get_uint("width");
   unsigned int frame = 1;
   unsigned int fmax  = cfg.get_uint("frames");
   unsigned int i     = cfg.get_uint("ignore");
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
                  % (cfg.get_str("output")[1] - cfg.get_str("output")[0] + 1)
                  + cfg.get_str("output")[0]);
         }
         if (!nolines) {cout << endl;}

         if (l == lmax - 1) {
            cout.flush();
            lframe = frame;

            if (ms != 0 && (fmax == 0 || frame < fmax)) {
               this_thread::sleep_for(chrono::milliseconds(ms));
            }
            frame++;
         }
         if (fmax != 0 && frame > fmax) {break;}
      }
      if (fmax != 0 && frame > fmax) {break;}
   }

   return;
}

void cmd_env() {
   cout << cfg.file_path << ":\n" << cfg.getJSON() << "\n";
   return;
}

void callback_func_prompt(int sig) {
   char o = 'e';

   if (cfg.get_uint("lines") == 0) {
      cout << endl;
   }
   cout << "\n           --- paused ---\n";
   cout << "\n(last frame: " << lframe;
   cout << ") [u: resume | k: exit | s: save]\n-> ";

   while (cin >> o) {
      if (o < 97) {o += 32;}

      if (o == 'e' || o == 'q' || o == 'x' || o == 'k') {
         exit(0);
      }
      if (o == 's' || o == 'o') {
         cfg.set("ignore", lframe);
         cfg.save();
         exit(0);
      }
      if (o == 'u' || o == 'r') {
         break;
      }
   }
   cout << endl;
   return;
}
