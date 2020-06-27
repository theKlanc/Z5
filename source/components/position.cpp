#include "components/position.hpp"

void to_json(nlohmann::json& j, const position& n){
	j=nlohmann::json{{"pos",n.pos},{"parent",n.parent->getID()}};
}

void from_json(const nlohmann::json& j, position& n)
{
	n.pos=j.at("pos");
	n.parentID=j.at("parent");
}
fdd position::getRPos() const
{
	return config::extrapolateRenderPositions ? rpos : pos;
}

void position::setRPos(fdd p)
{
	rpos = p;
}

universeNode *position::calculateBestParent()
{
	std::vector<universeNode*> candidates;

	candidates.push_back(parent);
	const auto& brethren = parent->getChildren();
	for(auto& brotha : brethren){
		candidates.push_back(brotha);
	}
	if(parent->getParent()){
		candidates.push_back(parent->getParent());
	}

	// is first < second ?
	//IC("calculating parent for node " + _name);
	return *std::max_element(candidates.begin(),candidates.end(),[this](universeNode* a, universeNode* b){
		fdd posOnA = a->getLocalPos(pos,parent);
		fdd posOnB = b->getLocalPos(pos,parent);

		bool isInsideA = a->getTheoreticalBlock(posOnA.getPoint3Di()) != metaBlock::nullBlock;
		bool isInsideB = b->getTheoreticalBlock(posOnB.getPoint3Di()) != metaBlock::nullBlock;
		if(isInsideA && !isInsideB){
			//IC("	is inside " +a->getName() + " but NOT inside " + b->getName());
			return false;
		}
		else if(!isInsideA && isInsideB){
			//IC("	is inside " +b->getName() + " but NOT inside " + a->getName());
			return true;
		}
		else{
			if(isInsideA){//is inside both
				//IC("	is inside BOTH " +a->getName() + " AND " + b->getName());
				return a->getDiameter() > b->getDiameter(); //if a is bigger, then b is a better parent
			}
			double ASOI = a->getSOI();
			double BSOI = b->getSOI();
			if(ASOI > posOnA.magnitude() && BSOI > posOnB.magnitude()){//is inside both sois
				//IC("	is inside BOTH SOIS OF " +a->getName() + " AND " + b->getName());
				return BSOI < ASOI;
			}
			else if(ASOI > posOnA.magnitude()){ //is only inside A soi
				//IC("	is inside SOI of " +a->getName());
				return false;
			}
			else if(BSOI > posOnB.magnitude()){//is inside B SOI
				//IC("	is inside SOI of " +b->getName());
				return true;
			}
			//not inside any SOI
			//IC("	is not inside any SOI of " +a->getName() + " NOR " + b->getName());
			return posOnB.magnitude() < posOnA.magnitude();
		}
		;
	});
}
