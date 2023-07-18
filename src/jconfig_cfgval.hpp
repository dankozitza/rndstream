//
// jconfig_cfgval.hpp
//
// Created by Daniel Kozitza
//

#ifndef _JCONFIG_CFGVAL
#define _JCONFIG_CFGVAL

#include <unordered_map>
#include <vector>

struct cfgval {
   std::string               type;
   bool                      set;
   bool                      bval;
   std::vector<int>          vint;
   std::vector<std::string>  vstr;
   std::vector<double>       vdbl;
   std::vector<unsigned int> vuint;
};

typedef std::unordered_map<std::string, cfgval> JCFG_umap;

#endif
