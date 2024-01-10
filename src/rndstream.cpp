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
//
// Created by Daniel Kozitza
//

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <fstream>
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

void cmd_gen(vector<string>& argv, ostream& out, jconfig& cfg);
void cmd_stream(vector<string>& argv, ostream& out, jconfig& cfg);
void cmd_file(vector<string>& argv, ostream& file, jconfig& cfg);
void cmd_env(vector<string>& argv, ostream& out, jconfig& cfg);
void cmd_purge(vector<string>& argv, ostream& out, jconfig& cfg);

void define_config(jconfig& cfg, unsigned int t) {
   cfg.define_uint("seed",   t);
   cfg.define_uint("lines",  0);
   cfg.define_uint("width",  1);
   cfg.define_uint("frames", 0);
   cfg.define_uint("print",  0);
   cfg.define_uint("ignore", 0);
   cfg.define_dbl("delay",   0.5);
   cfg.define_str("config",  cfg.file_path);
   cfg.define_str("output",  " ~");
   cfg.define_str("output_list", "none");
   cfg.define_str("output_str", "none");
   cfg.define_vstr("output_vstr", {"none"});
   cfg.define_str("recursion_str", "");
   cfg.define_vstr("recursion_vstr", {});
   cfg.define_int("verbose", 0);
   cfg.define_bool("handle_ctrl_c", 1);
   cfg.define_bool("delete_temporary_files", 1);
   cfg.define_btn("r");
   cfg.define_btn("R");
   cfg.define_btn("x");
   cfg.define_btn("h");
   cfg.define_btn("d");
   cfg.define_btn("n");
   cfg.define_btn("b");
   cfg.define_str("pn", "rndstream");
   cfg.define_str("instance", "NULL");
   return;
}

unsigned int lframe = 1;
char instance[] = "..........\0";

int main(int argc, char *argv[]) {
   vector<string> Argv(0);
   string         cmd;
   options        opt;
   commands       cmds;
   unsigned int   t = time(NULL);
   bool           display_help = false;
   Error          e;

   signal(SIGINT, scbh_return_quiet);
   signals(SIGINT, callback_func_prompt);

   struct winsize ws;
   ioctl(0, TIOCGWINSZ, &ws);

   jconfig cfg;
   if (getenv("HOME") != NULL && string(getenv("HOME")) != "" && dir_exists(getenv("HOME"))) {
      cfg.set_file_location(string(getenv("HOME")) + "/.rndstream.json");
   }
   define_config(cfg, t);

   opt.handle('c', cfg.m["config"].set,        cfg.m["config"].vstr);
   opt.handle('s', cfg.m["seed"].set,          cfg.m["seed"].vstr);
   opt.handle('l', cfg.m["lines"].set,         cfg.m["lines"].vstr);
   opt.handle('w', cfg.m["width"].set,         cfg.m["width"].vstr);
   opt.handle('f', cfg.m["frames"].set,        cfg.m["frames"].vstr);
   opt.handle('p', cfg.m["print"].set,         cfg.m["print"].vstr);
   opt.handle('o', cfg.m["output"].set,        cfg.m["output"].vstr);
   opt.handle('O', cfg.m["output_list"].set,   cfg.m["output_list"].vstr);
   opt.handle('S', cfg.m["output_str"].set,    cfg.m["output_str"].vstr);
   opt.handle('N', cfg.m["recursion_str"].set, cfg.m["recursion_str"].vstr);
   opt.handle('t', cfg.m["delay"].set,         cfg.m["delay"].vstr);
   opt.handle('i', cfg.m["ignore"].set,        cfg.m["ignore"].vstr);
   opt.handle('v', cfg.m["verbose"].set,       cfg.m["verbose"].vstr);
   opt.handle('C', cfg.m["handle_ctrl_c"].set, cfg.m["handle_ctrl_c"].vstr);
   opt.handle('D', cfg.m["delete_temporary_files"].set,
                   cfg.m["delete_temporary_files"].vstr);
   opt.handle('i', cfg.m["instance"].set,      cfg.m["instance"].vstr);
   opt.handle('r', cfg.m["r"].set);
   opt.handle('R', cfg.m["R"].set);
   opt.handle('x', cfg.m["x"].set);
   opt.handle('h', cfg.m["h"].set);
   opt.handle('d', cfg.m["d"].set);
   opt.handle('n', cfg.m["n"].set);
   opt.handle('b', cfg.m["b"].set);

   for (int i = 1; i < argc; i++) {
      Argv.push_back(string(argv[i]));
   }

   e = cfg.load();
   if (e != NULL) {
      cout << cfg.get_str("pn") << ": " << e << "\n";
      cout << cfg.get_str("pn") << ": Making configuration file '" << cfg.file_path;
      cout << "'.\n";
      e = cfg.save();
      if (e != NULL) {
         cout << cfg.get_str("pn") << ": " << e << endl;
         return 1;
      }
   }

   e = opt.evaluate(Argv);
   if (e != NULL) {
      cout << cfg.get_str("pn") << ": " << e << endl;
      return 1;
   }

   if (cfg.m["config"].set || cfg.file_path != cfg.get_str("config")) {

      Argv.clear();
      for (int i = 1; i < argc; i++) {
         Argv.push_back(string(argv[i]));
      }

      cfg.file_path = cfg.get_str("config");
      e = cfg.load();
      if (e != NULL && !cfg.get_btn("d")) {
         if (cfg.get_int("verbose") > 0) {
            cout << cfg.get_str("pn") << ": " << e << ": Making jconfig file '" << cfg.file_path;
            cout << "'.\n";
         }
         e = cfg.save();
         if (e != NULL) {
            if (cfg.get_int("verbose") > 0) {
               cout << cfg.get_str("pn") << ": " << e << endl;
            }
            return 1;
         }
      }

      opt.clear();
      e = opt.evaluate(Argv);
      if (e != NULL) {
         cout << cfg.get_str("pn") << ": " << e << endl;
         return 1;
      }
   }

   if (cfg.m["output"].set)      {
      cfg.define_str("output_list", "none");
      cfg.define_str("output_str", "none");
   }
   if (cfg.m["output_list"].set) {
      cfg.define_str("output", "none");
      cfg.define_str("output_str", "none");
   }
   if (cfg.m["output_str"].set) {
      cfg.define_str("output", "none");
      cfg.define_str("output_list", "none");

      cfg.m["output_vstr"].vstr.clear();
      chop(',', cfg.m["output_str"].vstr[0], cfg.m["output_vstr"].vstr);
   }

   if (cfg.m["recursion_str"].set) {
      cfg.m["recursion_vstr"].vstr.clear();
      chop(',', cfg.m["recursion_str"].vstr[0], cfg.m["recursion_vstr"].vstr);
   }

   e = cfg.convert();
   if (e != NULL) {
      cout << cfg.get_str("pn") << ": Error: " << e << endl;
      return 1;
   }

   if (cfg.get_btn("h")) {
      Argv.push_back("help");
   }
   for (size_t i = 0; i < Argv.size(); i++) {
      if (Argv[i] == "help") { display_help = true; }
   }

   if (cfg.get_btn("r")) {
      cfg.set("seed", t);
   }

   if ((cfg.get_int("verbose") >= 2) ||
       (cfg.get_int("verbose") >= 1 && cfg.get_btn("r"))) {

      if (!display_help) {
         cout << "Seed: " << cfg.get_uint("seed") << endl;
      }
   }

   if (cfg.get_btn("R")) {

      for (size_t i = 0; i < opt.get_opt_times_set('R'); i++) {
         srand(cfg.get_uint("seed"));
         unsigned int li = 0; li--;
         cfg.set("seed", (unsigned int) (rand() % li));

         if (cfg.get_int("verbose") >= 1) {
            cout << "Generated seed: " << cfg.get_uint("seed") << "\n";
         }
      }
   }

   if (!cfg.m["instance"].set) {
      srand(t - 910);
      size_t namesize = rand() % 5 + 6;
      cfg.m["instance"].vstr[0].resize(namesize);
      for (size_t i = 0; i < namesize; i++) {
         cfg.m["instance"].vstr[0][i] = rand() % ('z' - 'a' + 1) + 'a';
         instance[i] = cfg.m["instance"].vstr[0][i];

         if (i + 1 == namesize) {
            instance[i+1] = '\0';
         }
      }

      if (cfg.get_int("verbose") > 1) {
         cout << "Instance: " << instance << endl;
      }
   }

   if (cfg.m["print"].set || cfg.get_uint("print") != 0) {
      if (cfg.get_uint("print") != 0) {
         cfg.set("frames", cfg.get_uint("ignore") + cfg.get_uint("print"));
      }
      else {
         cfg.set("frames", 0);
      }
   }

   for (size_t i = 0; i < opt.get_opt_times_set('n'); i++) {
      cfg.set("ignore", cfg.get_uint("ignore") + 1);
      cfg.set("frames", cfg.get_uint("frames") + 1);
   }

   for (size_t i = 0; i < opt.get_opt_times_set('b') && cfg.get_uint("ignore") > 0; i++) {
      cfg.set("ignore", cfg.get_uint("ignore") - 1);
      cfg.set("frames", cfg.get_uint("frames") - 1);
   }

   if (cfg.get_btn("d") == false) {
      e = cfg.save();
      if (e != NULL) {
         cout << cfg.get_str("pn") << ": Error: " << e << endl;
         return 1;
      }
   }

   if (cfg.get_btn("x")) {cfg.define_bool("tmp_x_set", true);}

   if (Argv.size() == 0) {
      cmd = "gen";
      Argv = {"str"};
   }
   else {
      cmd = Argv[0];
      Argv.erase(Argv.begin());
   }

   cmds.set_program_name(cfg.get_str("pn"));
   cmds.set_max_line_width(ws.ws_col);
   cmds.set_cmd_name_width(5);
   cmds.set_cmds_help(
      "\n   Rndstream generates customized random text.\n\n"
      "Usage:\n\n   rndstream [command] [-options]\n\n"
      "Options:\n\n"
      "   -s <seed>   " + fold(15, ws.ws_col, "Set the seed passed to srand.") + "\n"
      "   -w <width>  " + fold(15, ws.ws_col, "Set the number of characters printed per line.") + "\n"
      "   -l <lines>  " + fold(15, ws.ws_col, "Set the number of lines printed per frame.") + "\n"
      "   -p <frames> " + fold(15, ws.ws_col, "Set the number of frames to print.") + "\n"
      "   -f <frame>  " + fold(15, ws.ws_col, "Set the last frame to be printed.") + "\n"
      "   -i <ignore> " + fold(15, ws.ws_col, "Set the number of frames to skip.") + "\n"
      "   -t <delay>  " + fold(15, ws.ws_col, "Set the time to wait between frames (seconds)") + ".\n"
      "   -o <output> " + fold(15, ws.ws_col, "Define the output as a range of characters.") + "\n"
      "               " + fold(15, ws.ws_col, "Ex: ' ~' for all, 'az' for alphabet, '09' for digits.") + "\n"
      "   -O <list>   " + fold(15, ws.ws_col, "Define the output as a list of characters.") + "\n"
      "               " + fold(15, ws.ws_col, "Ex: '123abc', '1123', '\\/|-    '.") + "\n"
      "   -S <string> " + fold(15, ws.ws_col, "Define the output as a comma separated list of strings.") + "\n"
      "               " + fold(15, ws.ws_col, "Ex: '70 ,80 ,90 ', 'G ,G ,C ,Em,Am,D '.") + "\n"
      "   -N <string> " + fold(15, ws.ws_col, "Define recursion triggers and their corresponding") + "\n"
      "               " + fold(15, ws.ws_col, "configuration files as a comma separated list.") + "\n"
      "               " + fold(15, ws.ws_col, "Ex: 'n,numbers.json,7,letters.json'") + "\n"
      "   -x          " + fold(15, ws.ws_col, "Set width and lines to terminal size.") + "\n"
      "   -c <config> " + fold(15, ws.ws_col, "Set a custom config file location. (set every time)") + "\n"
      "   -r          " + fold(15, ws.ws_col, "Re-set the seed using current time.") + "\n"
      "   -R          " + fold(15, ws.ws_col, "Re-set the seed using current seed.") + "\n"
      "   -d          " + fold(15, ws.ws_col, "Do not overwrite the config file.") + "\n"
      "   -C          " + fold(15, ws.ws_col, "Exit rather than pause on Ctrl+c signal. (toggle) (default pause)") + "\n"
      "   -D          " + fold(15, ws.ws_col, "Delete temporary files. (toggle) (default true)") + "\n"
      "   -n          " + fold(15, ws.ws_col, "Go to the next frame.") + "\n"
      "   -b          " + fold(15, ws.ws_col, "Go to the last frame.") + "\n");

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
   cmds.handle(
      "purge",
      cmd_purge,
      "Delete temporary files.",
      "purge");

   cmds.run(cmd, Argv, cout, cfg);

   return 0;
}

void cmd_gen(vector<string>& argv, ostream& out, jconfig& cfg) {
   struct winsize ws;
   ioctl(0, TIOCGWINSZ, &ws);

   commands cmds2;
   cmds2.set_program_name(cfg.get_str("pn") + " gen");
   cmds2.set_max_line_width(ws.ws_col);
   cmds2.set_cmd_name_width(5);
   string cmd2 = "help";

   if (argv.size() > 0) {
      cmd2 = argv[0];
      argv.erase(argv.begin());
   }

   if (argv[0] == "help") {
      cmd2 = "help";
      argv.clear();
   }
   else if (cmd2 != "help" && cfg.get_int("verbose") == 1 && cfg.get_uint("ignore") > 0) {
      cout << "Ignoring " << cfg.get_uint("ignore") << " frames.\n";
   }

   cmds2.set_cmds_help("\nUsage:\n\n   " + cfg.get_str("pn") + " gen <command> [-options]\n");
   cmds2.handle(
         "str",
         cmd_stream,
         "Generate a stream of random text and print to stdout.",
         "str [-options]");

   cmds2.handle(
         "file",
         cmd_file,
         "Generate a stream of random text and print to file.",
         "file <file_name> [-options]");

   if (cmd2 == "str") {
      cmds2.run(cmd2, argv, cout, cfg);
   }
   else if (cmd2 == "file") {
      // attempt to open file in argv 3
      if (argv.size() < 1) {
         cout << cfg.get_str("pn") << ": Command requires a file name.\n";
         return;
      }
      if (argv[0] == "help" || argv[0] == "-h") {
         argv.resize(1);
         argv[0] = "file";
         cmds2.run("help", argv, cout, cfg);
         return;
      }

      cfg.define_dbl("delay", 0.0);

      if (cfg.get_uint("frames") == 0) {
         cout << cfg.get_str("pn") << ": Frame (-f #) is not set. Program will run indefinately.\n";
         return;
      }

      ofstream fout;
      fout.open(argv[0].c_str(), ofstream::out);
      if (!fout.is_open()) {
         cout << cfg.get_str("pn") << ": Output file cannot be opened.";
         return;
      }
      cmds2.run(cmd2, argv, fout, cfg);
   }
   else {
      cmds2.run("help", argv, cout, cfg);
   }
   return;
}

void cmd_file(vector<string>& argv, ostream& file, jconfig& cfg) {
   cmd_stream(argv, file, cfg);
   return;
}

void cmd_stream(vector<string>& argv, ostream& out, jconfig& cfg) {

   Error e = NULL;
   e = cfg.save_tmp(instance);
   if (e != NULL) {
      cout << cfg.get_str("pn") << ": Error: " << e << endl;
      exit(1);
   }

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
   bool output_is_range = true;
   bool output_is_list  = true;
   if (cfg.get_str("output")     == "none") { output_is_range = false; }
   if (cfg.get_str("output_list") == "none") { output_is_list  = false; }

   if (lmax == 0) {
      lmax = 1;
      nolines = true;
   }

   while (true) {
      if (i != 0 && i >= frame) {skipframe = true;}
      else {skipframe = false;}

      ioctl(0, TIOCGWINSZ, &ws);
      if (cfg.get_btn("x")) {
         cfg.define_bool("tmp_x_set", true);
         lmax = ws.ws_row;
         wmax = ws.ws_col;
      }
      if (wmax == 0) {wmax = 1;}

      string outframe = "";

      for (unsigned int l = 0; l < lmax; l++) {
         if (!skipframe && v >= 3 && l == 0 && !nolines) {
            out << "Frame: " << frame << endl;
         }

         if (skipframe) {
            for (unsigned int w = 0; w < wmax; w++) {rand();}
            if (l == lmax - 1) {frame++;}
            continue;
         }

         std::string outstr = "";
         for (unsigned int w = 0; w < wmax; w++) {
            if (output_is_range && cfg.get_str("output").size() == 2) {
               outstr += char(rand()
                     % (cfg.get_str("output")[1]-cfg.get_str("output")[0]+1)
                     + cfg.get_str("output")[0]);
            }
            else if (output_is_list && cfg.get_str("output_list").size() > 0) {
               outstr += cfg.get_str("output_list")[
                        rand() % cfg.get_str("output_list").size()];
            }
            else if (cfg.get_vstr("output_vstr").size() > 0) {
               outstr += cfg.get_vstr("output_vstr")[
                        rand() % cfg.get_vstr("output_vstr").size()];
            }
            else {
               out << cfg.get_str("pn") << ": Error: Incorrect output format.\n";
               return;
            }
         }

         if (outstr.size() != 0) {
            out << outstr.c_str();
            outframe += outstr;
         }

         if (!nolines) {
            out << endl;
         }

         if (l == lmax - 1) {
            out.flush();
            lframe = frame;

            if (ms != 0 && (fmax == 0 || frame < fmax)) {
               this_thread::sleep_for(chrono::milliseconds(ms));
            }
            frame++;
         }
         if (fmax != 0 && frame > fmax) {break;}
      }

      // check for matching string in recursion_vstr
      if (!skipframe && cfg.m["recursion_vstr"].vstr.size() != 0) {
         for (size_t i = 0; i < cfg.m["recursion_vstr"].vstr.size(); i += 2) {

            if (cfg.m["recursion_vstr"].vstr[i] == outframe) {

               if (v >= 2 && !nolines) {
                  out << "Loading: " << cfg.m["recursion_vstr"].vstr[i + 1];
                  out << endl;
               }

               cfg.set("ignore", lframe);
               cfg.save();

               jconfig cfgn("");
               cfgn.set_file_location(cfg.m["recursion_vstr"].vstr[i + 1]);
               define_config(cfgn, 0);
               e = cfgn.load();
               if (e != NULL) {
                  cout << cfgn.get_str("pn") << ": Error: jconfig file could not be loaded: ";
                  cout << e << endl;
                  exit(1);
               }
               e = cfgn.convert();
               if (e != NULL) {
                  cout << cfgn.get_str("pn") << ": Error: jconfig failed to convert: ";
                  cout << e << endl;
                  exit(1);
               }

               return cmd_stream(argv, out, cfgn);
            }
         }
      }

      if (fmax != 0 && frame > fmax) {break;}
   }

   if (cfg.get_bool("delete_temporary_files")) {
      system(string("rm " + cfg.tmp_file_path + " &").c_str());
   }

   return;
}

void cmd_env(vector<string>& argv, ostream& out, jconfig& cfg) {
   cout << cfg.file_path << ":\n" << cfg.getJSON() << "\n";
   return;
}

void cmd_purge(vector<string>& cmdv, ostream& out, jconfig& cfg) {
   string cmd = "rm " + string(JCONFIG_TMP_PATH) + "*" + string(JCONFIG_TMP_FILENAME);
   cout << cfg.get_str("pn") << ": system call: " << cmd << endl;
   system(cmd.c_str());
   return;
}

void callback_func_prompt(int sig) {
   char o = 'e';
   unsigned int width = 0;
   unsigned int lines = 0;
   struct winsize ws;
   ioctl(0, TIOCGWINSZ, &ws);

   jconfig cfg("");
   define_config(cfg, 0);
   Error e = cfg.load_tmp(instance);
   if (e != NULL) {
      cout << cfg.get_str("pn") << ": Error: jconfig tmp file could not be loaded: ";
      cout << e << endl;
      exit(1);
   }
   e = cfg.convert();
   if (e != NULL) {
      cout << cfg.get_str("pn") << ": Error: jconfig failed to convert: ";
      cout << e << endl;
      exit(1);
   }

   if (cfg.get_bool("handle_ctrl_c") == false) {
      exit(0);
   }

   if (cfg.get_bool("tmp_x_set") == true) {
      width = ws.ws_col;
      lines = ws.ws_row;
   }
   else {
      width = cfg.get_uint("width");
      lines = cfg.get_uint("lines");
   }

   // assume config has not been modified since save_tmp was called
   if (cfg.get_str("config") != cfg.file_path) {
      cfg.set_file_location(cfg.get_str("config"));
   }

   if (cfg.get_uint("lines") == 0) {
      cout << endl;
   }
   cout << "\n                     --- paused ---\n";

   if (cfg.get_int("verbose") > 0) {
      cout << "\n- (instance: " << cfg.get_str("instance").c_str();
      cout << ") (last frame: " << lframe;
      cout << ") (frame size: " << width;
      cout << "x"               << lines;
      cout << ") (delay: " << cfg.get_dbl("delay") << ")";
   }

   cout << "\n- Type key and press enter: [r: resume | e: exit | s: save]\n-> ";

   while (cin >> o) {
      if (o < 97) {o += 32;}

      if (o == 'e' || o == 'q' || o == 'x' || o == 'k') {
         if (cfg.get_bool("delete_temporary_files")) {
            system(string("rm " + cfg.tmp_file_path).c_str());
         }
         exit(0);
      }
      if (o == 's' || o == 'o') {
         cfg.set("ignore", lframe);
         cfg.save();
         if (cfg.get_bool("delete_temporary_files")) {
            system(string("rm " + cfg.tmp_file_path).c_str());
         }
         exit(0);
      }
      if (o == 'u' || o == 'r') {
         break;
      }
   }
   cout << endl;
   return;
}
