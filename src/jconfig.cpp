//
// jconfig.cpp
//
// Created by Daniel Kozitza
//

#include "jconfig.hpp"
//#include "tools.hpp"

//using namespace tools;

//unordered_map<string, cfgval> m;
//string m_FL;

jconfig::jconfig(string file) {
   file_path = file;
   // read config from file
}

void jconfig::set_file_location(string fl) {
   file_path = fl;
   return;
}

void jconfig::set(string key, string val) {
   cfgval nv;
   nv.type = "string";
   nv.set = true;
   nv.sval = val;
   m[key] = nv;
   return;
}

void jconfig::set(string key, unsigned int val) {
   cfgval nv;
   nv.type = "uint";
   nv.set = true;
   nv.uival = val;
   m[key] = nv;
   return;
}

string jconfig::getJSON() {
   string r;
   r = "{\n";
   int keycnt = 0;
   for (auto it = m.begin(); it != m.end(); it++) {
      r += "\"" + it->first + "\": ";

      if (it->second.type == "string") {
         r += "\"" + it->second.sval + "\"";
      }
      else if (it->second.type == "uint") {
         char buf[100];
         sprintf(buf, "%li", it->second.uival);
         r += buf;
      }

      if (keycnt != m.size() - 1) {
         r += ",\n";
      }
      else {r+= "\n";}

      keycnt++;
   }
   r += "}";
   return r;
}
