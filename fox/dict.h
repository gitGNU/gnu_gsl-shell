#ifndef DICT_H
#define DICT_H

#include "agg-plot/list.h"

template <typename Key, typename Value>
class dict {

  struct pair {
    Key key;
    Value value;

    pair(const Key& _key, const Value& _val) : key(_key), value(_val) {}
  };

  list<pair>* m_map;

public:
  typedef list<pair> iterator;

  dict() : m_map(0) {}

  ~dict() { list<pair>::free(m_map); }

  void insert(const Key& key, const Value& val) {
    m_map = new list<pair>(pair(key, val), m_map);
  }

  bool search(const Key& key, Value& result) {
    for (list<pair>* p = m_map; p; p = p->next())
      {
	const pair& ass = p->content();
	if (ass.key == key) {
	  result = ass.value;
	  return true;
	}
      }
    return false;
  }

  list<pair>* start() { return m_map; }

  // the user should check himself if p is not null
  list<pair>* next(list<pair>* p) { return p->next(); }
};

#endif
