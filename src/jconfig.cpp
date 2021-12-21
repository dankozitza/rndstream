//
// jconfig.cpp
//
// Created by Daniel Kozitza
//

#include "jconfig.hpp"

//unordered_map<string, cfgval> m;
//string m_FL;

jconfig::jconfig(string file) {
   file_path = file;
   mit = m.begin();
   // read config from file
}

void jconfig::set_file_location(string fl) {
   file_path = fl;
   return;
}

void jconfig::define_int(string key, int val) {
   cfgval nv;
   nv.type = "int";
   nv.set = false;
   nv.vstr.resize(1);
   nv.vint.resize(1);
   nv.vint[0] = val;
   m[key] = nv;
   return;
}
void jconfig::define_uint(string key, unsigned int val) {
   cfgval nv;
   nv.type = "uint";
   nv.set = false;
   nv.vstr.resize(1);
   nv.vuint.resize(1);
   nv.vuint[0] = val;
   m[key] = nv;
   return;
}

void jconfig::define_vstr(string key, vector<string> val) {
   cfgval nv;
   nv.type = "string";
   nv.set = false;
   nv.vstr = val;
   m[key] = nv;
   return;
}

void jconfig::define_str(string key, string val) {
   define_vstr(key, {val});
   return;
}

void jconfig::set(string key, vector<string>& val) {
   cfgval nv;
   nv.type = "string";
   nv.set = true;
   m[key] = nv;
   m[key].vstr = val;
   return;
}

void jconfig::set(string key, unsigned int val) {
   cfgval nv;
   nv.type = "uint";
   nv.set = true;
   nv.vuint.resize(1);
   nv.vuint[0] = val;
   m[key] = nv;
   return;
}

tools::Error jconfig::convert() {
   tools::Error e = NULL;
   Json::Value jv;

   for (mit = m.begin(); mit != m.end(); mit++) {

      if (mit->second.set == false) {continue;} mit->second.set = false;
      if (mit->second.type == "string") {continue;}

      if (mit->second.type == "int") {
         mit->second.vint.resize(mit->second.vstr.size());
      }
      if (mit->second.type == "uint") {
         mit->second.vuint.resize(mit->second.vstr.size());
      }

      for (int i = 0; i < mit->second.vstr.size(); i++) {
         e = load_json_value_from_string(jv, mit->second.vstr[i]);
         if (e != NULL) {return e;}
         if (mit->second.type == "int") {
            if (!jv.isInt()) {return "jconfig: Wrong type. Need int.";}
            mit->second.vint[i] = jv.asInt();
         }
         if (mit->second.type == "uint") {
            if (!jv.isUInt()) {return "jconfig: Wrong type. Need uint.";}
            mit->second.vuint[i] = jv.asUInt();
         }
      }
   }

   return e;
}

tools::Error jconfig::load() {
   tools::Error e = NULL;
   Json::Value rjv;
   e = tools::load_json_value_from_file(rjv, file_path);
   if (e != NULL) {return e;}

   if (rjv.isObject()) {
      Json::Value::iterator rjvit = rjv.begin();
      for (rjvit; rjvit != rjv.end(); rjvit++) {

         if (!rjvit.key().isString()) {
            return "jconfig: Config key is not a string.";
         }
         string key = rjvit.key().asString();

         Json::Value elmntv = *rjvit;
         if (!elmntv.isArray()) {
            return "jconfig: Config value is not an array.";
         }

         if (m[key].type == "int") {
            m[key].vint.resize(elmntv.size());

            for (int i = 0; i < elmntv.size(); i++) {
               if (!elmntv[i].isInt()) {
                  return "jconfig: Config value is not an int.";
               }
               m[key].vint[i] = elmntv[i].asInt();
            }
         }
         if (m[key].type == "uint") {
            m[key].vuint.resize(elmntv.size());

            for (uint i = 0; i < elmntv.size(); i++) {
               if (!elmntv[i].isUInt()) {
                  return "jconfig: Config value is not an uint.";
               }
               m[key].vuint[i] = elmntv[i].asUInt();
            }
         }
      }
   }
   return e;
}

tools::Error jconfig::save() {
   string fc = getJSON();
   return tools::write_file(file_path, fc);
}

string jconfig::getJSON() {
   string r;
   r = "{\n";
   int keycnt = 0;
   for (mit = m.begin(); mit != m.end(); mit++) {
      r += "\"" + mit->first + "\": [";

      if (mit->second.type == "string") {
         for (int i = 0; i < mit->second.vstr.size(); i++) {
            r += "\"" + mit->second.vstr[i] + "\"";
            if (i != mit->second.vstr.size()-1) {r += ", ";}
         }
      }
      if (mit->second.type == "int") {
         for (int i = 0; i < mit->second.vint.size(); i++) {
            char buf[100];
            sprintf(buf, "%i", mit->second.vint[i]);
            r += buf;
            if (i != mit->second.vint.size()-1) {r += ", ";}
         }
      }
      if (mit->second.type == "uint") {
         for (int i = 0; i < mit->second.vuint.size(); i++) {
            char buf[100];
            sprintf(buf, "%u", mit->second.vuint[i]);
            r += buf;
            if (i != mit->second.vuint.size()-1) {r += ", ";}
         }
      }

      r += "]";
      if (keycnt != m.size() - 1) {
         r += ",\n";
      }
      else {r+= "\n";}

      keycnt++;
   }
   r += "}";
   return r;
}
