//
// commands.cpp
//
// Created by Daniel Kozitza
//

#include <cassert>
#include <iostream>
#include <iomanip>
#include "commands.hpp"
#include "tools.hpp"

void dummy_func() {};
void dummy_func(vector<string>& j) {};
void dummy_func(ostream& out) {};
void dummy_func(ostream& out, vector<string>& j) {};
void dummy_func(ostream& out, jconfig& cfg, vector<string>& j) {};

commands::commands() {
   cmd_name_width = 10;
   max_line_width = 80;
   is_resolved    = false;
}

// set_program_name
// 
// When this is set the commands object will act as if the command being
// run is from the command line arguments of a program named 'pn'.
// 
// If set two things will happen:
//
//    1. the program name will be used in the main help output.
//
//    2. The program name will be prepended to usage strings.
//
void commands::set_program_name(string pn) {
   program_name = pn;
}

// set_cmds_help
// 
// This message is printed when run("help", Argv) is called. Before the list
// of available commands.
//
void commands::set_cmds_help(string msg) {
   cmds_help = msg;
}

// set_cmd_name_width
//
// Sets the minimum width of the cmd name column in the help message.
//
void commands::set_cmd_name_width(int w) {
   cmd_name_width = w;
}

// set_max_line_width
//
// Sets the maximum number of characters allowed in help output lines.
//
void commands::set_max_line_width(int w) {
   max_line_width = w;
}

// default_help
//
// This function is automatically called when the "help" command is run. If
// argv is empty it will print the help message for the entire commands object,
// 'cmds_help'. This message is expected to explain what the situation is that
// requires user input and should include it's own usage. After the help message
// is printed a list of the available commands is printed.
//
// Ex:
//
//    commands cmds;
//    cmds.set_cmds_help(
//       "program_name is designed to ...\n"
//       "Usage: program_name command [arguments]\n");
//    cmds.handle("cmd_name", function_name, "synopsis", "usage");
//    cmds.run("help", Argv);
//
// If the argv vector has arguments in it the usage, synopsis, and description
// is printed for each command named in the arguments.
//
void commands::default_help(vector<string>& argv) {
   is_resolved = false;
   if (argv.size() == 0) {

      if (cmds_help != "")
         cout << cmds_help << endl;

      // print the list of available commands
      if (cmds.size() > 0) {
         cout << "Commands:\n\n";
         //for (const auto c : cmds) {
         for (cmds_iter it = cmds.begin(); it != cmds.end(); it++) {
            cout << "   " << left << setw(cmd_name_width) << it->first;
            cout << " - " << tools::fold(
               cmd_name_width + 6,
               max_line_width,
               it->second.synopsis) << endl;
         }

         string msg = "Use \"";
         if (program_name != "")
            msg += program_name + " ";
         msg += "help [command]\" for more information about a command.";
         cout << endl << tools::fold(0, max_line_width, msg);
         cout << "\n\n";
      }
   }
   else {
      // print the usage, synopsis, and description of each argument
      for (int i = 0; i < argv.size(); i++) {
         if (cmds.find(argv[i]) != cmds.end()) {
            cout << "\nUsage: ";
            if (program_name != "")
               cout << program_name << " ";
            cout << cmds[argv[i]].usage << "\n\n";
            cout << tools::fold(
               0, max_line_width, cmds[argv[i]].synopsis) << endl;
            if (cmds[argv[i]].description != "")
               cout << endl << tools::fold(
                  0, max_line_width, cmds[argv[i]].description) << endl;

            cout << endl;
         }
         else {
            // if the user calls help help
            if (argv[i] == "help") {
               vector<string> tmp;
               default_help(tmp);
            }
            else {
               cout << "commands::default_help: Unknown command '";
               cout << argv[i] << "'. Try '";
               if (program_name != "")
                  cout << program_name << " "; 
               cout << "help'.\n";
            }
         }
      }
   }
}

void commands::handle(
      string cmd,
      void (*func)(vector<string>&),
      string synopsis,
      string usage,
      string description) {

   Command tmp = {dummy_func, func, dummy_func, dummy_func, dummy_func,
                  true, false, false, synopsis, usage, description};
   cmds[cmd]   = tmp;
}

void commands::handle(
      string cmd,
      void (*func)(),
      string synopsis,
      string usage,
      string description) {

   Command tmp = {func, dummy_func, dummy_func, dummy_func, dummy_func,
                  false, false, false, synopsis, usage, description};
   cmds[cmd]   = tmp;
}

void commands::handle(
      string cmd,
      void (*func)(ostream&),
      string synopsis,
      string usage,
      string description) {

   Command tmp = {dummy_func, dummy_func, func, dummy_func, dummy_func,
                  false, true, false, synopsis, usage, description};
   cmds[cmd]   = tmp;
}

void commands::handle(
      string cmd,
      void (*func)(ostream&, vector<string>&),
      string synopsis,
      string usage,
      string description) {

   Command tmp = {dummy_func, dummy_func, dummy_func, func, dummy_func,
                  true, true, false, synopsis, usage, description};
   cmds[cmd]   = tmp;
}

void commands::handle(
      string cmd,
      void (*func)(ostream&, jconfig& cfg, vector<string>&),
      string synopsis,
      string usage,
      string description) {

   Command tmp = {dummy_func, dummy_func, dummy_func, dummy_func, func,
                  true, true, true, synopsis, usage, description};
   cmds[cmd]   = tmp;
}

void commands::run(string cmd, vector<string>& arguments) {
   map<string, Command>::iterator it = cmds.find(cmd);

   // if help has been defined by the user do not run default_help
   if (cmd == "help" && it == cmds.end())
      default_help(arguments);

   else if (it != cmds.end()) {
      is_resolved = true;
      if (cmds[cmd].has_arguments) {
         cmds[cmd].func_wa(arguments);
      }
      else {
         cmds[cmd].func_na();
      }
   }
   else {
      cout << "commands::run: Unknown command '" << cmd << "'. Try '";
      if (program_name != "")
         cout << program_name << " ";
      cout << "help'.\n";
   }
}

void commands::run(string cmd, vector<string>& arguments, ostream& out) {
   map<string, Command>::iterator it = cmds.find(cmd);

   // if help has been defined by the user do not run default_help
   if (cmd == "help" && it == cmds.end()) {
      default_help(arguments);
   }

   else if (it != cmds.end()) {
      is_resolved = true;
      if (cmds[cmd].has_arguments) {
         if (cmds[cmd].has_output) {
            cmds[cmd].func_woa(out, arguments);
         }
         else {
            cmds[cmd].func_wa(arguments);
         }
      }
      else {
         if (cmds[cmd].has_output) {
            cmds[cmd].func_wo(out);
         }
         else {
            cmds[cmd].func_na();
         }
      }
   }
   else {
      cout << "commands::run: Unknown command '" << cmd << "'. Try '";
      if (program_name != "")
         cout << program_name << " ";
      cout << "help'.\n";
   }
}

void commands::run(string cmd, vector<string>& arguments, ostream& out, jconfig& cfg) {
   map<string, Command>::iterator it = cmds.find(cmd);

   // if help has been defined by the user do not run default_help
   if (cmd == "help" && it == cmds.end()) {
      default_help(arguments);
   }

   else if (it != cmds.end()) {
      is_resolved = true;
      if (cmds[cmd].has_all_args) {
         cmds[cmd].func_all(out, cfg, arguments);
      }
      else if (cmds[cmd].has_arguments) {
         if (cmds[cmd].has_output) {
            cmds[cmd].func_woa(out, arguments);
         }
         else {
            cmds[cmd].func_wa(arguments);
         }
      }
      else {
         if (cmds[cmd].has_output) {
            cmds[cmd].func_wo(out);
         }
         else {
            cmds[cmd].func_na();
         }
      }
   }
   else {
      cout << "commands::run: Unknown command '" << cmd << "'. Try '";
      if (program_name != "")
         cout << program_name << " ";
      cout << "help'.\n";
   }
}

// resolved
//
// For use in interactive loops. resolved will return true if any command
// other than help is run. false if the command is help or unknown.
//
// Ex:
//
//    void quit() {
//       cout << "exiting\n";
//    }
//
//    ...
//
//       vector<string> Argv;
//       string         cmd_str = "help";
//       commands       cmds;
//       cmds.set_cmds_help = "\ncmds_help\n";
//       cmds.handle("quit", quit, "quit1", "quit2", "quit3");
//       while (!cmds.resolved()) {
//          cout << "   enter command: ";
//          cin >> cmd_str >> Argv;
//          cmds.run(cmd_str, Argv);
//       }
//
// TODO: overload operator>> function to work on vectors
//
bool commands::resolved() {
   return is_resolved;
}

// reset
//
// Sets resolved to false.
//
void commands::reset() {
   is_resolved = false;
}
