#include "movenode.h"
//------------------------------------------------------------------
TMoveNode::TMoveNode(int _CName, int _CMast, int aLimit) :
 TCard(_CName,_CMast),Tclist( aLimit ) {
 nPower=0;
 nPlayer = 0;
}
//------------------------------------------------------------------
TMoveNode::TMoveNode(int _CName, int _CMast, int aLimit,int _nPlayer) :
 TCard(_CName,_CMast),Tclist( aLimit ) {
 nPower=0;
 nPlayer = _nPlayer;
}
//------------------------------------------------------------------
