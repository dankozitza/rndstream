// 
// jconfig.hpp
//
// Data structure used for program configuration.
//
// Created by Daniel Kozitza
//

#ifndef _JCONFIG
#define _JCONFIG

#include <unordered_map>
#include "tools.hpp"

using namespace tools;

class jconfig {

   public:

      string file_path;

      // types: int, uint, double, string, bool
      // bool type has no arguments and toggles bval when it is set

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

      jconfig(string file);

      void set_file_location(string fl);

      void define_int(string key, int val);
      void define_uint(string key, unsigned int val);
      void define_dbl(string key, double val);
      void define_str(string key, string val);
      void define_bool(string key, bool val);

      void define_vint(string key, vector<int> val);
      void define_vuint(string key, vector<unsigned int> val);
      void define_vdbl(string key, vector<double> val);
      void define_vstr(string key, vector<string> val);

      int          get_int(string key);
      double       get_dbl(string key);
      string       get_str(string key);
      bool         get_bool(string key);
      unsigned int get_uint(string key);

      vector<int>          get_vint(string key);
      vector<unsigned int> get_vuint(string key);
      vector<double>       get_vdbl(string key);
      vector<string>       get_vstr(string key);

      tools::Error convert();
      tools::Error load();
      tools::Error save();

      string getJSON();
};

#endif
