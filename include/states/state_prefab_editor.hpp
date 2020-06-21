#pragma once
#include "state_base.hpp"
#include "HI2.hpp"
#include "block.hpp"
#include "prefab.hpp"
#include <deque>
#include <set>

namespace State {
	class PrefabEditor : public State_Base {
	  public:
		PrefabEditor(gameCore& c, std::string name);
		PrefabEditor(gameCore& c, std::string name, point3Di size);
		~PrefabEditor() override;

		void input(double dt) override; 
		void update(double dt) override;
		void draw(double dt) override;

	  private:
		void save();
		void load();

		void reloadTerrainTable();
		void initToolbar();
		void drawBG();

		void applyTool(point3Di pos, bool rightClick);

		void applyPencil(point3Di pos, bool rightClick);
		void applyBucket(point3Di pos);
		void applySelect(point3Di pos);

		void floodFill(std::deque<point3Di>& pendingPositions, std::set<point3Di>& visited,baseBlock* targetBlock, baseBlock* newBlock);


		int _bgType = 0;
		bool _drawingHelp = false;
		bool _drawStats = true;
		enum class symmetry
		{
			NONE,
			V,
			H,
			BOTH
		}_symmetryMode = symmetry::NONE;
		enum class tool
		{
			PENCIL,
			BUCKET,
			SELECT
		}_currentTool = tool::PENCIL;

		prefab _prefab;
		point3Di _camera;
		blockRotation _rotation = blockRotation::UP;
		bool _flip = false;
		int _selectedToolbarPos = 0;
		std::array<baseBlock*,9> _toolbar;
		double zoom = 1;
        bool _drawInvisible = false;
        unsigned _cameraDepth = 3;
	};
}
