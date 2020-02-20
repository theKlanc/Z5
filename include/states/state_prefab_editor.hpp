#pragma once
#include "state_base.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include "block.hpp"
#include "prefab.hpp"

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

		void initToolbar();
		
		prefab _prefab;
		point3Di _camera;
		blockRotation _rotation = UP;
		int _selectedToolbarPos = 0;
		std::array<baseBlock*,9> _toolbar;
		int zoom = 1;
        bool _drawInvisible = false;
        unsigned _cameraDepth = 3;
	};
}