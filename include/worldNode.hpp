#pragma once
#include <memory>
#include <vector>


class worldNode {
  private:
	std::vector<std::unique_ptr<worldNode>> children;
	worldNode *parent;

  public:
	worldNode();
};
