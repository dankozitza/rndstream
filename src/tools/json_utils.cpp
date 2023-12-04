//
// Json_utils.cpp
//
// Created by Daniel Kozitza
//

#include "../tools.hpp"

tools::Error tools::load_json_value_from_file(JValue& jv, string fname) {

   tools::Error e = NULL;
   string filetxt;
   e = read_file(fname, filetxt);
   if (e != NULL) {
      return errorf("tools::load_json_value_from_file: %s", e);
   }

   e = load_json_value_from_string(jv, filetxt);
   if (e != NULL) {
      return errorf(
         "tools::load_json_value_from_file: Error parsing file '%s': %s",
         fname.c_str(), e);
   }
   return NULL;
}

tools::Error tools::load_json_value_from_string(JValue& jv, const string& s) {

   //Json::Reader reader;
   //if (reader.parse(s, jv, true)) {return NULL; }

   Error e = NULL;
   e = jv.load_value(s);
   return e;
}
