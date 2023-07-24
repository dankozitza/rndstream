//
// vectors.cpp
//
// Created by Daniel Kozitza
//

#include "../tools.hpp"

istream& tools::operator>>(istream& is, vector<string>& v) {
   char c, last = ' ';
   int cnt = 0;

   v.clear();

   is.get(c);
   while (is.good() && c != '\n') {

      if (c != ' ' && c != '	') {

         if (v.empty() || v.size() == cnt)
            v.push_back("");

         v[cnt] += c;
      }
      else if (last != ' ' && last != '	') {
         // if c is a space but last is not then the word is done
         cnt++;
      }

      last = c;
      is.get(c);
   }

   return is;
}

ostream& tools::operator<<(ostream& os, vector<string>& v) {
   os << "{";
   if (!v.empty()) {
      for (int i = 0; i < v.size() - 1; ++i)
         os << v[i] << ", ";

      cout << v[v.size() - 1];
   }
   os << "}";

   return os;
}

void tools::vectors_test() {

   vector<string> v1;

   cout << "enter command: ";
   cin >> v1;
   cout << "vector size: '" << v1.size() << "'\n";
   cout << "got vector: '" << v1 << "'\n\n";

   cout << "enter command: ";
   cin >> v1;
   cout << "vector size: '" << v1.size() << "'\n";
   cout << "got vector: '" << v1 << "'\n\n";
}

void tools::chop(char separator, string& source, vector<string>& result) {

   string value = "";
   for (int i = 0; i < source.size(); i++) {

      if (i+3 < source.size() && source.substr(i, 4) == string("\\\\\\") + separator) {
         value += "\\";
         value += separator;
         i += 3;
         if (i+1 >= source.size()) {
            result.push_back(value);
         }
         continue;
      }

      if (i+1 < source.size()) {
      if (source[i] == '\\' && source[i+1] == separator) {

         if (i-1 < 0 || source[i-1] != '\\') {

            value += separator;
            i++;
            if (i+1 >= source.size()) {
               result.push_back(value);
            }
            continue;
         }
         else if (i-1 >= 0 && source[i-1] == '\\') {
            continue;
         }
      }}

      if (source[i] == separator) {
         result.push_back(value);
         value = "";
         continue;
      }

      value += source[i];

      if (i == source.size() - 1) {
         result.push_back(value);
         value = "";
      }
   }
   return;
}
