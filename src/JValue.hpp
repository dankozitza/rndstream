//
// JValue.hpp
//
// Created by Daniel Kozitza
//

#ifndef _DKM_JSON_VALUE
#define _DKM_JSON_VALUE

#include <map>
#include <vector>
#include <string>
#include <stack>
#include <iostream>

using namespace std;

class JValue {

   public:

      struct jsonval {
         string              type;
         bool                defined;
         string              parent_key;
         string              str_val;
         bool                bool_val;
         long int            int_val;
         double              dbl_val;
         vector<JValue>      array_val;
         map<string, JValue> object_val;
      };

      bool loaded;
      bool store_strings;

      typedef const char * Error;

      jsonval data;

      JValue();
      JValue(jsonval json_struct_val);
      JValue(const JValue& source_value);

      // Load the entire json value in json_string.
      // If no errors occur is_defined() returns true.
      Error load_value(string& json_string);
      Error load_value_r(string& json_string, size_t begin, size_t end);

      // Begin loading an object or array from a stream.
      Error load_list(istream& json_istream);

      // Read one element of an object or array. After the entire
      // list is read the returned value is not defined and the
      // return value of is_loaded() changes to true.
      JValue get_list_value();

      bool is_defined();

      string get_type();

      bool is_object();
      bool is_array();
      bool is_int();
      bool is_uint();
      bool is_dbl();
      bool is_str();
      bool is_bool();

      bool isObject();
      bool isArray();
      bool isInt();
      bool isUInt();
      bool isDouble();
      bool isString();
      bool isBool();

      JValue get_object();
      JValue get_array();

      long int     get_int();
      unsigned int get_uint();
      double       get_dbl();
      string       get_str();
      bool         get_bool();

      JValue asObject();
      JValue asArray();

      long int     asInt();
      unsigned int asUInt();
      double       asDouble();
      string       asString();
      bool         asBool();

      size_t size();
      void   clear();

      JValue operator[](size_t index);
      JValue operator[](string key);

      size_t get_eos(string& json_string, size_t bos_index);

      void serialize(string& json_str);
      string get_JSON();
};

#endif
