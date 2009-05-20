#ifndef MOVENODE_H
#define MOVENODE_H

#include "card.h"
#include "ctlist.h"

class TMoveNode : public TCard, Tclist {
  public:
  TMoveNode(int _CName, int _CMast, int aLimit);
  TMoveNode(int _CName, int _CMast, int aLimit,int _nPlayer);
  int nPower;
  int nPlayer;

};


#endif
