#include "jsonFactory.hpp"
#include "universeNode.hpp"
#include "nodeTypes/star.hpp"


universeNode jsonFactory::getUniverseNode(const nlohmann::json &j)
{
	switch (j["type"].get<nodeType>()){
	case STAR:{
		return star(j);
	}
	case BLACK_HOLE:{
		break;
	}
	case PLANET_GAS:{
		break;
	}
	case PLANET_ROCK:{
		break;
	}
	case ASTEROID:{
		break;
	}
	case COMET:{
		break;
	}
	case ARTIFICIAL_SATELLITE:{
		break;
	}
	case SPACE_STATION:{
		break;
	}
	}
	return star(j);
}
