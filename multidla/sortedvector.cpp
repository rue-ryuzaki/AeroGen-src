#include "sortedvector.h"

//template <class T, class Allocator>
//void sorted_vector<T, Allocator>::add(const T& val)
//{
//  if (mVec.empty()) {
//      mVec.push_back(val);
//  } else {
//      int32_t place = this->find_insert_pos(val);
//      // if (this->binary_search(0, mVec.size(), val, place))
//      if (place != -1) {
//          if (place == mVec.size()) {
//              mVec.push_back(val);
//          } else {
//              typename std::vector<T>::iterator it = mVec.begin() + place;
//              mVec.insert(it, val);
//          }
//      } else {
//          // don't insert repeated value
//      }
//  }
//}

//template <class T, class Allocator>
//int32_t sorted_vector<T, Allocator>::find_insert_pos(const T& key) const {
//  int32_t res = 0;
//  for (iterator it = mVec.begin(); it < mVec.end(); ++it) {
//      if (*it > key) {
//          return res;
//      } else if (*it == key) {
//          return -1;
//      }
//      res++;
//  }
//  return res;
//}
