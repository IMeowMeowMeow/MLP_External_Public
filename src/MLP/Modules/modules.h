#pragma once
#include <windows.h>
#include <imgui/imgui.h>
#include <windef.h>
#include <string>
#include "../MLP/Headers/globals.h"
#include "../Headers/enums.h"

using namespace globals;

namespace Modules {

	namespace Test {
		void Enable();
		void Disable();
		void Thread();
		bool IsRunning();
	}

	namespace Misc {
		void Enable();
		void Disable();
		void Thread();
		bool IsRunning();

		void SetAlive(bool TRUE_0);
		void SetSilentGun(bool TRUE_0);
		void FovTP();
		void HostTP();
	}

	namespace FlyHack {
		void Enable();
		void Disable();
		void Thread();
		bool IsRunning();

		void NoPush();
		void NoFall();
		void NoClip();
	}

	namespace FreeCam {
		void Enable();
		void Disable();
		void Thread();
		bool IsRunning();

		void NoPush();
		void AttachCamera();
		void DetachCamera();
	}


	namespace Aimbot {
		void Enable();
		void Disable();
		void Thread();
		bool IsRunning();

		mec_pawn* getNearestMec(bool check_fov = false, float fov_radius = 0.0f);
		bool IsInFOV(ImVec2& screen_pos, float fov_radius);
		void DrawFOV(float fov_radius);
	}

	namespace ESP {
		namespace Players {
			void Draw();
		}

		namespace Items {
			void Draw();
			std::string ConvertStateToName(const std::string& item_name, int state_value, int time_value);
			ImColor GetItemColor(const std::string& itemName);
			bool IsSelectedItem(const std::string& item_name);
		}

		namespace Tasks {
			void Draw();
		}
	}

	namespace Handler {
		void Enable();
	}
}