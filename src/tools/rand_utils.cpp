//
// rand_utils.cpp
//
// crand - re-seeds every n calls
// qrand - makes frequent calls to srand
// grand - filters output of rand by calling srand when finding 'bad' numbers
// mrand - keeps record of seeds and calls to rand. has accessible json index.
// lrand - uses dual calls to rand to produce long uints.
//
// Created by Daniel Kozitza
//

#include "../tools.hpp"

unsigned int tools::srandtp() {
   return 0;
}
