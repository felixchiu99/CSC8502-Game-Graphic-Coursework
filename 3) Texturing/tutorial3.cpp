#include "../nclgl/window.h"
#include "Renderer.h"

int main() {
	Window w("Texturing!", 1280, 720,false);	 //This is all boring win32 window creation stuff!
	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);	//This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!
	if(!renderer.HasInitialised()) {
		return -1;
	}

	float rotate = 0.0f;
	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_1)) {
			renderer.SwitchToOrthographic();
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_2)) {
			renderer.SwitchToPerspective();
		}

		if(Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT) ) {
			--rotate;
			renderer.UpdateTextureMatrix(rotate);
		}

		if(Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT) ) {
			++rotate;
			renderer.UpdateTextureMatrix(rotate);
		}

		if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_3) ) {
			renderer.ToggleFiltering();
		}

		if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_4) ) {
			renderer.ToggleRepeating();
		}
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
	}

	return 0;
}