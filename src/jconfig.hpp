// 
// jconfig.hpp
//
// Configuration structure for c++ applications.
//
// Created by Daniel Kozitza
//

#ifndef _RNDZ_JCONFIG
#define _RNDZ_JCONFIG

#include <string>
#include <vector>
#include <unordered_map>
#include "tools.hpp"

using namespace tools;

class jconfig {

   public:

      string file_path;

      struct cfgval {
         string         type;
         bool           set;
         string         sval;
         unsigned int   uival;
      };

   //double         dval;
   //vector<cfgval> vval;
   //unordered_map<string, cfgval> mval;

      unordered_map<string, cfgval> m;

      jconfig(string file);

      void set_file_location(string fl);
      void set(string key, string val);
      void set(string key, unsigned int val);
      string getJSON();
};

#endif
