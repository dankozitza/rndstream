//
// jconfig.cpp
//
// Created by Daniel Kozitza
//

#include "jconfig.hpp"

jconfig::jconfig() {
   tmp_file_path = string(JCONFIG_TMP_PATH) + string(JCONFIG_TMP_FILENAME);
   tmp_operation = false;
   file_path = tmp_file_path;
   m = unordered_map<string, cfgval>();
   mit = m.begin();
}

jconfig::jconfig(string file) {
   tmp_file_path = string(JCONFIG_TMP_PATH) + string(JCONFIG_TMP_FILENAME);
   tmp_operation = false;
   file_path = file;
   m = unordered_map<string, cfgval>();
   mit = m.begin();
}

void jconfig::set_file_location(string fl) {
   file_path = fl;
   return;
}

void jconfig::define_int(string key, int val) {
   define_vint(key, {val});
   return;
}
void jconfig::define_vint(string key, vector<int> val) {
   cfgval nv;
   nv.type = "int";
   nv.set = false;
   nv.vint = val;
   nv.vstr.resize(val.size());
   m[key] = nv;
   return;
}

void jconfig::define_uint(string key, unsigned int val) {
   define_vuint(key, {val});
   return;
}
void jconfig::define_vuint(string key, vector<unsigned int> val) {
   cfgval nv;
   nv.type = "uint";
   nv.set = false;
   nv.vuint = val;
   nv.vstr.resize(val.size());
   m[key] = nv;
   return;
}

void jconfig::define_dbl(string key, double val) {
   define_vdbl(key, {val});
   return;
}
void jconfig::define_vdbl(string key, vector<double> val) {
   cfgval nv;
   nv.type = "double";
   nv.set = false;
   nv.vdbl = val;
   nv.vstr.resize(val.size());
   m[key] = nv;
   return;
}

void jconfig::define_str(string key, string val) {
   define_vstr(key, {val});
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

void jconfig::define_bool(string key, bool val) {
   cfgval nv;
   nv.type = "bool";
   nv.set = false;
   nv.bval = val;
   m[key] = nv;
   return;
}

void jconfig::define_btn(string key) {
   cfgval nv;
   nv.type = "button";
   nv.set = false;
   nv.bval = false;
   m[key] = nv;
  return;
}

int jconfig::get_int(string key) {
   if (m[key].vint.size() < 1) {return 0;}
   return m[key].vint[0];
}
vector<int> jconfig::get_vint(string key) {
   return m[key].vint;
}

unsigned int jconfig::get_uint(string key) {
   if (m[key].vuint.size() < 1) {return 0;}
   return m[key].vuint[0];
}
vector<unsigned int> jconfig::get_vuint(string key) {
   return m[key].vuint;
}

double jconfig::get_dbl(string key) {
   if (m[key].vdbl.size() < 1) {return 0;}
   return m[key].vdbl[0];
}
vector<double> jconfig::get_vdbl(string key) {
   return m[key].vdbl;
}

string jconfig::get_str(string key) {
   if (m[key].vstr.size() < 1) {return "";}
   return m[key].vstr[0];
}
vector<string> jconfig::get_vstr(string key) {
   return m[key].vstr;
}

bool jconfig::get_bool(string key) {
   return m[key].bval;
}

bool jconfig::get_btn(string key) {
   return m[key].bval;
}

void jconfig::set(string key, int val) {
   vector<int> tmp = {val};
   set(key, tmp);
   return;
}
void jconfig::set(string key, vector<int> vval) {
   cfgval tmp;
   tmp.type = "int";
   tmp.vint = vval;
   tmp.set = true;
   m[key] = tmp;
   return;
}

void jconfig::set(string key, unsigned int val) {
   vector<unsigned int> tmp = {val};
   set(key, tmp);
   return;
}
void jconfig::set(string key, vector<unsigned int> vval) {
   cfgval tmp;
   tmp.type = "uint";
   tmp.vuint = vval;
   tmp.set = true;
   m[key] = tmp;
   return;
}

void jconfig::set(string key, double val) {
   vector<double> tmp = {val};
   set(key, tmp);
   return;
}
void jconfig::set(string key, vector<double> vval) {
   cfgval tmp;
   tmp.type = "double";
   tmp.vdbl = vval;
   tmp.set = true;
   m[key] = tmp;
   return;
}

void jconfig::set(string key, string val) {
   vector<string> tmp = {val};
   set(key, tmp);
   return;
}
void jconfig::set(string key, vector<string> vval) {
   cfgval tmp;
   tmp.type = "string";
   tmp.vstr = vval;
   tmp.set = true;
   m[key] = tmp;
   return;
}

void jconfig::set(string key, bool val) {
   m[key].type = "bool";
   m[key].bval = val;
   m[key].set = true;
   return;
}

bool jconfig::is_set(string key) {
   return m[key].set;
}

tools::Error jconfig::convert() {
   tools::Error e = NULL;
   JValue jv;

   for (mit = m.begin(); mit != m.end(); mit++) {

      if (mit->second.set == false) {continue;}
      if (mit->second.type == "string") {continue;}

      if (mit->second.type == "bool") {
         if (mit->second.bval) {mit->second.bval = false;}
         else {mit->second.bval = true;}
         continue;
      }

      if (mit->second.type == "button") {
         mit->second.bval = true;
         continue;
      }

      if (mit->second.type == "int") {
         mit->second.vint.resize(mit->second.vstr.size());
      }
      if (mit->second.type == "uint") {
         mit->second.vuint.resize(mit->second.vstr.size());
      }
      if (mit->second.type == "double") {
         mit->second.vdbl.resize(mit->second.vstr.size());
      }

      for (int i = 0; i < mit->second.vstr.size(); i++) {
         //e = load_json_value_from_string(jv, mit->second.vstr[i]);
         e = jv.load_value(mit->second.vstr[i]);
         if (e != NULL) {return e;}

         if (mit->second.type == "int") {
            if (!jv.isInt()) {return "jconfig: Wrong type. Need int.";}
            mit->second.vint[i] = jv.asInt();
         }
         if (mit->second.type == "uint") {
            if (!jv.isUInt()) {return "jconfig: Wrong type. Need uint.";}
            mit->second.vuint[i] = jv.asUInt();
         }
         if (mit->second.type == "double") {
            if (!jv.isDouble() && !jv.isInt()) {
               return "jconfig: Wrong type. Need double.";
            }
            mit->second.vdbl[i] = jv.asDouble();
         }
      }
   }

   return e;
}

tools::Error jconfig::load() {
   tools::Error e = NULL;
   JValue rjv;
   //e = tools::load_json_value_from_file(rjv, file_path);
   //if (e != NULL) {return e;}
   string json_str = "";
   e = tools::read_file(file_path, json_str);

   if (e != NULL) {return e;}

   e = rjv.load_value(json_str);
   if (e != NULL) {return e;}

   if (rjv.isObject()) {
      map<string, JValue>::iterator rjvit = rjv.data.object_val.begin();
      for (; rjvit != rjv.data.object_val.end(); rjvit++) {

         string key = rjvit->first;

         JValue elmntv(rjvit->second);
         if (!elmntv.isArray()) {
            if (elmntv.isBool()) {
               m[key].bval = elmntv.asBool();
            }
            else {
               return errorf("Error: jconfig key '%s' has no value",
                             key.c_str());
            }
            continue;
         }

         if (m[key].type == "string") {
            m[key].vstr.resize(elmntv.size());

            for (int i = 0; i < elmntv.size(); i++) {
               if (!elmntv[i].isString()) {
                  return error("jconfig: Config value is not a string.");
               }
               m[key].vstr[i] = elmntv[i].asString();
            }
         }
         if (m[key].type == "int") {
            m[key].vint.resize(elmntv.size());

            for (int i = 0; i < elmntv.size(); i++) {
               if (!elmntv[i].isInt()) {
                  return error("jconfig: Config value is not an int.");
               }
               m[key].vint[i] = elmntv[i].asInt();
            }
         }
         if (m[key].type == "uint") {
            m[key].vuint.resize(elmntv.size());

            for (int i = 0; i < elmntv.size(); i++) {
               if (!elmntv[i].isUInt()) {
                  return error("jconfig: Config value is not an uint.");
               }
               m[key].vuint[i] = elmntv[i].asUInt();
            }
         }
         if (m[key].type == "double") {
            m[key].vdbl.resize(elmntv.size());

            for (int i = 0; i < elmntv.size(); i++) {
               if (!elmntv[i].isDouble()) {
                  return error("jconfig: Config value is not a double.");
               }
               m[key].vdbl[i] = elmntv[i].asDouble();
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

tools::Error jconfig::save_tmp(const char * name) {
   tmp_file_path = string(JCONFIG_TMP_PATH) +
                   string(name) + "_" +
                   string(JCONFIG_TMP_FILENAME);

   tmp_operation = true;
   string fc = getJSON();
   Error e = tools::write_file(tmp_file_path, fc);
   tmp_operation = false;
   return e;
}

tools::Error jconfig::load_tmp(const char * name) {

   tmp_file_path = string(JCONFIG_TMP_PATH) +
                   string(name) + "_" +
                   string(JCONFIG_TMP_FILENAME);

   tmp_operation = true;
   tools::Error e = NULL;
   string real_file_path = file_path;
   set_file_location(tmp_file_path);
   e = load();
   set_file_location(real_file_path);
   tmp_operation = false;
   return e;
}

string jconfig::getJSON() {
   string r = "{\n";
   int keycnt = 0;
   unordered_map<string, cfgval> wm = {};
   for (mit = m.begin(); mit != m.end(); mit++) {
      if (mit->second.type != "button") {

         if (!tmp_operation && mit->first.substr(0, 4) == "tmp_") { continue; }

         wm[mit->first] = mit->second;
      }
   }
   for (mit = wm.begin(); mit != wm.end(); mit++) {

      r += "\"" + mit->first + "\": ";

      if (mit->second.type == "bool") {
         if (mit->second.bval) {r += "true";}
         else {r += "false";}
      }
      else {
         r += "[";
         if (mit->second.type == "string") {
            for (int i = 0; i < mit->second.vstr.size(); i++) {

               string str1 = mit->second.vstr[i];
               string str2 = "";

               for (int si = 0; si < str1.size(); si++) {
                  if (str1[si] == '"') {
                     str2 += "\\\"";
                  }
                  else if (str1[si] == '\\') {
                     str2 += "\\\\";
                  }
                  else {
                     str2 += str1[si];
                  }
               }

               if (str1.size() != str2.size()) {
                  str1 = str2;
               }


               r += "\"" + str1 + "\"";

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
         if (mit->second.type == "double") {
            for (int i = 0; i < mit->second.vdbl.size(); i++) {
               char buf[100];
               sprintf(buf, "%lf", mit->second.vdbl[i]);
               r += buf;
               if (i != mit->second.vdbl.size()-1) {r += ", ";}
            }
         }
         r += "]";
      }

      if (keycnt != wm.size() - 1) {r += ",\n";}
      else {r += "\n";}
      keycnt++;
   }
   r += "}";
   return r;
}
