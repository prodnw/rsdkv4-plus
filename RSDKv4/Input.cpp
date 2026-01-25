#include "RetroEngine.hpp"

InputData keyPress[DEFAULT_INPUT_COUNT + 1];
InputData keyDown[DEFAULT_INPUT_COUNT + 1];
int controllerIDMap[DEFAULT_INPUT_COUNT];

int touchDown[8];
int touchX[8];
int touchY[8];
int touchID[8];
float touchXF[8];
float touchYF[8];
int touches = 0;

int hapticEffectNum = -2;

#if !RETRO_USE_ORIGINAL_CODE
#include <algorithm>
#include <vector>

InputButton inputDevice[DEFAULT_INPUT_COUNT][INPUT_BUTTONCOUNT];
int inputType[DEFAULT_INPUT_COUNT];

// mania deadzone vals lol
float LSTICK_DEADZONE[DEFAULT_INPUT_COUNT];
float RSTICK_DEADZONE[DEFAULT_INPUT_COUNT];
float LTRIGGER_DEADZONE[DEFAULT_INPUT_COUNT];
float RTRIGGER_DEADZONE[DEFAULT_INPUT_COUNT];

int mouseHideTimer = 0;
int lastMouseX     = 0;
int lastMouseY     = 0;

int gamepadCount = 0;

struct InputDevice {
#if RETRO_USING_SDL2
    // we need the controller index reported from SDL2's controller added event
    int index;
    SDL_GameController *devicePtr;
    SDL_Haptic *hapticPtr;
#endif
#if RETRO_USING_SDL1
    SDL_Joystick *devicePtr;
#endif
    int id;
};

std::vector<InputDevice> controllers;

#if RETRO_USING_SDL1
byte keyState[SDLK_LAST];
#endif

#define normalize(val, minVal, maxVal) ((float)(val) - (float)(minVal)) / ((float)(maxVal) - (float)(minVal))

#if RETRO_USING_SDL2
bool getControllerButton(byte buttonID, int deviceID)
{
    bool pressed = false;

    if (deviceID < gamepadCount && deviceID < DEFAULT_INPUT_COUNT) {
        SDL_GameController *controller = controllers[deviceID].devicePtr;

        if (SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)buttonID)) {
            pressed |= true;
            return pressed;
        }
        else {
            switch (buttonID) {
                default: break;
                case SDL_CONTROLLER_BUTTON_DPAD_UP: {
                    int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
                    float delta;
                    if (axis < 0)
                        delta = -normalize(-axis, 1, 32768);
                    else
                        delta = normalize(axis, 0, 32767);
                    pressed |= delta < -LSTICK_DEADZONE[deviceID];
                    break;
                }
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN: {
                    int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
                    float delta;
                    if (axis < 0)
                        delta = -normalize(-axis, 1, 32768);
                    else
                        delta = normalize(axis, 0, 32767);
                    pressed |= delta > LSTICK_DEADZONE[deviceID];
                    break;
                }
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT: {
                    int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
                    float delta;
                    if (axis < 0)
                        delta = -normalize(-axis, 1, 32768);
                    else
                        delta = normalize(axis, 0, 32767);
                    pressed |= delta < -LSTICK_DEADZONE[deviceID];
                    break;
                }
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: {
                    int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
                    float delta;
                    if (axis < 0)
                        delta = -normalize(-axis, 1, 32768);
                    else
                        delta = normalize(axis, 0, 32767);
                    pressed |= delta > LSTICK_DEADZONE[deviceID];
                    break;
                }
            }
        }

        switch (buttonID) {
            default: break;
            case SDL_CONTROLLER_BUTTON_ZL: {
            	float delta;
			    delta = normalize(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT), 0, 32767);
                pressed |= delta > LTRIGGER_DEADZONE[deviceID];
                break;
            }
            case SDL_CONTROLLER_BUTTON_ZR: {
            	float delta;
			    delta = normalize(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT), 0, 32767);
                pressed |= delta > RTRIGGER_DEADZONE[deviceID];
                break;
            }
            case SDL_CONTROLLER_BUTTON_LSTICK_UP: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
                float delta;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta < -LSTICK_DEADZONE[deviceID];
                break;
            }
            case SDL_CONTROLLER_BUTTON_LSTICK_DOWN: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
                float delta;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta > LSTICK_DEADZONE[deviceID];
                break;
            }
            case SDL_CONTROLLER_BUTTON_LSTICK_LEFT: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
                float delta;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta > LSTICK_DEADZONE[deviceID];
                break;
            }
            case SDL_CONTROLLER_BUTTON_LSTICK_RIGHT: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
                float delta;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta < -LSTICK_DEADZONE[deviceID];
                break;
            }
            case SDL_CONTROLLER_BUTTON_RSTICK_UP: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);
                float delta;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta < -RSTICK_DEADZONE[deviceID];
                break;
            }
            case SDL_CONTROLLER_BUTTON_RSTICK_DOWN: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);
                float delta;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta > RSTICK_DEADZONE[deviceID];
                break;
            }
            case SDL_CONTROLLER_BUTTON_RSTICK_LEFT: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
                float delta;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta > RSTICK_DEADZONE[deviceID];
                break;
            }
            case SDL_CONTROLLER_BUTTON_RSTICK_RIGHT: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
                float delta;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta < -RSTICK_DEADZONE[deviceID];
                break;
            }
        }
    }

    return pressed;
}
#endif //! RETRO_USING_SDL2

void controllerInit(int controllerID)
{
    for (int i = 0; i < controllers.size(); ++i) {
        if (controllers[i].id == controllerID) {
            return; // we already opened this one!
        }
    }

#if RETRO_USING_SDL2
    SDL_GameController *controller = SDL_GameControllerOpen(controllerID);
    if (controller) {
        InputDevice device;
        device.id        = controllerID;
        device.index     = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
        device.devicePtr = controller;
        device.hapticPtr = SDL_HapticOpenFromJoystick(SDL_GameControllerGetJoystick(controller));
        if (device.hapticPtr == NULL) {
            PrintLog("Could not open controller haptics...\nSDL_GetError() -> %s", SDL_GetError());
        }
        else {
            if (SDL_HapticRumbleInit(device.hapticPtr) < 0) {
                printf("Unable to initialize rumble!\nSDL_GetError() -> %s", SDL_GetError());
            }
        }

        controllers.push_back(device);
        inputType[controllerID] = 1;
    }
    else {
        PrintLog("Could not open controller...\nSDL_GetError() -> %s", SDL_GetError());
    }
#endif
}

void controllerClose(int controllerID)
{
#if RETRO_USING_SDL2
    SDL_GameController *controller = SDL_GameControllerFromInstanceID(controllerID);
    if (controller) {
        SDL_GameControllerClose(controller);
#endif
        if (controllers.size() == 1) {
            controllers.clear();
        } else {
            for (int i = 0; i < controllers.size(); ++i) {
                if (controllers[i].index == controllerID) {
                    controllers.erase(controllers.begin() + i);
#if RETRO_USING_SDL2
                    if (controllers[i].hapticPtr) {
                        SDL_HapticClose(controllers[i].hapticPtr);
                    }
#endif
                    break;
                }
            }
        }
#if RETRO_USING_SDL2
    }
#endif

    if (controllers.empty())
        inputType[controllerID] = 0;
}

void InitInputDevices()
{
	for (int i = 0; i < DEFAULT_INPUT_COUNT; i++) { keyPress[i] = InputData(); }
	for (int i = 0; i < DEFAULT_INPUT_COUNT; i++) { keyDown[i]  = InputData(); }
#if RETRO_USING_SDL2
    PrintLog("Initializing gamepads...");

    // fix for issue #334 on github, not sure what's going wrong, but it seems to not be initializing the gamepad api maybe?
    SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);

    int joyStickCount = SDL_NumJoysticks();
    controllers.clear();
    gamepadCount = 0;

    // Count how many controllers there are
    for (int i = 0; i < joyStickCount; i++)
        if (SDL_IsGameController(i))
            gamepadCount++;

    PrintLog("Found %d gamepads!", gamepadCount);
    for (int i = 0; i < gamepadCount; i++) {
        SDL_GameController *gamepad = SDL_GameControllerOpen(i);
        InputDevice device;
        device.id        = 0;
        device.index     = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(gamepad));
        device.devicePtr = gamepad;
        PrintLog("Gamepad %d: %s", i, SDL_GameControllerName(gamepad));

        if (SDL_GameControllerGetAttached(gamepad))
            controllers.push_back(device);
        else
            PrintLog("InitInputDevices() error -> %s", SDL_GetError());
    }

    if (gamepadCount > 0)
        SDL_GameControllerEventState(SDL_ENABLE);
#endif
}

void ReleaseInputDevices()
{
    for (int i = 0; i < controllers.size(); i++) {
#if RETRO_USING_SDL2
        if (controllers[i].devicePtr)
            SDL_GameControllerClose(controllers[i].devicePtr);
        if (controllers[i].hapticPtr)
            SDL_HapticClose(controllers[i].hapticPtr);
#endif
    }
    controllers.clear();
}

void ProcessInput(int deviceID)
{
#if RETRO_USING_SDL2
    int length           = 0;
    const byte *keyState = SDL_GetKeyboardState(&length);

    if (inputType[deviceID] == 0) {
        for (int i = 0; i < INPUT_ANY; i++) {
            if (keyState[inputDevice[deviceID][i].keyMappings]) {
                inputDevice[deviceID][i].setHeld();
                if (!inputDevice[deviceID][INPUT_ANY].hold)
                    inputDevice[deviceID][INPUT_ANY].setHeld();
            }
            else if (inputDevice[deviceID][i].hold)
                inputDevice[deviceID][i].setReleased();
        }
    }
    else if (inputType[deviceID] == 1) {
        for (int i = 0; i < INPUT_ANY; i++) {
            if (getControllerButton(inputDevice[deviceID][i].contMappings, deviceID)) {
                inputDevice[deviceID][i].setHeld();
                if (!inputDevice[deviceID][INPUT_ANY].hold)
                    inputDevice[deviceID][INPUT_ANY].setHeld();
            }
            else if (inputDevice[deviceID][i].hold)
                inputDevice[deviceID][i].setReleased();
        }
    }

    bool isPressed = false;
    for (int i = 0; i < INPUT_BUTTONCOUNT; i++) {
        if (keyState[inputDevice[deviceID][i].keyMappings]) {
            isPressed = true;
            break;
        }
    }
    if (isPressed)
        inputType[deviceID] = 0;
    else if (inputType[deviceID] == 0)
        inputDevice[deviceID][INPUT_ANY].setReleased();

    isPressed = false;
    for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
        if (getControllerButton(i, deviceID)) {
            isPressed = true;
            break;
        }
    }
    if (isPressed)
        inputType[deviceID] = 1;
    else if (inputType[deviceID] == 1)
        inputDevice[deviceID][INPUT_ANY].setReleased();

    if (inputDevice[deviceID][INPUT_ANY].press || inputDevice[deviceID][INPUT_ANY].hold || touches > 1) {
        Engine.dimTimer = 0;
    }
    else if (Engine.dimTimer < Engine.dimLimit && !Engine.masterPaused) {
        ++Engine.dimTimer;
    }

#ifdef RETRO_USING_MOUSE
    if (touches <= 0) { // Touch always takes priority over mouse
        int mx = 0, my = 0;
        SDL_GetMouseState(&mx, &my);

        if (mx == lastMouseX && my == lastMouseY) {
            ++mouseHideTimer;
            if (mouseHideTimer == 120) {
                SDL_ShowCursor(false);
            }
        }
        else {
            if (mouseHideTimer >= 120)
                SDL_ShowCursor(true);
            mouseHideTimer  = 0;
            Engine.dimTimer = 0;
        }

        lastMouseX = mx;
        lastMouseY = my;
    }
#endif //! RETRO_USING_MOUSE

#elif RETRO_USING_SDL1
    if (SDL_NumJoysticks() > 0) {
        controller = SDL_JoystickOpen(0);

        // There's a problem opening the joystick
        if (controller == NULL) {
            // Uh oh
        }
        else {
            inputType[deviceID] = 1;
        }
    }
    else {
        if (controller) {
            // Close the joystick
            SDL_JoystickClose(controller);
        }
        controller = nullptr;
        inputType[deviceID]  = 0;
    }

    if (inputType[deviceID] == 0) {
        for (int i = 0; i < INPUT_BUTTONCOUNT; i++) {
            if (keyState[inputDevice[deviceID][i].keyMappings]) {
                inputDevice[deviceID][i].setHeld();
                inputDevice[deviceID][INPUT_ANY].setHeld();
                continue;
            }
            else if (inputDevice[deviceID][i].hold)
                inputDevice[deviceID][i].setReleased();
        }
    }
    else if (inputType[deviceID] == 1 && controller) {
        for (int i = 0; i < INPUT_BUTTONCOUNT; i++) {
            if (SDL_JoystickGetButton(controller, inputDevice[deviceID][i].contMappings)) {
                inputDevice[deviceID][i].setHeld();
                inputDevice[deviceID][INPUT_ANY].setHeld();
                continue;
            }
            else if (inputDevice[deviceID][i].hold)
                inputDevice[deviceID][i].setReleased();
        }
    }

    bool isPressed = false;
    for (int i = 0; i < INPUT_BUTTONCOUNT; i++) {
        if (keyState[inputDevice[deviceID][i].keyMappings]) {
            isPressed = true;
            break;
        }
    }
    if (isPressed)
        inputType[deviceID] = 0;
    else if (inputType[deviceID] == 0)
        inputDevice[deviceID][INPUT_ANY].setReleased();

    int buttonCnt = 0;
    if (controller)
        buttonCnt = SDL_JoystickNumButtons(controller);
    bool flag = false;
    for (int i = 0; i < buttonCnt; ++i) {
        flag      = true;
        inputType[deviceID] = 1;
    }
    if (!flag && inputType[deviceID] == 1) {
        inputDevice[deviceID][INPUT_ANY].setReleased();
    }
#endif //! RETRO_USING_SDL2
}
#endif //! !RETRO_USE_ORIGINAL_CODE

// Pretty much is this code in the original, just formatted differently
void CheckKeyPress(InputData input[])
{
	for (int i = 0; i < DEFAULT_INPUT_COUNT; i++) {
		input[i].up		= false;
		input[i].down	= false;
		input[i].left	= false;
		input[i].right	= false;
		input[i].A		= false;
		input[i].B		= false;
		input[i].C		= false;
		input[i].X		= false;
		input[i].Y		= false;
		input[i].Z		= false;
		input[i].L		= false;
		input[i].R		= false;
		input[i].start	= false;
		input[i].select	= false;
	}
	
	input[DEFAULT_INPUT_COUNT].up		= false;
	input[DEFAULT_INPUT_COUNT].down		= false;
	input[DEFAULT_INPUT_COUNT].left		= false;
	input[DEFAULT_INPUT_COUNT].right	= false;
	input[DEFAULT_INPUT_COUNT].A		= false;
	input[DEFAULT_INPUT_COUNT].B		= false;
	input[DEFAULT_INPUT_COUNT].C		= false;
	input[DEFAULT_INPUT_COUNT].X		= false;
	input[DEFAULT_INPUT_COUNT].Y		= false;
	input[DEFAULT_INPUT_COUNT].Z		= false;
	input[DEFAULT_INPUT_COUNT].L		= false;
	input[DEFAULT_INPUT_COUNT].R		= false;
	input[DEFAULT_INPUT_COUNT].start	= false;
	input[DEFAULT_INPUT_COUNT].select	= false;
	
	int inputMap = 0;
	for (int i = 0; i < DEFAULT_INPUT_COUNT; i++) {
		inputMap = i;
		if (inputType[i] == 1)
			inputMap = controllerIDMap[i];
		
		input[inputMap].up		|= inputDevice[i][INPUT_UP].press;
		input[inputMap].down	|= inputDevice[i][INPUT_DOWN].press;
		input[inputMap].left	|= inputDevice[i][INPUT_LEFT].press;
		input[inputMap].right	|= inputDevice[i][INPUT_RIGHT].press;
		input[inputMap].A		|= inputDevice[i][INPUT_BUTTONA].press;
		input[inputMap].B		|= inputDevice[i][INPUT_BUTTONB].press;
		input[inputMap].C		|= inputDevice[i][INPUT_BUTTONC].press;
		input[inputMap].X		|= inputDevice[i][INPUT_BUTTONX].press;
		input[inputMap].Y		|= inputDevice[i][INPUT_BUTTONY].press;
		input[inputMap].Z		|= inputDevice[i][INPUT_BUTTONZ].press;
		input[inputMap].L		|= inputDevice[i][INPUT_BUTTONL].press;
		input[inputMap].R		|= inputDevice[i][INPUT_BUTTONR].press;
		input[inputMap].start	|= inputDevice[i][INPUT_START].press;
		input[inputMap].select	|= inputDevice[i][INPUT_SELECT].press;
		
		input[DEFAULT_INPUT_COUNT].up		|= inputDevice[i][INPUT_UP].press;
		input[DEFAULT_INPUT_COUNT].down		|= inputDevice[i][INPUT_DOWN].press;
		input[DEFAULT_INPUT_COUNT].left		|= inputDevice[i][INPUT_LEFT].press;
		input[DEFAULT_INPUT_COUNT].right	|= inputDevice[i][INPUT_RIGHT].press;
		input[DEFAULT_INPUT_COUNT].A		|= inputDevice[i][INPUT_BUTTONA].press;
		input[DEFAULT_INPUT_COUNT].B		|= inputDevice[i][INPUT_BUTTONB].press;
		input[DEFAULT_INPUT_COUNT].C		|= inputDevice[i][INPUT_BUTTONC].press;
		input[DEFAULT_INPUT_COUNT].X		|= inputDevice[i][INPUT_BUTTONX].press;
		input[DEFAULT_INPUT_COUNT].Y		|= inputDevice[i][INPUT_BUTTONY].press;
		input[DEFAULT_INPUT_COUNT].Z		|= inputDevice[i][INPUT_BUTTONZ].press;
		input[DEFAULT_INPUT_COUNT].L		|= inputDevice[i][INPUT_BUTTONL].press;
		input[DEFAULT_INPUT_COUNT].R		|= inputDevice[i][INPUT_BUTTONR].press;
		input[DEFAULT_INPUT_COUNT].start	|= inputDevice[i][INPUT_START].press;
		input[DEFAULT_INPUT_COUNT].select	|= inputDevice[i][INPUT_SELECT].press;
	}

#if RETRO_REV03
	SetGlobalVariableByName("input.pressButton", input[0].A || input[0].B || input[0].C || input[0].X || input[0].Y || input[0].Z || input[0].L
													|| input[0].R || input[0].start || input[0].select);
#endif
}

void CheckKeyDown(InputData input[])
{
	for (int i = 0; i < DEFAULT_INPUT_COUNT; i++) {
		input[i].up		= false;
		input[i].down	= false;
		input[i].left	= false;
		input[i].right	= false;
		input[i].A		= false;
		input[i].B		= false;
		input[i].C		= false;
		input[i].X		= false;
		input[i].Y		= false;
		input[i].Z		= false;
		input[i].L		= false;
		input[i].R		= false;
		input[i].start	= false;
		input[i].select	= false;
	}
	
	input[DEFAULT_INPUT_COUNT].up		= false;
	input[DEFAULT_INPUT_COUNT].down		= false;
	input[DEFAULT_INPUT_COUNT].left		= false;
	input[DEFAULT_INPUT_COUNT].right	= false;
	input[DEFAULT_INPUT_COUNT].A		= false;
	input[DEFAULT_INPUT_COUNT].B		= false;
	input[DEFAULT_INPUT_COUNT].C		= false;
	input[DEFAULT_INPUT_COUNT].X		= false;
	input[DEFAULT_INPUT_COUNT].Y		= false;
	input[DEFAULT_INPUT_COUNT].Z		= false;
	input[DEFAULT_INPUT_COUNT].L		= false;
	input[DEFAULT_INPUT_COUNT].R		= false;
	input[DEFAULT_INPUT_COUNT].start	= false;
	input[DEFAULT_INPUT_COUNT].select	= false;
	
	int inputMap = 0;
	for (int i = 0; i < DEFAULT_INPUT_COUNT; i++) {
		inputMap = i;
		if (inputType[i] == 1)
			inputMap = controllerIDMap[i];
		
		input[inputMap].up		|= inputDevice[i][INPUT_UP].hold;
		input[inputMap].down	|= inputDevice[i][INPUT_DOWN].hold;
		input[inputMap].left	|= inputDevice[i][INPUT_LEFT].hold;
		input[inputMap].right	|= inputDevice[i][INPUT_RIGHT].hold;
		input[inputMap].A		|= inputDevice[i][INPUT_BUTTONA].hold;
		input[inputMap].B		|= inputDevice[i][INPUT_BUTTONB].hold;
		input[inputMap].C		|= inputDevice[i][INPUT_BUTTONC].hold;
		input[inputMap].X		|= inputDevice[i][INPUT_BUTTONX].hold;
		input[inputMap].Y		|= inputDevice[i][INPUT_BUTTONY].hold;
		input[inputMap].Z		|= inputDevice[i][INPUT_BUTTONZ].hold;
		input[inputMap].L		|= inputDevice[i][INPUT_BUTTONL].hold;
		input[inputMap].R		|= inputDevice[i][INPUT_BUTTONR].hold;
		input[inputMap].start	|= inputDevice[i][INPUT_START].hold;
		input[inputMap].select	|= inputDevice[i][INPUT_SELECT].hold;
		
		input[DEFAULT_INPUT_COUNT].up		|= inputDevice[i][INPUT_UP].hold;
		input[DEFAULT_INPUT_COUNT].down		|= inputDevice[i][INPUT_DOWN].hold;
		input[DEFAULT_INPUT_COUNT].left		|= inputDevice[i][INPUT_LEFT].hold;
		input[DEFAULT_INPUT_COUNT].right	|= inputDevice[i][INPUT_RIGHT].hold;
		input[DEFAULT_INPUT_COUNT].A		|= inputDevice[i][INPUT_BUTTONA].hold;
		input[DEFAULT_INPUT_COUNT].B		|= inputDevice[i][INPUT_BUTTONB].hold;
		input[DEFAULT_INPUT_COUNT].C		|= inputDevice[i][INPUT_BUTTONC].hold;
		input[DEFAULT_INPUT_COUNT].X		|= inputDevice[i][INPUT_BUTTONX].hold;
		input[DEFAULT_INPUT_COUNT].Y		|= inputDevice[i][INPUT_BUTTONY].hold;
		input[DEFAULT_INPUT_COUNT].Z		|= inputDevice[i][INPUT_BUTTONZ].hold;
		input[DEFAULT_INPUT_COUNT].L		|= inputDevice[i][INPUT_BUTTONL].hold;
		input[DEFAULT_INPUT_COUNT].R		|= inputDevice[i][INPUT_BUTTONR].hold;
		input[DEFAULT_INPUT_COUNT].start	|= inputDevice[i][INPUT_START].hold;
		input[DEFAULT_INPUT_COUNT].select	|= inputDevice[i][INPUT_SELECT].hold;
	}
}

int CheckTouchRect(float x, float y, float w, float h)
{
    for (int f = 0; f < touches; ++f) {
        if (touchDown[f] && touchXF[f] > (x - w) && touchYF[f] > (y - h) && touchXF[f] <= (x + w) && touchYF[f] <= (y + h)) {
            return f;
        }
    }
    return -1;
}

int CheckTouchRectMatrix(void *m, float x, float y, float w, float h)
{
    MatrixF *mat = (MatrixF *)m;
    for (int f = 0; f < touches; ++f) {
        float tx = touchXF[f];
        float ty = touchYF[f];
        if (touchDown[f]) {
            float posX = (((tx * mat->values[0][0]) + (ty * mat->values[1][0])) + (mat->values[2][0] * SCREEN_YSIZE)) + mat->values[3][0];
            if (posX > (x - w) && posX <= (x + w)) {
                float posY = (((tx * mat->values[0][1]) + (ty * mat->values[1][1])) + (mat->values[2][1] * SCREEN_YSIZE)) + mat->values[3][1];
                if (posY > (y - h) && posY <= (y + h))
                    return f;
            }
        }
    }
    return -1;
}

#if RETRO_USE_HAPTICS
void HapticEffect(int *hapticID, int *a2, int *a3, int *a4)
{
    if (Engine.hapticsEnabled)
        hapticEffectNum = *hapticID;
}
#endif

// Set the LED color on a specific controller that supports it
void SetControllerLEDColour(int controllerID, Uint8 r, Uint8 g, Uint8 b)
{
#if RETRO_USING_SDL2
    if (SDL_IsGameController(controllerID)) {
        SDL_GameController *controller = SDL_GameControllerOpen(controllerID);
        if (controller) {
            if (SDL_GameControllerSetLED(controller, r, g, b) == 0) {
                const char* name = SDL_GameControllerName(controller);
                PrintLog("Set LED color for controller %d (%s) to (%d, %d, %d)", controllerID, name ? name : "Unknown", r, g, b);
            } else {
                PrintLog("Controller %d does not support RGB LED", controllerID);
            }
            SDL_GameControllerClose(controller);
        } else {
            PrintLog("Failed to open controller %d", controllerID);
        }
    }
#endif
}

int GetGamepadBatteryLevel()
{
#if RETRO_USING_SDL2
    // Load battery level from the first connected gamepad
    if (gamepadCount > 0 && controllers.size() > 0) {
        SDL_GameController *controller = controllers[0].devicePtr;
        if (controller) {
            SDL_Joystick *joystick = SDL_GameControllerGetJoystick(controller);
            if (joystick) {
                SDL_JoystickPowerLevel powerLevel = SDL_JoystickCurrentPowerLevel(joystick);
                
                // Hey Vegeta, what does that scouter say about his power level?
                switch (powerLevel) {
                    case SDL_JOYSTICK_POWER_UNKNOWN: return 100;
                    case SDL_JOYSTICK_POWER_EMPTY: return 0;
                    case SDL_JOYSTICK_POWER_LOW: return 25;
                    case SDL_JOYSTICK_POWER_MEDIUM: return 50;
                    case SDL_JOYSTICK_POWER_FULL: return 100;
                    case SDL_JOYSTICK_POWER_WIRED: return 100;
                    default: return 100;
                }
            }
        }
    }
#endif
    return 100; // Default to full
}