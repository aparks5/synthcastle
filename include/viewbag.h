#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "nodetypes.h"

struct EdgeSnapshot
{
	int from;
	int to;
	EdgeSnapshot()
		: from(0)
		, to(0) {}
	EdgeSnapshot(int f, int t)
		: from(f)
		, to(t) {}
};

struct NodeSnapshot{
	NodeType nodeType;
	std::unordered_map<std::string, float> params;
	std::unordered_map<std::string, std::string> stringParams;
	std::unordered_map<int, EdgeSnapshot> edges;
};

struct ViewBag
{
	std::unordered_map<int, NodeSnapshot> map;
};

//high level model if this kind of looked like JSON
//{
//"vertices":
//   {
//   "1":{
//      "attr_ids":{
//         "freq_id":3,
//         "modfreq_id":4
//      }
//   },
//   "params":{
//      "freq":500,
//      "modfreq":0.5,
//      "coarse":12,
//      "fine":0.01
//   },
//  "stringParams": {
//	    "path": "C:/sample.wav",
//   },
//   "edges":
//   {
//   "from": "to",
//   }
//}

