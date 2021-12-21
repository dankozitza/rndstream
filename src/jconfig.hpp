// 
// jconfig.hpp
//
// Configuration structure for c++ applications.
//
// Created by Daniel Kozitza
//

#ifndef _JCONFIG
#define _JCONFIG

#include <string>
#include <vector>
#include <unordered_map>
#include "tools.hpp"

using namespace tools;

class jconfig {

   public:

      string file_path;

      // types: int, uint, double, string, bool

      struct cfgval {
         string         type;
         bool           set;
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
      void define_str(string key, string val);
      void define_vstr(string key, vector<string> val);
      void set(string key, unsigned int val);
      void set(string key, vector<string>& val);

      //int  get_i(string key);
      //void get(vector<int>& rval);

      tools::Error convert();

      tools::Error load();
      tools::Error save();

      string getJSON();
};

#endif
