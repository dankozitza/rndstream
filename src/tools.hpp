//
// tools.hpp
//
// utility functions for mc
//
// Created by Daniel Kozitza
//

#ifndef _TOOLS
#define _TOOLS

#include <iostream>
#include <map>
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include "JValue.hpp"

using namespace std;

namespace tools {

   // utils.cpp
   bool get_vfnmkmc_conf(map<string, string>& config);
   bool save_vfnmkmc_conf(map<string, string>& config);
   string get_src_files(string src_dir);

   // system.cpp
   typedef char const * Error;
   Error error(string message = "");
   Error errorf(char const * format, ...);
   void signals_callback_handler(int signum);
   void scbh_return(int signum);
   void scbh_return_quiet(int signum);
   void cfgsigs_cbh_return_quiet(int signum);
   void signals(int sig, void (*callback_func)(int));
   bool require(int sys_exit_val, string msg = "");
   bool require(bool func_return_val, string msg = "");
   bool dir_exists(string dir_name);
   bool list_dir(string dir_name, vector<string>& contents);
   bool list_dir_r(string dir_name, vector<string>& contents);
   bool list_dir_r(
         string dir_name,
         vector<string>& contents,
         string prefix);
   Error read_file(string file_path, string& contents);
   Error write_file(string file_path, string& contents);

   // strings.cpp
   string fold(int indent_width, int max_line_width, string s);

   // pcre_utils.cpp
   bool pmatches(string s, string str_re);
   bool pmatches(string results[], string s, string str_re);
   bool pmatches(vector<string> &results, string s, string str_re);
   bool pmatches(string results[], string s, string str_re, bool get_res);
   bool pmatches(
         vector<string> &results,
         string s,
         string str_re,
         bool get_res);
   void test_pmatches();
   bool replace_first(string &s, string str_re, string rpl);
   size_t replace_all(string &s, string str_re, string rpl);

   // vectors.cpp
   istream& operator>>(istream& is, vector<string>& v);
   ostream& operator<<(ostream& os, vector<string>& v);
   void chop(char separator, string& source, vector<string>& result);
   void vectors_test();

   // doubles.cpp
   bool equal(double const &d1, double const &d2, double epsilon = 0.0001);

   // json_utils.cpp
   Error load_json_value_from_string(JValue& jv, const string& s);
   Error load_json_value_from_file(JValue& jv, string fname);

   // rand_utils.cpp
   unsigned int srandtp();
}

#endif
