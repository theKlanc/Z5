#pragma once
#include <memory>
#include <vector>


class worldNode {
public:
	worldNode();
  private:
	
	
	std::vector<worldNode> _children;
	std::weak_ptr<worldNode> _parent;
  
};
