// 
// JValue.cpp
//
// Created by Daniel Kozitza
//

#include "JValue.hpp"

JValue::JValue() {

   store_strings   = true;
   data            = {};
   data.type       = "NULL";
   data.defined    = false;
   data.parent_key = "";
   data.bool_val   = false;
   data.int_val    = 0;
   data.str_val    = "";
   data.dbl_val    = 0.0;
   data.array_val  = vector<JValue>();
   data.object_val = map<string, JValue>();

   return;
}

JValue::JValue(jsonval json_struct_val) {
   store_strings = true;
   data = {};
   data.type = json_struct_val.type;
   data.defined = json_struct_val.defined;
   data.parent_key = json_struct_val.parent_key;
   data.bool_val = json_struct_val.bool_val;
   data.int_val = json_struct_val.int_val;
   data.str_val = json_struct_val.str_val;
   data.dbl_val = json_struct_val.dbl_val;
   if (json_struct_val.type == "array") {
      data.array_val = vector<JValue>();
      for (size_t i = 0; i < json_struct_val.array_val.size(); i++) {
         data.array_val.push_back(json_struct_val.array_val[i]);
      }
   }
   if (json_struct_val.type == "object") {
      data.object_val = map<string, JValue>();
      map<string, JValue>::iterator mit = json_struct_val.object_val.begin();
      for (; mit != json_struct_val.object_val.end(); mit++) {
         data.object_val[mit->first] = mit->second;
      }
   }
   return;
}

JValue::JValue(const JValue& source_value) {
   *this = JValue(source_value.data);
   return;
}

void JValue::clear() {
   *this = JValue();
   return;
}

JValue::Error JValue::load_value(string& json_string) {
   clear();
   return load_value_r(json_string, 0, json_string.size() - 1);
}

JValue::Error JValue::load_value_r(string& json_string, size_t begin, size_t end) {
   JValue::Error e = NULL;

   if (data.type == "NULL") {
      // determine the type
      for (size_t i = begin; i <= end; i++) {
         char l = json_string[i];

         if (l == ' ' || l == '\n') { continue; }

         if (l == '{') {
            data.type = "object";
            return load_value_r(json_string, i+1, get_eos(json_string, i)-1);
         }
         else if (l == '[') {
            data.type = "array";
            return load_value_r(json_string, i+1, get_eos(json_string, i)-1);
         }
         else if (l == '"') {
            data.type = "string";
            return load_value_r(json_string, i+1, get_eos(json_string, i)-1);
         }
         else {
            // data type must be int, float, or bool
            if (json_string.substr(i, end - i + 1) == "true") {
               data.type = "bool";
               if (store_strings) {data.str_val = "true";}
               data.bool_val = true;
            }
            else if (json_string.substr(i, end - i + 1) == "false" ) {
               data.type = "bool";
               if (store_strings) {data.str_val = "false";}
               data.bool_val = false;
            }
            else {
               data.type = "integer";
               for (size_t x = i; x <= end; x++) {
                  if ((json_string[x] >= '0' && json_string[x] <= '9') ||
                      json_string[x] == '-' || json_string[x] == '.') {

                     if (json_string[x] == '.') {
                        data.type = "float";
                        break;
                     }
                  }
                  else {
                     return "JValue::load_value: json value is not a number or boolean.";
                  }
               }
            }
            if (data.type == "integer") {
               data.int_val = stol(json_string.substr(i, end - i + 1));
               if (store_strings) {
                  data.str_val = json_string.substr(i, end - i + 1);
               }
            }
            else if (data.type == "float") {
               data.dbl_val = stod(json_string.substr(i, end - i + 1));
               if (store_strings) {
                  data.str_val = json_string.substr(i, end - i + 1);
               }
            }
            data.defined = true;
            return NULL;
         }
      }
   }
   else {
      // type is known, build the json value
      string key = "";
      JValue val;
      size_t val_begin = begin;

      if (data.type == "object") {
         // scan for a list of key value pairs
         bool scan_val = false;
         for (size_t j = begin; j <= end; j++) {
            char l = json_string[j];

            if (scan_val) {
               if (l != ',' && j != end) {
                  //cout << "scanning object pair\n";
               }
               else {
                  scan_val = false;
                  val.data.parent_key = key;
                  data.object_val[key] = val;
                  if (j != end) {
                     e = data.object_val[key].load_value_r(json_string, val_begin, j - 1);
                  }
                  else {
                     e = data.object_val[key].load_value_r(json_string, val_begin, j);
                  }
                  if (e != NULL) {
                     return e;
                  }
                  key = "";
                  val_begin = j + 1;
               }
            }
            else if (l == '"') {
               size_t val_end = get_eos(json_string, j) - 1;
               key = json_string.substr(j+1, val_end - j);

               if (val_end == j || val_end == j + 1) {
                  return "JValue::load_value: value out of range";
               }
               else {
                  j = val_end + 1;
                  if (j <= end && json_string[j+1] != ':') {
                     j++;
                  }
               }
            }
            if (key != "" && l == ':') {
               val_begin = j + 1;
               scan_val = true;
            }
         }
      }
      else if (data.type == "array") {
         // scan for a list of elements
         val_begin = begin;
         for (size_t j = begin; j <= end; j++) {
            char l = json_string[j];

            if (l != ',' && j != end) {
               //cout << "scanning array element\n";
            }
            else {
               data.array_val.push_back(val);
               if (j != end) {
e = data.array_val[data.array_val.size() - 1].load_value_r(json_string, val_begin, j - 1);
               }
               else {
e = data.array_val[data.array_val.size() - 1].load_value_r(json_string, val_begin, j);
               }
               if (e != NULL) {
                  return e;
               }
               val_begin = j + 1;
            }
         }
      }
      else if (data.type == "string") {
         data.str_val = json_string.substr(begin, end - begin + 1);
         //cout << "got value: " << data.str_val.c_str() << endl;
      }
   }

   data.defined = true;
   return NULL;
}

size_t JValue::get_eos(string& json_string, size_t bos_index) {

   stack<char> depth;

   for (size_t i = bos_index; i < json_string.size(); i++) {
      char l = json_string[i];

      if (l == '\\') {
         i++;
         continue;
      }

      if (depth.size() > 0 && l == depth.top()) {
         depth.pop();
         if (depth.size() == 0) {
            return i;
         }
         else {
            continue;
         }
      }

      if (depth.size() == 0 || depth.top() != '"') {
         if      (l == '{') { depth.push('}'); }
         else if (l == '[') { depth.push(']'); }
      }

      if (l == '"' && (depth.size() == 0 || depth.top() != '"')) {
         depth.push('"');
      }
   }

   return bos_index;
}

JValue::Error JValue::load_list(istream& json_istream) {

   return NULL;
}

JValue JValue::get_list_value() {
   JValue rval;
   return rval;
}

string JValue::get_type() {
   return data.type;
}

bool JValue::is_object() {
   if (data.type == "object") {
      return true;
   }
   return false;
}
bool JValue::isObject() {
   return is_object();
}

bool JValue::is_array() {
   if (data.type == "array") {
      return true;
   }
   return false;
}
bool JValue::isArray() {
   return is_array();
}

bool JValue::is_int() {
   if (data.type == "integer") {
      return true;
   }
   return false;
}
bool JValue::isInt() {
   return is_int();
}

bool JValue::is_uint() {
   if (data.type == "integer" && data.int_val >= 0) {
      return true;
   }
   return false;
}
bool JValue::isUInt() {
   return is_uint();
}

bool JValue::is_dbl() {
   if (data.type == "float") {
      return true;
   }
   return false;
}
bool JValue::isDouble() {
   return is_dbl();
}

bool JValue::is_str() {
   if (data.type == "string") {
      return true;
   }
   return false;
}
bool JValue::isString() {
   return is_str();
}

bool JValue::is_bool() {
   if (data.type == "bool") {
      return true;
   }

   return false;
}
bool JValue::isBool() {
   return is_bool();
}

bool JValue::is_defined() {
   return data.defined;
}

long int JValue::get_int() {
   return data.int_val;
}
long int JValue::asInt() {
   return get_int();
}

unsigned int JValue::get_uint() {
   if (data.int_val < 0) {
      return 0;
   }
   return (unsigned int) data.int_val;
}
unsigned int JValue::asUInt() {
   return get_uint();
}

double JValue::get_dbl() {
   if (data.type == "integer") {
      return (double) data.int_val;
   }
   return data.dbl_val;
}
double JValue::asDouble() {
   return get_dbl();
}

string JValue::get_str() {
   return data.str_val;
}
string JValue::asString() {
   return get_str();
}

bool JValue::get_bool() {
   return data.bool_val;
}
bool JValue::asBool() {
   return get_bool();
}

map<string, JValue> JValue::get_object() {
   return data.object_val;
}
map<string, JValue> JValue::asObject() {
   return get_object();
}

vector<JValue> JValue::get_array() {
   return data.array_val;
}
vector<JValue> JValue::asArray() {
   return get_array();
}

size_t JValue::size() {
   if (data.type == "array") {
      return data.array_val.size();
   }
   if (data.type == "object") {
      return data.object_val.size();
   }
   return 0;
}

JValue JValue::operator[](size_t index) {
   return data.array_val[index];
}

JValue JValue::operator[](string key) {
   return data.object_val[key];
}

string JValue::get_JSON() {
   string str = "";
   serialize(str);
   return str;
}

void JValue::serialize(string& json_str) {
   if (data.type == "string") {
      json_str += "\"" + data.str_val + "\"";
      return;
   }
   if (data.type == "integer") {
      json_str += data.str_val;
      return;
   }
   if (data.type == "float") {
      json_str += data.str_val;
      return;
   }
   if (data.type == "bool") {
      json_str += data.str_val;
      return;
   }
   if (data.type == "object") {
      json_str += "{";
      map<string, JValue>::iterator mit = data.object_val.begin();
      size_t i = 0;
      for (; mit != data.object_val.end(); mit++) {
         string key = mit->first;
         string val = "";
         data.object_val[key].serialize(val);
         json_str += "\"" + key + "\": " + val;

         if (i + 1 != data.object_val.size()) {
            json_str += ", ";
         }
         i++;
      }
      json_str += "}";
      return;
   }
   if (data.type == "array") {
      json_str += "[";
      for (size_t i = 0; i < data.array_val.size(); i++) {
         data.array_val[i].serialize(json_str);
         if (i + 1 != data.array_val.size()) {
            json_str += ", ";
         }
      }
      json_str += "]";
      return;
   }
   return;
}
