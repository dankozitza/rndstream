// 
// commands.hpp
//
// Created by Daniel Kozitza
//

#ifndef _COMMANDS
#define _COMMANDS

#include <map>
#include <string>
#include <vector>

using namespace std;

class commands {

   private:
      struct Command {
         void (*func_na)();                // function with no arguments
         void (*func_wa)(vector<string>&); // function with argument list
         void (*func_wo)(ostream&);        // function with output stream
         void (*func_woa)(ostream&, vector<string>&); // function with both
         bool has_arguments;
         bool has_output;
         string synopsis;
         string usage;
         string description;
      };

      typedef map<string, Command>::iterator cmds_iter;

      map<string, Command> cmds;
      string               cmds_help;
      string               program_name;
      int                  cmd_name_width;
      int                  max_line_width;
      bool                 is_resolved;

   public:
      commands();
      void set_program_name(string pn);
      void set_cmds_help(string msg);
      void set_cmd_name_width(int w);
      void set_max_line_width(int w);
      void handle(
            string cmd,
            void (*func)(),
            string synopsis,
            string usage,
            string description = "");
      void handle(
            string cmd,
            void (*func)(ostream& out),
            string synopsis,
            string usage,
            string description = "");
      void handle(
            string cmd,
            void (*func)(vector<string>&),
            string synopsis,
            string usage,
            string description = "");
      void handle(
            string cmd,
            void (*func)(ostream&, vector<string>&),
            string synopsis,
            string usage,
            string description = "");
      void run(string cmd, vector<string>& arguments);
      void run(string cmd, vector<string>& arguments, ostream& out);
      void default_help(vector<string>& arg);
      bool resolved();
      void reset();
};

#endif
