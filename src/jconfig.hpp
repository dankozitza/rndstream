// 
// jconfig.hpp
//
// Data structure used for program configuration.
//
// Created by Daniel Kozitza
//

#ifndef _JCONFIG
#define _JCONFIG

#ifndef __ANDROID__
   #define JCONFIG_TMP_PATH "/tmp/"
#else
   #define JCONFIG_TMP_PATH "/data/data/com.termux/files/usr/tmp/"
#endif

#define JCONFIG_TMP_FILENAME "DKM_TEMPORARY.json"

#include <unordered_map>
#include <vector>
#include "tools.hpp"

using namespace tools;

class jconfig {

   public:

      string file_path;
      string tmp_file_path;
      bool   tmp_operation;

      // types: int, uint, double, string, bool, button
      // bool and button types have no arguments.
      // buttons are off unless set on the command line,
      // bools are toggled by options on the command line.

      struct cfgval {
         string         type;
         bool           set;
         bool           bval;
         vector<int>    vint;
         vector<string> vstr;
         vector<double> vdbl;
         vector<unsigned int> vuint;
      };

      unordered_map<string, cfgval> m;
      unordered_map<string, cfgval>::iterator mit;

      jconfig();
      jconfig(string file);

      void set_file_location(string fl);

      void define_int(string key, int val);
      void define_uint(string key, unsigned int val);
      void define_dbl(string key, double val);
      void define_str(string key, string val);
      void define_bool(string key, bool val);
      void define_btn(string key);

      void define_vint(string key, vector<int> val);
      void define_vuint(string key, vector<unsigned int> val);
      void define_vdbl(string key, vector<double> val);
      void define_vstr(string key, vector<string> val);

      int          get_int(string key);
      double       get_dbl(string key);
      string       get_str(string key);
      bool         get_bool(string key);
      bool         get_btn(string key);
      unsigned int get_uint(string key);

      vector<int>          get_vint(string key);
      vector<unsigned int> get_vuint(string key);
      vector<double>       get_vdbl(string key);
      vector<string>       get_vstr(string key);

      void set(string key, int val);
      void set(string key, unsigned int val);
      void set(string key, double val);
      void set(string key, string val);
      void set(string key, bool val);

      void set(string key, vector<int> vval);
      void set(string key, vector<unsigned int> vval);
      void set(string key, vector<double> vval);
      void set(string key, vector<string> vval);

      bool is_set(string key);

      tools::Error convert();
      tools::Error load();
      tools::Error save();

      tools::Error load_tmp(const char * name);
      tools::Error save_tmp(const char * name);

      string getJSON();
};

#endif

