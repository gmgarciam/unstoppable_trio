
//#pragma once
/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2018 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include "SDL_internal.h"

/* This is the game controller API for Simple DirectMedia Layer */

#include "/home/estebagel/UE_4.20-unstoppable/Engine/Source/ThirdParty/SDL2/SDL-gui-backend/include/SDL_events.h"
#include "/home/estebagel/UE_4.20-unstoppable/Engine/Source/ThirdParty/SDL2/SDL-gui-backend/include/SDL_assert.h"
#include "ICHU_SDL_hints.h"
#include "/home/estebagel/UE_4.20-unstoppable/Engine/Source/ThirdParty/SDL2/SDL-gui-backend/include/SDL_timer.h"
#include "SDL_sysjoystick.h"
#include "SDL_joystick_c.h"
//Don't need header below because I included function into this file.
//#include "/home/estebagel/UE_4.20-unstoppable/Engine/Source/ThirdParty/SDL2/SDL-gui-backend/src/joystick/SDL_gamecontrollerdb.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wint-conversion"
#pragma clang diagnostic ignored "-Wint-to-pointer-cast"
#pragma clang diagnostic ignored "-Wimplicit-function-declaration"

//#undef SDL_EVENTS_DISABLED
//#if !SDL_EVENTS_DISABLED
#include "/home/estebagel/Documents/Unreal Projects/myICHU/Source/myICHU/Public/SDL_events_c.h"
//#endif

#include "/home/estebagel/UE_4.20-unstoppable/Engine/Source/ThirdParty/SDL2/SDL-gui-backend/src/events/SDL_events_c.h"
#include "../../../../../../UE_4.20-unstoppable/Engine/Source/ThirdParty/SDL2/SDL-gui-backend/include/SDL_gamecontroller.h"
#include "../../../../../../UE_4.20-unstoppable/Engine/Source/ThirdParty/SDL2/SDL-gui-backend/include/SDL_events.h"
#include "../../../../../../UE_4.20-unstoppable/Engine/Extras/ThirdPartyNotUE/SDKs/HostLinux/Linux_x64/v11_clang-5.0.0-centos7/x86_64-unknown-linux-gnu/lib/clang/5.0.0/include/stddef.h"
#include "/home/estebagel/UE_4.20-unstoppable/Engine/Source/ThirdParty/SDL2/SDL-gui-backend/include/SDL_rwops.h"
//#include "../../../../../../UE_4.20-unstoppable/Engine/Source/ThirdParty/SDL2/SDL-gui-backend/src/joystick/SDL_gamecontrollerdb.h"
//#include "../../../../../../UE_4.20-unstoppable/Engine/Source/ThirdParty/SDL2/SDL-gui-backend/src/joystick/SDL_gamecontrollerdb.h"

//#ifdef SDL_JOYSTICK_XINPUT
//#define SDL_JOYSTICK_XINPUT
//include "../../../../../../UE_4.20-unstoppable/Engine/Source/ThirdParty/SDL2/SDL-gui-backend/src/joystick/SDL_gamecontrollerdb.h"
//#endif

//#endif

#if defined(__ANDROID__)
#include "SDL_system.h"
#endif

//I added this part to define the SDL_rwops








/* Many controllers turn the center button into an instantaneous button press */
#define SDL_MINIMUM_GUIDE_BUTTON_DELAY_MS   250

#define SDL_CONTROLLER_PLATFORM_FIELD "platform:"

/* a list of currently opened game controllers */
static SDL_GameController *SDL_gamecontrollers = NULL;












typedef struct
{
    SDL_GameControllerBindType inputType;
    union
    {
        int button;

        struct {
            int axis;
            int axis_min;
            int axis_max;
        } axis;

        struct {
            int hat;
            int hat_mask;
        } hat;

    } input;

    SDL_GameControllerBindType outputType;
    union
    {
        SDL_GameControllerButton button;

        struct {
            SDL_GameControllerAxis axis;
            int axis_min;
            int axis_max;
        } axis;

    } output;

} SDL_ExtendedGameControllerBind;

/* our hard coded list of mapping support */
typedef enum
{
    SDL_CONTROLLER_MAPPING_PRIORITY_DEFAULT,
    SDL_CONTROLLER_MAPPING_PRIORITY_API,
    SDL_CONTROLLER_MAPPING_PRIORITY_USER,
} SDL_ControllerMappingPriority;

typedef struct _ControllerMapping_t
{
    SDL_JoystickGUID guid;
    char *name;
    char *mapping;
    SDL_ControllerMappingPriority priority;
    struct _ControllerMapping_t *next;
} ControllerMapping_t;

static SDL_JoystickGUID s_zeroGUID;
static ControllerMapping_t *s_pSupportedControllers = NULL;
static ControllerMapping_t *s_pDefaultMapping = NULL;
static ControllerMapping_t *s_pHIDAPIMapping = NULL;
static ControllerMapping_t *s_pXInputMapping = NULL;

/* The SDL game controller structure */
struct _SDL_GameController
{
    SDL_Joystick *joystick; /* underlying joystick device */
    int ref_count;

    const char *name;
    int num_bindings;
    SDL_ExtendedGameControllerBind *bindings;
    SDL_ExtendedGameControllerBind **last_match_axis;
    Uint8 *last_hat_mask;
    Uint32 guide_button_down;

    struct _SDL_GameController *next; /* pointer to next game controller we have allocated */
};


typedef struct
{
    int num_entries;
    int max_entries;
    Uint32 *entries;
} SDL_vidpid_list;

static SDL_vidpid_list SDL_allowed_controllers;
static SDL_vidpid_list SDL_ignored_controllers;

static void
SDL_LoadVIDPIDListFromHint(const char *hint, SDL_vidpid_list *list)
{
    Uint32 entry;
    char *spot;
    char *file = NULL;

    list->num_entries = 0;

    if (*hint == '@') {
        if (hint != NULL) {
            spot = file = (char *) SDL_LoadFile(hint + 1, NULL);
        } else {
            spot = (char *) hint;
        }
    } else {
        spot = (char *) hint;
    }

    if (!spot) {
        return;
    }

    while ((spot = SDL_strstr(spot, "0x")) != NULL) {
        entry = (Uint16)SDL_strtol(spot, &spot, 0);
        entry <<= 16;
        spot = SDL_strstr(spot, "0x");
        if (!spot) {
            break;
        }
        entry |= (Uint16)SDL_strtol(spot, &spot, 0);

        if (list->num_entries == list->max_entries) {
            int max_entries = list->max_entries + 16;
            Uint32 *entries = (Uint32 *)SDL_realloc(list->entries, max_entries*sizeof(*list->entries));
            if (entries == NULL) {
                /* Out of memory, go with what we have already */
                break;
            }
            list->entries = entries;
            list->max_entries = max_entries;
        }
        list->entries[list->num_entries++] = entry;
    }

    if (file) {
        SDL_free(file);
    }
}

static void SDLCALL
SDL_GameControllerIgnoreDevicesChanged(const char *name, const char *oldValue, const char *hint)
{
    SDL_LoadVIDPIDListFromHint(name, &SDL_ignored_controllers);
}

static void SDLCALL
SDL_GameControllerIgnoreDevicesExceptChanged(const char *name, const char *oldValue, const char *hint)
{
    SDL_LoadVIDPIDListFromHint(name, &SDL_allowed_controllers);
}

static int SDL_PrivateGameControllerAxis(SDL_GameController * gamecontroller, SDL_GameControllerAxis axis, Sint16 value);
static int SDL_PrivateGameControllerButton(SDL_GameController * gamecontroller, SDL_GameControllerButton button, Uint8 state);

/*
 * If there is an existing add event in the queue, it needs to be modified
 * to have the right value for which, because the number of controllers in
 * the system is now one less.
 */
static void UpdateEventsForDeviceRemoval()
{
    int i, num_events;
    SDL_Event *events;
    SDL_bool isstack;

    num_events = SDL_PeepEvents(NULL, 0, SDL_PEEKEVENT, SDL_CONTROLLERDEVICEADDED, SDL_CONTROLLERDEVICEADDED);
    if (num_events <= 0) {
        return;
    }

    int SDL_Event = 0;
   // SDL_Event = SDL_Event;
    events = SDL_small_alloc(SDL_Event, num_events, &isstack);
    if (!events) {
        return;
    }

    num_events = SDL_PeepEvents(events, num_events, SDL_GETEVENT, SDL_CONTROLLERDEVICEADDED, SDL_CONTROLLERDEVICEADDED);
    for (i = 0; i < num_events; ++i) {
        --events[i].cdevice.which;
    }
    SDL_PeepEvents(events, num_events, SDL_ADDEVENT, 0, 0);

    SDL_small_free(events, isstack);
}

static SDL_bool HasSameOutput(SDL_ExtendedGameControllerBind *a, SDL_ExtendedGameControllerBind *b)
{
    if (a->outputType != b->outputType) {
        return SDL_FALSE;
    }

    if (a->outputType == SDL_CONTROLLER_BINDTYPE_AXIS) {
        return (a->output.axis.axis == b->output.axis.axis);
    } else {
        return (a->output.button == b->output.button);
    }
}

static void ResetOutput(SDL_GameController *gamecontroller, SDL_ExtendedGameControllerBind *bind)
{
    if (bind->outputType == SDL_CONTROLLER_BINDTYPE_AXIS) {
        SDL_PrivateGameControllerAxis(gamecontroller, bind->output.axis.axis, 0);
    } else {
        SDL_PrivateGameControllerButton(gamecontroller, bind->output.button, SDL_RELEASED);
    }
}

static void HandleJoystickAxis(SDL_GameController *gamecontroller, int axis, int value)
{
    int i;
    SDL_ExtendedGameControllerBind *last_match = gamecontroller->last_match_axis[axis];
    SDL_ExtendedGameControllerBind *match = NULL;

    for (i = 0; i < gamecontroller->num_bindings; ++i) {
        SDL_ExtendedGameControllerBind *binding = &gamecontroller->bindings[i];
        if (binding->inputType == SDL_CONTROLLER_BINDTYPE_AXIS &&
            axis == binding->input.axis.axis) {
            if (binding->input.axis.axis_min < binding->input.axis.axis_max) {
                if (value >= binding->input.axis.axis_min &&
                    value <= binding->input.axis.axis_max) {
                    match = binding;
                    break;
                }
            } else {
                if (value >= binding->input.axis.axis_max &&
                    value <= binding->input.axis.axis_min) {
                    match = binding;
                    break;
                }
            }
        }
    }

    if (last_match && (!match || !HasSameOutput(last_match, match))) {
        /* Clear the last input that this axis generated */
        ResetOutput(gamecontroller, last_match);
    }

    if (match) {
        if (match->outputType == SDL_CONTROLLER_BINDTYPE_AXIS) {
            if (match->input.axis.axis_min != match->output.axis.axis_min || match->input.axis.axis_max != match->output.axis.axis_max) {
                float normalized_value = (float)(value - match->input.axis.axis_min) / (match->input.axis.axis_max - match->input.axis.axis_min);
                value = match->output.axis.axis_min + (int)(normalized_value * (match->output.axis.axis_max - match->output.axis.axis_min));
            }
            SDL_PrivateGameControllerAxis(gamecontroller, match->output.axis.axis, (Sint16)value);
        } else {
            Uint8 state;
            int threshold = match->input.axis.axis_min + (match->input.axis.axis_max - match->input.axis.axis_min) / 2;
            if (match->input.axis.axis_max < match->input.axis.axis_min) {
                state = (value <= threshold) ? SDL_PRESSED : SDL_RELEASED;
            } else {
                state = (value >= threshold) ? SDL_PRESSED : SDL_RELEASED;
            }
            SDL_PrivateGameControllerButton(gamecontroller, match->output.button, state);
        }
    }
    gamecontroller->last_match_axis[axis] = match;
}

static void HandleJoystickButton(SDL_GameController *gamecontroller, int button, Uint8 state)
{
    int i;

    for (i = 0; i < gamecontroller->num_bindings; ++i) {
        SDL_ExtendedGameControllerBind *binding = &gamecontroller->bindings[i];
        if (binding->inputType == SDL_CONTROLLER_BINDTYPE_BUTTON &&
            button == binding->input.button) {
            if (binding->outputType == SDL_CONTROLLER_BINDTYPE_AXIS) {
                int value = state ? binding->output.axis.axis_max : binding->output.axis.axis_min;
                SDL_PrivateGameControllerAxis(gamecontroller, binding->output.axis.axis, (Sint16)value);
            } else {
                SDL_PrivateGameControllerButton(gamecontroller, binding->output.button, state);
            }
            break;
        }
    }
}

static void HandleJoystickHat(SDL_GameController *gamecontroller, int hat, Uint8 value)
{
    int i;
    Uint8 last_mask = gamecontroller->last_hat_mask[hat];
    Uint8 changed_mask = (last_mask ^ value);

    for (i = 0; i < gamecontroller->num_bindings; ++i) {
        SDL_ExtendedGameControllerBind *binding = &gamecontroller->bindings[i];
        if (binding->inputType == SDL_CONTROLLER_BINDTYPE_HAT && hat == binding->input.hat.hat) {
            if ((changed_mask & binding->input.hat.hat_mask) != 0) {
                if (value & binding->input.hat.hat_mask) {
                    if (binding->outputType == SDL_CONTROLLER_BINDTYPE_AXIS) {
                        SDL_PrivateGameControllerAxis(gamecontroller, binding->output.axis.axis, (Sint16)binding->output.axis.axis_max);
                    } else {
                        SDL_PrivateGameControllerButton(gamecontroller, binding->output.button, SDL_PRESSED);
                    }
                } else {
                    ResetOutput(gamecontroller, binding);
                }
            }
        }
    }
    gamecontroller->last_hat_mask[hat] = value;
}

/*
 * Event filter to fire controller events from joystick ones
 */
static int SDLCALL SDL_GameControllerEventWatcher(void *userdata, SDL_Event * event)
{
    switch(event->type) {
        case SDL_JOYAXISMOTION:
        {
            SDL_GameController *controllerlist = SDL_gamecontrollers;
            while (controllerlist) {
                if (controllerlist->joystick->instance_id == event->jaxis.which) {
                    HandleJoystickAxis(controllerlist, event->jaxis.axis, event->jaxis.value);
                    break;
                }
                controllerlist = controllerlist->next;
            }
        }
            break;
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
        {
            SDL_GameController *controllerlist = SDL_gamecontrollers;
            while (controllerlist) {
                if (controllerlist->joystick->instance_id == event->jbutton.which) {
                    HandleJoystickButton(controllerlist, event->jbutton.button, event->jbutton.state);
                    break;
                }
                controllerlist = controllerlist->next;
            }
        }
            break;
        case SDL_JOYHATMOTION:
        {
            SDL_GameController *controllerlist = SDL_gamecontrollers;
            while (controllerlist) {
                if (controllerlist->joystick->instance_id == event->jhat.which) {
                    HandleJoystickHat(controllerlist, event->jhat.hat, event->jhat.value);
                    break;
                }
                controllerlist = controllerlist->next;
            }
        }
            break;
        case SDL_JOYDEVICEADDED:
        {
            if (SDL_IsGameController(event->jdevice.which)) {
                SDL_Event deviceevent;
                deviceevent.type = SDL_CONTROLLERDEVICEADDED;
                deviceevent.cdevice.which = event->jdevice.which;
                SDL_PushEvent(&deviceevent);
            }
        }
            break;
        case SDL_JOYDEVICEREMOVED:
        {
            SDL_GameController *controllerlist = SDL_gamecontrollers;
            while (controllerlist) {
                if (controllerlist->joystick->instance_id == event->jdevice.which) {
                    SDL_Event deviceevent;

                    deviceevent.type = SDL_CONTROLLERDEVICEREMOVED;
                    deviceevent.cdevice.which = event->jdevice.which;
                    SDL_PushEvent(&deviceevent);

                    UpdateEventsForDeviceRemoval();
                    break;
                }
                controllerlist = controllerlist->next;
            }
        }
            break;
        default:
            break;
    }

    return 1;
}

/*
 * Helper function to scan the mappings database for a controller with the specified GUID
 */
static ControllerMapping_t *SDL_PrivateGetControllerMappingForGUID(SDL_JoystickGUID *guid, SDL_bool exact_match)
{
    ControllerMapping_t *pSupportedController = s_pSupportedControllers;
    while (pSupportedController) {
        if (SDL_memcmp(guid, &pSupportedController->guid, sizeof(*guid)) == 0) {
            return pSupportedController;
        }
        pSupportedController = pSupportedController->next;
    }
    if (!exact_match) {
        if (SDL_IsJoystickHIDAPI(*guid)) {
            /* This is a HIDAPI device */
            return s_pHIDAPIMapping;
        }
//#if SDL_JOYSTICK_XINPUT
        if (SDL_IsJoystickXInput(*guid)) {
            /* This is an XInput device */
            return s_pXInputMapping;
        }
//#endif
    }
    return NULL;
}

static const char* map_StringForControllerAxis[] = {
        "leftx",
        "lefty",
        "rightx",
        "righty",
        "lefttrigger",
        "righttrigger",
        NULL
};

/*
 * convert a string to its enum equivalent
 */
SDL_GameControllerAxis SDL_GameControllerGetAxisFromString(const char *pchString)
{
    int entry;

    if (pchString && (*pchString == '+' || *pchString == '-')) {
        ++pchString;
    }

    if (!pchString || !pchString[0]) {
        return SDL_CONTROLLER_AXIS_INVALID;
    }

    for (entry = 0; map_StringForControllerAxis[entry]; ++entry) {
        if (!SDL_strcasecmp(pchString, map_StringForControllerAxis[entry]))
            return (SDL_GameControllerAxis) entry;
    }
    return SDL_CONTROLLER_AXIS_INVALID;
}

/*
 * convert an enum to its string equivalent
 */
const char* SDL_GameControllerGetStringForAxis(SDL_GameControllerAxis axis)
{
    if (axis > SDL_CONTROLLER_AXIS_INVALID && axis < SDL_CONTROLLER_AXIS_MAX) {
        return map_StringForControllerAxis[axis];
    }
    return NULL;
}

static const char* map_StringForControllerButton[] = {
        "a",
        "b",
        "x",
        "y",
        "back",
        "guide",
        "start",
        "leftstick",
        "rightstick",
        "leftshoulder",
        "rightshoulder",
        "dpup",
        "dpdown",
        "dpleft",
        "dpright",
        NULL
};

/*
 * convert a string to its enum equivalent
 */
SDL_GameControllerButton SDL_GameControllerGetButtonFromString(const char *pchString)
{
    int entry;
    if (!pchString || !pchString[0])
        return SDL_CONTROLLER_BUTTON_INVALID;

    for (entry = 0; map_StringForControllerButton[entry]; ++entry) {
        if (SDL_strcasecmp(pchString, map_StringForControllerButton[entry]) == 0)
            return (SDL_GameControllerButton) entry;
    }
    return SDL_CONTROLLER_BUTTON_INVALID;
}

/*
 * convert an enum to its string equivalent
 */
const char* SDL_GameControllerGetStringForButton(SDL_GameControllerButton axis)
{
    if (axis > SDL_CONTROLLER_BUTTON_INVALID && axis < SDL_CONTROLLER_BUTTON_MAX) {
        return map_StringForControllerButton[axis];
    }
    return NULL;
}

/*
 * given a controller button name and a joystick name update our mapping structure with it
 */
static void SDL_PrivateGameControllerParseElement(SDL_GameController *gamecontroller, const char *szGameButton, const char *szJoystickButton)
{
    SDL_ExtendedGameControllerBind bind;
    SDL_GameControllerButton button;
    SDL_GameControllerAxis axis;
    SDL_bool invert_input = SDL_FALSE;
    char half_axis_input = 0;
    char half_axis_output = 0;

    if (*szGameButton == '+' || *szGameButton == '-') {
        half_axis_output = *szGameButton++;
    }

    axis = SDL_GameControllerGetAxisFromString(szGameButton);
    button = SDL_GameControllerGetButtonFromString(szGameButton);
    if (axis != SDL_CONTROLLER_AXIS_INVALID) {
        bind.outputType = SDL_CONTROLLER_BINDTYPE_AXIS;
        bind.output.axis.axis = axis;
        if (axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT || axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
            bind.output.axis.axis_min = 0;
            bind.output.axis.axis_max = SDL_JOYSTICK_AXIS_MAX;
        } else {
            if (half_axis_output == '+') {
                bind.output.axis.axis_min = 0;
                bind.output.axis.axis_max = SDL_JOYSTICK_AXIS_MAX;
            } else if (half_axis_output == '-') {
                bind.output.axis.axis_min = 0;
                bind.output.axis.axis_max = SDL_JOYSTICK_AXIS_MIN;
            } else {
                bind.output.axis.axis_min = SDL_JOYSTICK_AXIS_MIN;
                bind.output.axis.axis_max = SDL_JOYSTICK_AXIS_MAX;
            }
        }
    } else if (button != SDL_CONTROLLER_BUTTON_INVALID) {
        bind.outputType = SDL_CONTROLLER_BINDTYPE_BUTTON;
        bind.output.button = button;
    } else {
        SDL_SetError("Unexpected controller element %s", szGameButton);
        return;
    }

    if (*szJoystickButton == '+' || *szJoystickButton == '-') {
        half_axis_input = *szJoystickButton++;
    }
    if (szJoystickButton[SDL_strlen(szJoystickButton) - 1] == '~') {
        invert_input = SDL_TRUE;
    }

    if (szJoystickButton[0] == 'a' && SDL_isdigit(szJoystickButton[1])) {
        bind.inputType = SDL_CONTROLLER_BINDTYPE_AXIS;
        bind.input.axis.axis = SDL_atoi(&szJoystickButton[1]);
        if (half_axis_input == '+') {
            bind.input.axis.axis_min = 0;
            bind.input.axis.axis_max = SDL_JOYSTICK_AXIS_MAX;
        } else if (half_axis_input == '-') {
            bind.input.axis.axis_min = 0;
            bind.input.axis.axis_max = SDL_JOYSTICK_AXIS_MIN;
        } else {
            bind.input.axis.axis_min = SDL_JOYSTICK_AXIS_MIN;
            bind.input.axis.axis_max = SDL_JOYSTICK_AXIS_MAX;
        }
        if (invert_input) {
            int tmp = bind.input.axis.axis_min;
            bind.input.axis.axis_min = bind.input.axis.axis_max;
            bind.input.axis.axis_max = tmp;
        }
    } else if (szJoystickButton[0] == 'b' && SDL_isdigit(szJoystickButton[1])) {
        bind.inputType = SDL_CONTROLLER_BINDTYPE_BUTTON;
        bind.input.button = SDL_atoi(&szJoystickButton[1]);
    } else if (szJoystickButton[0] == 'h' && SDL_isdigit(szJoystickButton[1]) &&
               szJoystickButton[2] == '.' && SDL_isdigit(szJoystickButton[3])) {
        int hat = SDL_atoi(&szJoystickButton[1]);
        int mask = SDL_atoi(&szJoystickButton[3]);
        bind.inputType = SDL_CONTROLLER_BINDTYPE_HAT;
        bind.input.hat.hat = hat;
        bind.input.hat.hat_mask = mask;
    } else {
        SDL_SetError("Unexpected joystick element: %s", szJoystickButton);
        return;
    }

    ++gamecontroller->num_bindings;
    gamecontroller->bindings = (SDL_ExtendedGameControllerBind *)SDL_realloc(gamecontroller->bindings, gamecontroller->num_bindings * sizeof(*gamecontroller->bindings));
    if (!gamecontroller->bindings) {
        gamecontroller->num_bindings = 0;
        SDL_OutOfMemory();
        return;
    }
    gamecontroller->bindings[gamecontroller->num_bindings - 1] = bind;
}


/*
 * given a controller mapping string update our mapping object
 */
static void
SDL_PrivateGameControllerParseControllerConfigString(SDL_GameController *gamecontroller, const char *pchString)
{
    char szGameButton[20];
    char szJoystickButton[20];
    SDL_bool bGameButton = SDL_TRUE;
    int i = 0;
    const char *pchPos = pchString;

    SDL_zero(szGameButton);
    SDL_zero(szJoystickButton);

    while (pchPos && *pchPos) {
        if (*pchPos == ':') {
            i = 0;
            bGameButton = SDL_FALSE;
        } else if (*pchPos == ' ') {

        } else if (*pchPos == ',') {
            i = 0;
            bGameButton = SDL_TRUE;
            SDL_PrivateGameControllerParseElement(gamecontroller, szGameButton, szJoystickButton);
            SDL_zero(szGameButton);
            SDL_zero(szJoystickButton);

        } else if (bGameButton) {
            if (i >= sizeof(szGameButton)) {
                SDL_SetError("Button name too large: %s", szGameButton);
                return;
            }
            szGameButton[i] = *pchPos;
            i++;
        } else {
            if (i >= sizeof(szJoystickButton)) {
                SDL_SetError("Joystick button name too large: %s", szJoystickButton);
                return;
            }
            szJoystickButton[i] = *pchPos;
            i++;
        }
        pchPos++;
    }

    SDL_PrivateGameControllerParseElement(gamecontroller, szGameButton, szJoystickButton);

}

/*
 * Make a new button mapping struct
 */
static void SDL_PrivateLoadButtonMapping(SDL_GameController *gamecontroller, const char *pchName, const char *pchMapping)
{
    int i;

    gamecontroller->name = pchName;
    gamecontroller->num_bindings = 0;
    SDL_memset(gamecontroller->last_match_axis, 0, gamecontroller->joystick->naxes * sizeof(*gamecontroller->last_match_axis));

    SDL_PrivateGameControllerParseControllerConfigString(gamecontroller, pchMapping);

    /* Set the zero point for triggers */
    for (i = 0; i < gamecontroller->num_bindings; ++i) {
        SDL_ExtendedGameControllerBind *binding = &gamecontroller->bindings[i];
        if (binding->inputType == SDL_CONTROLLER_BINDTYPE_AXIS &&
            binding->outputType == SDL_CONTROLLER_BINDTYPE_AXIS &&
            (binding->output.axis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT ||
             binding->output.axis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)) {
            if (binding->input.axis.axis < gamecontroller->joystick->naxes) {
                gamecontroller->joystick->axes[binding->input.axis.axis].value =
                gamecontroller->joystick->axes[binding->input.axis.axis].zero = (Sint16)binding->input.axis.axis_min;
            }
        }
    }
}


/*
 * grab the guid string from a mapping string
 */
static char *SDL_PrivateGetControllerGUIDFromMappingString(const char *pMapping)
{
    const char *pFirstComma = SDL_strchr(pMapping, ',');
    if (pFirstComma) {
        char *pchGUID = SDL_malloc(pFirstComma - pMapping + 1);
        if (!pchGUID) {
            SDL_OutOfMemory();
            return NULL;
        }
        SDL_memcpy(pchGUID, pMapping, pFirstComma - pMapping);
        pchGUID[pFirstComma - pMapping] = '\0';

        /* Convert old style GUIDs to the new style in 2.0.5 */
/*#if __WIN32__
        if (SDL_strlen(pchGUID) == 32 &&
            SDL_memcmp(&pchGUID[20], "504944564944", 12) == 0) {
            SDL_memcpy(&pchGUID[20], "000000000000", 12);
            SDL_memcpy(&pchGUID[16], &pchGUID[4], 4);
            SDL_memcpy(&pchGUID[8], &pchGUID[0], 4);
            SDL_memcpy(&pchGUID[0], "03000000", 8);
        }
#elif __MACOSX__
        if (SDL_strlen(pchGUID) == 32 &&
            SDL_memcmp(&pchGUID[4], "000000000000", 12) == 0 &&
            SDL_memcmp(&pchGUID[20], "000000000000", 12) == 0) {
            SDL_memcpy(&pchGUID[20], "000000000000", 12);
            SDL_memcpy(&pchGUID[8], &pchGUID[0], 4);
            SDL_memcpy(&pchGUID[0], "03000000", 8);
        }
#endif */
        return pchGUID;
    }
    return NULL;
}


/*
 * grab the name string from a mapping string
 */
static char *SDL_PrivateGetControllerNameFromMappingString(const char *pMapping)
{
    const char *pFirstComma, *pSecondComma;
    char *pchName;

    pFirstComma = SDL_strchr(pMapping, ',');
    if (!pFirstComma)
        return NULL;

    pSecondComma = SDL_strchr(pFirstComma + 1, ',');
    if (!pSecondComma)
        return NULL;

    pchName = SDL_malloc(pSecondComma - pFirstComma);
    if (!pchName) {
        SDL_OutOfMemory();
        return NULL;
    }
    SDL_memcpy(pchName, pFirstComma + 1, pSecondComma - pFirstComma);
    pchName[pSecondComma - pFirstComma - 1] = 0;
    return pchName;
}


/*
 * grab the button mapping string from a mapping string
 */
static char *SDL_PrivateGetControllerMappingFromMappingString(const char *pMapping)
{
    const char *pFirstComma, *pSecondComma;

    pFirstComma = SDL_strchr(pMapping, ',');
    if (!pFirstComma)
        return NULL;

    pSecondComma = SDL_strchr(pFirstComma + 1, ',');
    if (!pSecondComma)
        return NULL;

    return SDL_strdup(pSecondComma + 1); /* mapping is everything after the 3rd comma */
}

/*
 * Helper function to refresh a mapping
 */
static void SDL_PrivateGameControllerRefreshMapping(ControllerMapping_t *pControllerMapping)
{
    SDL_GameController *gamecontrollerlist = SDL_gamecontrollers;
    while (gamecontrollerlist) {
        if (!SDL_memcmp(&gamecontrollerlist->joystick->guid, &pControllerMapping->guid, sizeof(pControllerMapping->guid))) {
            /* Not really threadsafe.  Should this lock access within SDL_GameControllerEventWatcher? */
            SDL_PrivateLoadButtonMapping(gamecontrollerlist, pControllerMapping->name, pControllerMapping->mapping);

            {
                SDL_Event event;
                event.type = SDL_CONTROLLERDEVICEREMAPPED;
                event.cdevice.which = gamecontrollerlist->joystick->instance_id;
                SDL_PushEvent(&event);
            }
        }

        gamecontrollerlist = gamecontrollerlist->next;
    }
}

/*
 * Helper function to add a mapping for a guid
 */
static ControllerMapping_t *
SDL_PrivateAddMappingForGUID(SDL_JoystickGUID jGUID, const char *mappingString, SDL_bool *existing, SDL_ControllerMappingPriority priority)
{
    char *pchName;
    char *pchMapping;
    ControllerMapping_t *pControllerMapping;

    pchName = SDL_PrivateGetControllerNameFromMappingString(mappingString);
    if (!pchName) {
        SDL_SetError("Couldn't parse name from %s", mappingString);
        return NULL;
    }

    pchMapping = SDL_PrivateGetControllerMappingFromMappingString(mappingString);
    if (!pchMapping) {
        SDL_free(pchName);
        SDL_SetError("Couldn't parse %s", mappingString);
        return NULL;
    }

    pControllerMapping = SDL_PrivateGetControllerMappingForGUID(&jGUID, SDL_TRUE);
    if (pControllerMapping) {
        /* Only overwrite the mapping if the priority is the same or higher. */
        if (pControllerMapping->priority <= priority) {
            /* Update existing mapping */
            SDL_free(pControllerMapping->name);
            pControllerMapping->name = pchName;
            SDL_free(pControllerMapping->mapping);
            pControllerMapping->mapping = pchMapping;
            pControllerMapping->priority = priority;
            /* refresh open controllers */
            SDL_PrivateGameControllerRefreshMapping(pControllerMapping);
        } else {
            SDL_free(pchName);
            SDL_free(pchMapping);
        }
        *existing = SDL_TRUE;
    } else {
        pControllerMapping = SDL_malloc(sizeof(*pControllerMapping));
        if (!pControllerMapping) {
            SDL_free(pchName);
            SDL_free(pchMapping);
            SDL_OutOfMemory();
            return NULL;
        }
        pControllerMapping->guid = jGUID;
        pControllerMapping->name = pchName;
        pControllerMapping->mapping = pchMapping;
        pControllerMapping->next = NULL;
        pControllerMapping->priority = priority;

        if (s_pSupportedControllers) {
            /* Add the mapping to the end of the list */
            ControllerMapping_t *pCurrMapping, *pPrevMapping;

            for ( pPrevMapping = s_pSupportedControllers, pCurrMapping = pPrevMapping->next;
                  pCurrMapping;
                  pPrevMapping = pCurrMapping, pCurrMapping = pCurrMapping->next ) {
                continue;
            }
            pPrevMapping->next = pControllerMapping;
        } else {
            s_pSupportedControllers = pControllerMapping;
        }
        *existing = SDL_FALSE;
    }
    return pControllerMapping;
}

#ifdef __ANDROID__
/*
 * Helper function to guess at a mapping based on the elements reported for this controller
 */
static ControllerMapping_t *SDL_CreateMappingForAndroidController(const char *name, SDL_JoystickGUID guid)
{
    SDL_bool existing;
    char name_string[128];
    char mapping_string[1024];
    int button_mask;
    int axis_mask;

    button_mask = SDL_SwapLE16(*(Uint16*)(&guid.data[sizeof(guid.data)-4]));
    axis_mask = SDL_SwapLE16(*(Uint16*)(&guid.data[sizeof(guid.data)-2]));
    if (!button_mask && !axis_mask) {
        /* Accelerometer, shouldn't have a game controller mapping */
        return NULL;
    }

    /* Remove any commas in the name */
    SDL_strlcpy(name_string, name, sizeof(name_string));
    {
        char *spot;
        for (spot = name_string; *spot; ++spot) {
            if (*spot == ',') {
                *spot = ' ';
            }
        }
    }
    SDL_snprintf(mapping_string, sizeof(mapping_string), "none,%s,", name_string);
    if (button_mask & (1 << SDL_CONTROLLER_BUTTON_A)) {
        SDL_strlcat(mapping_string, "a:b0,", sizeof(mapping_string));
    }
    if (button_mask & (1 << SDL_CONTROLLER_BUTTON_B)) {
        SDL_strlcat(mapping_string, "b:b1,", sizeof(mapping_string));
    } else if (button_mask & (1 << SDL_CONTROLLER_BUTTON_BACK)) {
        /* Use the back button as "B" for easy UI navigation with TV remotes */
        SDL_strlcat(mapping_string, "b:b4,", sizeof(mapping_string));
        button_mask &= ~(1 << SDL_CONTROLLER_BUTTON_BACK);
    }
    if (button_mask & (1 << SDL_CONTROLLER_BUTTON_X)) {
        SDL_strlcat(mapping_string, "x:b2,", sizeof(mapping_string));
    }
    if (button_mask & (1 << SDL_CONTROLLER_BUTTON_Y)) {
        SDL_strlcat(mapping_string, "y:b3,", sizeof(mapping_string));
    }
    if (button_mask & (1 << SDL_CONTROLLER_BUTTON_BACK)) {
        SDL_strlcat(mapping_string, "back:b4,", sizeof(mapping_string));
    }
#if 0 /* The guide button generally isn't functional (or acts as a home button) on most Android controllers */
    if (button_mask & (1 << SDL_CONTROLLER_BUTTON_GUIDE)) {
        SDL_strlcat(mapping_string, "guide:b5,", sizeof(mapping_string));
#if 0 /* Actually this will be done in Steam */
    } else if (button_mask & (1 << SDL_CONTROLLER_BUTTON_START)) {
        /* The guide button doesn't exist, use the start button instead,
           so you can do Steam guide button chords and open the Steam overlay.
         */
        SDL_strlcat(mapping_string, "guide:b6,", sizeof(mapping_string));
        button_mask &= ~(1 << SDL_CONTROLLER_BUTTON_START);
#endif
    }
#endif
    if (button_mask & (1 << SDL_CONTROLLER_BUTTON_START)) {
        SDL_strlcat(mapping_string, "start:b6,", sizeof(mapping_string));
    }
    if (button_mask & (1 << SDL_CONTROLLER_BUTTON_LEFTSTICK)) {
        SDL_strlcat(mapping_string, "leftstick:b7,", sizeof(mapping_string));
    }
    if (button_mask & (1 << SDL_CONTROLLER_BUTTON_RIGHTSTICK)) {
        SDL_strlcat(mapping_string, "rightstick:b8,", sizeof(mapping_string));
    }
    if (button_mask & (1 << SDL_CONTROLLER_BUTTON_LEFTSHOULDER)) {
        SDL_strlcat(mapping_string, "leftshoulder:b9,", sizeof(mapping_string));
    }
    if (button_mask & (1 << SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)) {
        SDL_strlcat(mapping_string, "rightshoulder:b10,", sizeof(mapping_string));
    }
    if (button_mask & (1 << SDL_CONTROLLER_BUTTON_DPAD_UP)) {
        SDL_strlcat(mapping_string, "dpup:b11,", sizeof(mapping_string));
    }
    if (button_mask & (1 << SDL_CONTROLLER_BUTTON_DPAD_DOWN)) {
        SDL_strlcat(mapping_string, "dpdown:b12,", sizeof(mapping_string));
    }
    if (button_mask & (1 << SDL_CONTROLLER_BUTTON_DPAD_LEFT)) {
        SDL_strlcat(mapping_string, "dpleft:b13,", sizeof(mapping_string));
    }
    if (button_mask & (1 << SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        SDL_strlcat(mapping_string, "dpright:b14,", sizeof(mapping_string));
    }
    if (axis_mask & (1 << SDL_CONTROLLER_AXIS_LEFTX)) {
        SDL_strlcat(mapping_string, "leftx:a0,", sizeof(mapping_string));
    }
    if (axis_mask & (1 << SDL_CONTROLLER_AXIS_LEFTY)) {
        SDL_strlcat(mapping_string, "lefty:a1,", sizeof(mapping_string));
    }
    if (axis_mask & (1 << SDL_CONTROLLER_AXIS_RIGHTX)) {
        SDL_strlcat(mapping_string, "rightx:a2,", sizeof(mapping_string));
    }
    if (axis_mask & (1 << SDL_CONTROLLER_AXIS_RIGHTY)) {
        SDL_strlcat(mapping_string, "righty:a3,", sizeof(mapping_string));
    }
    if (axis_mask & (1 << SDL_CONTROLLER_AXIS_TRIGGERLEFT)) {
        SDL_strlcat(mapping_string, "lefttrigger:a4,", sizeof(mapping_string));
    }
    if (axis_mask & (1 << SDL_CONTROLLER_AXIS_TRIGGERRIGHT)) {
        SDL_strlcat(mapping_string, "righttrigger:a5,", sizeof(mapping_string));
    }
    return SDL_PrivateAddMappingForGUID(guid, mapping_string,
                      &existing, SDL_CONTROLLER_MAPPING_PRIORITY_DEFAULT);
}
#endif /* __ANDROID__ */


/*
 * Helper function to determine pre-calculated offset to certain joystick mappings
 */
static ControllerMapping_t *SDL_PrivateGetControllerMappingForNameAndGUID(const char *name, SDL_JoystickGUID guid)
{
    ControllerMapping_t *mapping;

    mapping = SDL_PrivateGetControllerMappingForGUID(&guid, SDL_FALSE);
#ifdef __LINUX__
    if (!mapping && name) {
        if (SDL_strstr(name, "Xbox 360 Wireless Receiver")) {
            /* The Linux driver xpad.c maps the wireless dpad to buttons */
            SDL_bool existing;
            mapping = SDL_PrivateAddMappingForGUID(guid,
"none,X360 Wireless Controller,a:b0,b:b1,back:b6,dpdown:b14,dpleft:b11,dpright:b12,dpup:b13,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
                          &existing, SDL_CONTROLLER_MAPPING_PRIORITY_DEFAULT);
        }
    }
#endif /* __LINUX__ */

    if (!mapping && name) {
        if (SDL_strstr(name, "Xbox") || SDL_strstr(name, "X-Box") || SDL_strstr(name, "XBOX")) {
            mapping = s_pXInputMapping;
        }
    }
#ifdef __ANDROID__
    if (!mapping && name && !SDL_IsJoystickHIDAPI(guid)) {
        mapping = SDL_CreateMappingForAndroidController(name, guid);
    }
#endif
    if (!mapping) {
        mapping = s_pDefaultMapping;
    }
    return mapping;
}

static ControllerMapping_t *SDL_PrivateGetControllerMapping(int device_index)
{
    const char *name;
    SDL_JoystickGUID guid;
    ControllerMapping_t *mapping;

    SDL_LockJoysticks();

    if ((device_index < 0) || (device_index >= SDL_NumJoysticks())) {
        SDL_SetError("There are %d joysticks available", SDL_NumJoysticks());
        SDL_UnlockJoysticks();
        return (NULL);
    }

    name = SDL_JoystickNameForIndex(device_index);
    guid = SDL_JoystickGetDeviceGUID(device_index);
    mapping = SDL_PrivateGetControllerMappingForNameAndGUID(name, guid);
    SDL_UnlockJoysticks();
    return mapping;
}

/*
 * Add or update an entry into the Mappings Database
 */
int
SDL_GameControllerAddMappingsFromRW(SDL_RWops * rw, int freerw)
{
    const char *platform = SDL_GetPlatform();
    int controllers = 0;
    char *buf, *line, *line_end, *tmp, *comma, line_platform[64];
    size_t db_size, platform_len;

    if (rw == NULL) {
        return SDL_SetError("Invalid RWops");
    }
    db_size = (size_t)SDL_RWsize(rw);

    buf = (char *)SDL_malloc(db_size + 1);
    if (buf == NULL) {
        if (freerw) {
            SDL_RWclose(rw);
        }
        return SDL_SetError("Could not allocate space to read DB into memory");
    }

    if (SDL_RWread(rw, buf, db_size, 1) != 1) {
        if (freerw) {
            SDL_RWclose(rw);
        }
        SDL_free(buf);
        return SDL_SetError("Could not read DB");
    }

    if (freerw) {
        SDL_RWclose(rw);
    }

    buf[db_size] = '\0';
    line = buf;

    while (line < buf + db_size) {
        line_end = SDL_strchr(line, '\n');
        if (line_end != NULL) {
            *line_end = '\0';
        } else {
            line_end = buf + db_size;
        }

        /* Extract and verify the platform */
        tmp = SDL_strstr(line, SDL_CONTROLLER_PLATFORM_FIELD);
        if (tmp != NULL) {
            tmp += SDL_strlen(SDL_CONTROLLER_PLATFORM_FIELD);
            comma = SDL_strchr(tmp, ',');
            if (comma != NULL) {
                platform_len = comma - tmp + 1;
                if (platform_len + 1 < SDL_arraysize(line_platform)) {
                    SDL_strlcpy(line_platform, tmp, platform_len);
                    if (SDL_strncasecmp(line_platform, platform, platform_len) == 0 &&
                        SDL_GameControllerAddMapping(line) > 0) {
                        controllers++;
                    }
                }
            }
        }

        line = line_end + 1;
    }

    SDL_free(buf);
    return controllers;
}

/*
 * Add or update an entry into the Mappings Database with a priority
 */
static int
SDL_PrivateGameControllerAddMapping(const char *mappingString, SDL_ControllerMappingPriority priority)
{
    char *pchGUID;
    SDL_JoystickGUID jGUID;
    SDL_bool is_default_mapping = SDL_FALSE;
    SDL_bool is_hidapi_mapping = SDL_FALSE;
    SDL_bool is_xinput_mapping = SDL_FALSE;
    SDL_bool existing = SDL_FALSE;
    ControllerMapping_t *pControllerMapping;

    if (!mappingString) {
        return SDL_InvalidParamError("mappingString");
    }

    pchGUID = SDL_PrivateGetControllerGUIDFromMappingString(mappingString);
    if (!pchGUID) {
        return SDL_SetError("Couldn't parse GUID from %s", mappingString);
    }
    if (!SDL_strcasecmp(pchGUID, "default")) {
        is_default_mapping = SDL_TRUE;
    } else if (!SDL_strcasecmp(pchGUID, "hidapi")) {
        is_hidapi_mapping = SDL_TRUE;
    } else if (!SDL_strcasecmp(pchGUID, "xinput")) {
        is_xinput_mapping = SDL_TRUE;
    }
    jGUID = SDL_JoystickGetGUIDFromString(pchGUID);
    SDL_free(pchGUID);

    pControllerMapping = SDL_PrivateAddMappingForGUID(jGUID, mappingString, &existing, priority);
    if (!pControllerMapping) {
        return -1;
    }

    if (existing) {
        return 0;
    } else {
        if (is_default_mapping) {
            s_pDefaultMapping = pControllerMapping;
        } else if (is_hidapi_mapping) {
            s_pHIDAPIMapping = pControllerMapping;
        } else if (is_xinput_mapping) {
            s_pXInputMapping = pControllerMapping;
        }
        return 1;
    }
}

/*
 * Add or update an entry into the Mappings Database
 */
int
SDL_GameControllerAddMapping(const char *mappingString)
{
    return SDL_PrivateGameControllerAddMapping(mappingString, SDL_CONTROLLER_MAPPING_PRIORITY_API);
}

/*
 *  Get the number of mappings installed
 */
int
SDL_GameControllerNumMappings(void)
{
    int num_mappings = 0;
    ControllerMapping_t *mapping;

    for (mapping = s_pSupportedControllers; mapping; mapping = mapping->next) {
        if (SDL_memcmp(&mapping->guid, &s_zeroGUID, sizeof(mapping->guid)) == 0) {
            continue;
        }
        ++num_mappings;
    }
    return num_mappings;
}

/*
 *  Get the mapping at a particular index.
 */
char *
SDL_GameControllerMappingForIndex(int mapping_index)
{
    ControllerMapping_t *mapping;

    for (mapping = s_pSupportedControllers; mapping; mapping = mapping->next) {
        if (SDL_memcmp(&mapping->guid, &s_zeroGUID, sizeof(mapping->guid)) == 0) {
            continue;
        }
        if (mapping_index == 0) {
            char *pMappingString;
            char pchGUID[33];
            size_t needed;

            SDL_JoystickGetGUIDString(mapping->guid, pchGUID, sizeof(pchGUID));
            /* allocate enough memory for GUID + ',' + name + ',' + mapping + \0 */
            needed = SDL_strlen(pchGUID) + 1 + SDL_strlen(mapping->name) + 1 + SDL_strlen(mapping->mapping) + 1;
            pMappingString = SDL_malloc(needed);
            if (!pMappingString) {
                SDL_OutOfMemory();
                return NULL;
            }
            SDL_snprintf(pMappingString, needed, "%s,%s,%s", pchGUID, mapping->name, mapping->mapping);
            return pMappingString;
        }
        --mapping_index;
    }
    return NULL;
}

/*
 * Get the mapping string for this GUID
 */
char *
SDL_GameControllerMappingForGUID(SDL_JoystickGUID guid)
{
    char *pMappingString = NULL;
    ControllerMapping_t *mapping = SDL_PrivateGetControllerMappingForGUID(&guid, SDL_FALSE);
    if (mapping) {
        char pchGUID[33];
        size_t needed;
        SDL_JoystickGetGUIDString(guid, pchGUID, sizeof(pchGUID));
        /* allocate enough memory for GUID + ',' + name + ',' + mapping + \0 */
        needed = SDL_strlen(pchGUID) + 1 + SDL_strlen(mapping->name) + 1 + SDL_strlen(mapping->mapping) + 1;
        pMappingString = SDL_malloc(needed);
        if (!pMappingString) {
            SDL_OutOfMemory();
            return NULL;
        }
        SDL_snprintf(pMappingString, needed, "%s,%s,%s", pchGUID, mapping->name, mapping->mapping);
    }
    return pMappingString;
}

/*
 * Get the mapping string for this device
 */
char *
SDL_GameControllerMapping(SDL_GameController * gamecontroller)
{
    if (!gamecontroller) {
        return NULL;
    }

    return SDL_GameControllerMappingForGUID(gamecontroller->joystick->guid);
}

static void
SDL_GameControllerLoadHints()
{
    const char *hint = SDL_GetHint(SDL_HINT_GAMECONTROLLERCONFIG);
    if (hint && hint[0]) {
        size_t nchHints = SDL_strlen(hint);
        char *pUserMappings = SDL_malloc(nchHints + 1);
        char *pTempMappings = pUserMappings;
        SDL_memcpy(pUserMappings, hint, nchHints);
        pUserMappings[nchHints] = '\0';
        while (pUserMappings) {
            char *pchNewLine = NULL;

            pchNewLine = SDL_strchr(pUserMappings, '\n');
            if (pchNewLine)
                *pchNewLine = '\0';

            SDL_PrivateGameControllerAddMapping(pUserMappings, SDL_CONTROLLER_MAPPING_PRIORITY_USER);

            if (pchNewLine) {
                pUserMappings = pchNewLine + 1;
            } else {
                pUserMappings = NULL;
            }
        }
        SDL_free(pTempMappings);
    }
}

/*
 * Fill the given buffer with the expected controller mapping filepath.
 * Usually this will just be CONTROLLER_MAPPING_FILE, but for Android,
 * we want to get the internal storage path.
 */
static SDL_bool SDL_GetControllerMappingFilePath(size_t size)
{
#ifdef CONTROLLER_MAPPING_FILE
    #define STRING(X) SDL_STRINGIFY_ARG(X)
    return SDL_strlcpy(path, STRING(CONTROLLER_MAPPING_FILE), size) < size;
#elif defined(__ANDROID__)
    return SDL_snprintf(path, size, "%s/controller_map.txt", SDL_AndroidGetInternalStoragePath()) < size;
#else
    return SDL_FALSE;
#endif
}

static const char *s_ControllerMappings[] =
    {
#if SDL_JOYSTICK_XINPUT
        "xinput,XInput Controller,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b8,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b9,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
#endif
#if SDL_JOYSTICK_DINPUT
        "03000000fa2d00000100000000000000,3DRUDDER,leftx:a0,lefty:a1,rightx:a5,righty:a2,",
        "03000000022000000090000000000000,8Bitdo NES30 Pro,a:b1,b:b0,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:b9,rightx:a3,righty:a4,start:b11,x:b4,y:b3,",
        "03000000203800000900000000000000,8Bitdo NES30 Pro,a:b1,b:b0,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:b9,rightx:a3,righty:a4,start:b11,x:b4,y:b3,",
        "03000000c82d00000060000000000000,8Bitdo SF30 Pro,a:b1,b:b0,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:b9,rightx:a3,righty:a4,start:b11,x:b4,y:b3,",
        "03000000c82d00000061000000000000,8Bitdo SF30 Pro,a:b1,b:b0,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b2,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:b9,rightx:a3,righty:a4,start:b11,x:b4,y:b3,",
        "03000000102800000900000000000000,8Bitdo SFC30 GamePad,a:b1,b:b0,back:b10,leftshoulder:b6,leftx:a0,lefty:a1,rightshoulder:b7,start:b11,x:b4,y:b3,",
        "03000000a00500003232000000000000,8Bitdo Zero GamePad,a:b0,b:b1,back:b10,dpdown:+a2,dpleft:-a0,dpright:+a0,dpup:-a2,leftshoulder:b6,rightshoulder:b7,start:b11,x:b3,y:b4,",
        "03000000c82d00002038000000000000,8bitdo,a:b1,b:b0,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b2,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:b9,rightx:a3,righty:a4,start:b11,x:b4,y:b3,",
        "030000008f0e00001200000000000000,Acme GA-02,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b11,righttrigger:b7,rightx:a3,righty:a2,start:b9,x:b2,y:b3,",
        "03000000fa190000f0ff000000000000,Acteck AGJ-3200,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000341a00003608000000000000,Afterglow PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006f0e00000263000000000000,Afterglow PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006f0e00001101000000000000,Afterglow PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006f0e00001401000000000000,Afterglow PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006f0e00001402000000000000,Afterglow PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006f0e00001901000000000000,Afterglow PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006f0e00001a01000000000000,Afterglow PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000d62000001d57000000000000,Airflo PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000d81d00000b00000000000000,BUFFALO BSGP1601 Series ,a:b5,b:b3,back:b12,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b8,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b9,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b13,x:b4,y:b2,",
        "03000000d6200000e557000000000000,Batarang,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000c01100001352000000000000,Battalife Joystick,a:b6,b:b7,back:b2,leftshoulder:b0,leftx:a0,lefty:a1,rightshoulder:b1,start:b3,x:b4,y:b5,",
        "030000006f0e00003201000000000000,Battlefield 4 PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000bc2000006012000000000000,Betop 2126F,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000bc2000000055000000000000,Betop BFM Gamepad,a:b0,b:b1,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:b9,rightx:a3,righty:a4,start:b11,x:b3,y:b4,",
        "03000000bc2000006312000000000000,Betop Controller,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000bc2000006412000000000000,Betop Controller,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000c01100000555000000000000,Betop Controller,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000c01100000655000000000000,Betop Controller,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000790000000700000000000000,Betop Gamepad,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a4,start:b9,x:b3,y:b0,",
        "03000000808300000300000000000000,Betop Gamepad,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a4,start:b9,x:b3,y:b0,",
        "030000006b1400000055000000000000,Bigben PS3 Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "030000006b1400000103000000000000,Bigben PS3 Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b2,",
        "0300000066f700000500000000000000,BrutalLegendTest,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a3,righty:a2,start:b9,x:b0,y:b3,",
        "03000000e82000006058000000000000,Cideko AK08b,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000260900008888000000000000,Cyber Gadget GameCube Controller,a:b0,b:b1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,lefttrigger:a5,leftx:a0,lefty:a1,rightshoulder:b6,righttrigger:a4,rightx:a2,righty:a3~,start:b7,x:b2,y:b3,",
        "03000000a306000022f6000000000000,Cyborg V.3 Rumble Pad,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:+a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:-a3,rightx:a2,righty:a4,start:b9,x:b0,y:b3,",
        "03000000451300000830000000000000,Defender Game Racer X7,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "03000000791d00000103000000000000,Dual Box WII,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b6,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,righttrigger:b5,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000bd12000002e0000000000000,Dual USB Vibration Joystick,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b9,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b10,righttrigger:b5,rightx:a3,righty:a2,start:b11,x:b3,y:b0,",
        "030000006f0e00003001000000000000,EA SPORTS PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000341a00000108000000000000,EXEQ RF USB Gamepad 8206,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b8,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "030000008f0e00000f31000000000000,EXEQ,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b2,",
        "03000000b80500000410000000000000,Elecom Gamepad,a:b2,b:b3,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b1,",
        "03000000b80500000610000000000000,Elecom Gamepad,a:b2,b:b3,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b1,",
        "03000000852100000201000000000000,FF-GP1,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000000d0f00002700000000000000,FIGHTING STICK V3,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,start:b9,x:b0,y:b3,",
        "030000000d0f00008500000000000000,Fighting Commander 2016 PS3,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000000d0f00008400000000000000,Fighting Commander 5,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000000d0f00008700000000000000,Fighting Stick mini 4,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,start:b9,x:b0,y:b3,",
        "030000000d0f00008800000000000000,Fighting Stick mini 4,a:b1,b:b2,back:b9,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,start:b8,x:b0,y:b3,",
        "78696e70757403000000000000000000,Fightstick TES,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,lefttrigger:a2,rightshoulder:b5,righttrigger:a5,start:b7,x:b2,y:b3,",
        "03000000790000000600000000000000,G-Shark GS-GP702,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a4,start:b9,x:b3,y:b0,",
        "030000008f0e00000d31000000000000,GAMEPAD 3 TURBO,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000300f00000b01000000000000,GGE909 Recoil Pad,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a3,righty:a2,start:b9,x:b3,y:b0,",
        "03000000790000002201000000000000,Game Controller for PC,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "0300000066f700000100000000000000,Game VIB Joystick,a:b2,b:b3,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b8,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b9,righttrigger:b7,rightx:a3,righty:a2,start:b11,x:b0,y:b1,",
        "03000000280400000140000000000000,GamePad Pro USB,a:b1,b:b2,back:b8,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,start:b9,x:b0,y:b3,",
        "03000000ac0500003d03000000000000,GameSir,a:b0,b:b1,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:b9,rightx:a3,righty:a4,start:b11,x:b3,y:b4,",
        "03000000ac0500004d04000000000000,GameSir,a:b0,b:b1,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:b9,rightx:a3,righty:a4,start:b11,x:b3,y:b4,",
        "03000000ffff00000000000000000000,GameStop Gamepad,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "03000000260900002625000000000000,Gamecube Controller,a:b0,b:b1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b6,lefttrigger:a4,leftx:a0,lefty:a1,righttrigger:a5,rightx:a2,righty:a3,start:b7,x:b2,y:b3,",
        "030000005c1a00003330000000000000,Genius MaxFire Grandias 12V,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b10,lefttrigger:b7,leftx:a0,lefty:a1,rightshoulder:b4,rightstick:b11,righttrigger:b5,rightx:a3,righty:a2,start:b9,x:b2,y:b3,",
        "030000008305000031b0000000000000,Genius Maxfire Blaze 3,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "03000000451300000010000000000000,Genius Maxfire Grandias 12,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "030000008305000009a0000000000000,Genius,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "03000000f025000021c1000000000000,Gioteck PS3 Controller,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000f0250000c383000000000000,Gioteck VX2 Controller,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000f0250000c483000000000000,Gioteck VX2 Controller,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000f0250000c283000000000000,Gioteck,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000632500002605000000000000,HJD-X,a:b0,b:b1,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:b9,rightx:a3,righty:a4,start:b11,x:b3,y:b4,",
        "030000000d0f00006e00000000000000,HORIPAD 4 (PS3),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000000d0f00006600000000000000,HORIPAD 4 (PS4),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000000d0f0000ee00000000000000,HORIPAD mini4,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "03000000250900000017000000000000,HRAP2 on PS/SS/N64 Joypad to USB BOX,a:b2,b:b1,back:b9,leftshoulder:b5,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,righttrigger:b6,start:b8,x:b3,y:b0,",
        "03000000341a00000302000000000000,Hama Scorpad,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000000d0f00004900000000000000,Hatsune Miku Sho Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000d81400000862000000000000,HitBox Edition Cthulhu+,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b5,lefttrigger:b4,rightshoulder:b7,righttrigger:b6,start:b9,x:b0,y:b3,",
        "030000000d0f00005f00000000000000,Hori Fighting Commander 4 (PS3),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000000d0f00005e00000000000000,Hori Fighting Commander 4 (PS4),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000000d0f00004000000000000000,Hori Fighting Stick Mini 3,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b5,lefttrigger:b4,rightshoulder:b7,righttrigger:b6,start:b9,x:b0,y:b3,",
        "030000000d0f00000900000000000000,Hori Pad 3 Turbo,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000000d0f00005400000000000000,Hori Pad 3,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000000d0f00004d00000000000000,Hori Pad A,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000000d0f0000c100000000000000,Horipad,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000008f0e00001330000000000000,HuiJia SNES Controller,a:b2,b:b1,back:b8,dpdown:+a1,dpleft:-a0,dpright:+a0,dpup:-a1,leftshoulder:b6,rightshoulder:b7,start:b9,x:b3,y:b0,",
        "030000006f0e00002401000000000000,INJUSTICE FightStick PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,start:b9,x:b0,y:b3,",
        "03000000ac0500002c02000000000000,IPEGA,a:b0,b:b1,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b8,leftstick:b13,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b9,rightstick:b14,righttrigger:b7,rightx:a3,righty:a4,start:b11,x:b3,y:b4,",
        "03000000b50700001403000000000000,Impact Black,a:b2,b:b3,back:b8,leftshoulder:b4,leftstick:b10,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b11,righttrigger:b7,rightx:a3,righty:a2,start:b9,x:b0,y:b1,",
        "03000000491900000204000000000000,Ipega PG-9023,a:b0,b:b1,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:b9,rightx:a3,righty:a4,start:b11,x:b3,y:b4,",
        "030000006e0500000520000000000000,JC-P301U,a:b2,b:b3,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b8,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b9,righttrigger:b7,rightx:a2,righty:a3,start:b11,x:b0,y:b1,",
        "030000006e0500000320000000000000,JC-U3613M (DInput),a:b2,b:b3,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b8,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b9,righttrigger:b7,rightx:a2,righty:a3,start:b11,x:b0,y:b1,",
        "030000006e0500000720000000000000,JC-W01U,a:b2,b:b3,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b1,",
        "03000000790000000200000000000000,King PS3 Controller,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a4,start:b9,x:b3,y:b0,",
        "030000006d040000d1ca000000000000,Logitech ChillStream,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006d040000d2ca000000000000,Logitech Cordless Precision,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006d04000011c2000000000000,Logitech Cordless Wingman,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b9,leftstick:b5,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b10,rightstick:b2,righttrigger:b7,rightx:a3,righty:a4,x:b4,",
        "030000006d04000016c2000000000000,Logitech Dual Action,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006d04000018c2000000000000,Logitech F510 Gamepad,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006d04000019c2000000000000,Logitech F710 Gamepad,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,", /* Guide button doesn't seem to be sent in DInput mode. */
        "03000000380700008081000000000000,MADCATZ SFV Arcade FightStick Alpha PS4,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "03000000380700006382000000000000,MLG GamePad PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000250900006688000000000000,MP-8866 Super Dual Box,a:b2,b:b1,back:b9,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a2,righty:a3,start:b8,x:b3,y:b0,",
        "03000000380700006652000000000000,Mad Catz C.T.R.L.R,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a3,righty:a4,start:b9,x:b0,y:b3,",
        "03000000380700005032000000000000,Mad Catz FightPad PRO (PS3),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000380700005082000000000000,Mad Catz FightPad PRO (PS4),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "03000000380700008433000000000000,Mad Catz FightStick TE S+ (PS3),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000380700008483000000000000,Mad Catz FightStick TE S+ (PS4),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "03000000380700008134000000000000,Mad Catz FightStick TE2+ PS3,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b7,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b4,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000380700008184000000000000,Mad Catz FightStick TE2+ PS4,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b5,leftstick:b10,lefttrigger:a4,leftx:a0,lefty:a1,rightshoulder:b4,rightstick:b11,righttrigger:b7,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "03000000380700006252000000000000,Mad Catz Micro C.T.R.L.R,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a3,righty:a4,start:b9,x:b0,y:b3,",
        "03000000380700008034000000000000,Mad Catz TE2 PS3 Fightstick,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000380700008084000000000000,Mad Catz TE2 PS4 Fightstick,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "03000000380700001888000000000000,MadCatz SFIV FightStick PS3,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b5,lefttrigger:b7,leftx:a0,lefty:a1,rightshoulder:b4,righttrigger:b6,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "03000000380700008532000000000000,Madcatz Arcade Fightstick TE S PS3,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000380700003888000000000000,Madcatz Arcade Fightstick TE S+ PS3,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000002a0600001024000000000000,Matricom,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a3,righty:a4,start:b9,x:b2,y:b3,",
        "03000000250900000128000000000000,Mayflash Arcade Stick,a:b1,b:b2,back:b8,leftshoulder:b0,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b3,righttrigger:b7,start:b9,x:b5,y:b6,",
        "03000000790000004318000000000000,Mayflash GameCube Controller Adapter,a:b1,b:b2,back:b0,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b0,leftshoulder:b4,leftstick:b0,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b0,righttrigger:a4,rightx:a5,righty:a2,start:b9,x:b0,y:b3,",
        "03000000790000004418000000000000,Mayflash GameCube Controller,a:b1,b:b2,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b7,righttrigger:a4,rightx:a5,righty:a2,start:b9,x:b0,y:b3,",
        "030000008f0e00001030000000000000,Mayflash USB Adapter for original Sega Saturn controller,a:b0,b:b1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,lefttrigger:b5,rightshoulder:b2,righttrigger:b7,start:b9,x:b3,y:b4,",
        "0300000025090000e803000000000000,Mayflash Wii Classic Controller,a:b1,b:b0,back:b8,dpdown:b13,dpleft:b12,dpright:b14,dpup:b11,guide:b10,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b2,",
        "03000000790000000018000000000000,Mayflash WiiU Pro Game Controller Adapter (DInput),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000efbe0000edfe000000000000,Monect Virtual Controller,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a5,rightx:a3,righty:a4,start:b9,x:b3,y:b0,",
        "030000001008000001e5000000000000,NEXT SNES Controller,a:b2,b:b1,back:b8,dpdown:+a1,dpleft:-a0,dpright:+a0,dpup:-a1,leftshoulder:b4,rightshoulder:b6,start:b9,x:b3,y:b0,",
        "03000000152000000182000000000000,NGDS,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a3,righty:a4,start:b9,x:b3,y:b0,",
        "030000004b120000014d000000000000,NYKO AIRFLO,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:a3,leftstick:a0,lefttrigger:b6,rightshoulder:b5,rightstick:a2,righttrigger:b7,start:b9,x:b2,y:b3,",
        "03000000bd12000015d0000000000000,Nintendo Retrolink USB Super SNES Classic Controller,a:b2,b:b1,back:b8,leftshoulder:b4,leftx:a0,lefty:a1,rightshoulder:b5,start:b9,x:b3,y:b0,",
        "030000007e0500000920000000000000,Nintendo Switch Pro Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "030000000d0500000308000000000000,Nostromo N45,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b9,leftshoulder:b4,leftstick:b12,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b11,righttrigger:b7,rightx:a3,righty:a2,start:b10,x:b2,y:b3,",
        "03000000d62000006d57000000000000,OPP PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000362800000100000000000000,OUYA Game Controller,a:b0,b:b3,dpdown:b9,dpleft:b10,dpright:b11,dpup:b8,guide:b14,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,righttrigger:b13,rightx:a3,righty:a4,x:b1,y:b2,",
        "03000000782300000a10000000000000,Onlive Wireless Controller,a:b15,b:b14,back:b7,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b5,leftshoulder:b11,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b10,rightstick:b8,righttrigger:a5,rightx:a3,righty:a4,start:b6,x:b13,y:b12,",
        "030000006b14000001a1000000000000,Orange Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b6,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,righttrigger:a4,rightx:a5,righty:a2,start:b9,x:b2,y:b3,",
        "03000000120c0000f60e000000000000,P4 Wired Gamepad,a:b1,b:b2,back:b12,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b5,lefttrigger:b7,rightshoulder:b4,righttrigger:b6,start:b9,x:b0,y:b3,",
        "03000000632500002306000000000000,PS Controller,a:b0,b:b1,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:b9,rightx:a2,righty:a3,start:b11,x:b3,y:b4,",
        "03000000e30500009605000000000000,PS to USB convert cable,a:b2,b:b1,back:b9,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a2,righty:a3,start:b8,x:b3,y:b0,",
        "03000000100800000100000000000000,PS1 Controller,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a3,righty:a2,start:b9,x:b3,y:b0,",
        "030000008f0e00007530000000000000,PS1 Controller,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b1,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000100800000300000000000000,PS2 Controller,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a4,righty:a2,start:b9,x:b3,y:b0,",
        "03000000250900008888000000000000,PS2 Controller,a:b2,b:b1,back:b9,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a2,righty:a3,start:b8,x:b3,y:b0,",
        "03000000666600006706000000000000,PS2 Controller,a:b2,b:b1,back:b8,dpdown:b14,dpleft:b15,dpright:b13,dpup:b12,leftshoulder:b6,leftstick:b9,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b10,righttrigger:b5,rightx:a2,righty:a3,start:b11,x:b3,y:b0,",
        "030000006b1400000303000000000000,PS2 Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "030000009d0d00001330000000000000,PS2 Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "03000000250900000500000000000000,PS3 Controller,a:b2,b:b1,back:b9,dpdown:h0.8,dpleft:h0.4,dpright:h0.2,dpup:h0.1,guide:,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a2,righty:a3,start:b8,x:b0,y:b3,",
        "030000004c0500006802000000000000,PS3 Controller,a:b2,b:b1,back:b9,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b6,leftstick:b10,lefttrigger:a3~,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:a4~,rightx:a2,righty:a5,start:b8,x:b3,y:b0,",
        "03000000632500007505000000000000,PS3 Controller,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000888800000803000000000000,PS3 Controller,a:b2,b:b1,back:b8,dpdown:h0.8,dpleft:h0.4,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b9,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:b7,rightx:a3,righty:a4,start:b11,x:b0,y:b3,",
        "030000008f0e00001431000000000000,PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000003807000056a8000000000000,PS3 RF pad,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000100000008200000000000000,PS360+ v1.66,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:b6,leftx:h0.4,rightshoulder:b5,righttrigger:b7,start:b9,x:b0,y:b3,",
        "030000004c050000a00b000000000000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000004c050000c405000000000000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000004c050000cc09000000000000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000008f0e00000300000000000000,Piranha xtreme,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a3,righty:a2,start:b9,x:b3,y:b0,",
        "03000000d62000006dca000000000000,PowerA Pro Ex,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000d62000009557000000000000,Pro Elite PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000d62000009f31000000000000,Pro Ex mini PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000d6200000c757000000000000,Pro Ex mini PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000222c00000020000000000000,QANBA DRONE ARCADE JOYSTICK,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:a3,rightshoulder:b5,righttrigger:a4,start:b9,x:b0,y:b3,",
        "03000000300f00000011000000000000,QanBa Arcade JoyStick 1008,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,start:b10,x:b0,y:b3,",
        "03000000300f00001611000000000000,QanBa Arcade JoyStick 4018,a:b1,b:b2,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b9,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,start:b8,x:b0,y:b3,",
        "03000000300f00001210000000000000,QanBa Joystick Plus,a:b0,b:b1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,rightshoulder:b5,start:b9,x:b2,y:b3,",
        "03000000341a00000104000000000000,QanBa Joystick Q4RAF,a:b5,b:b6,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b0,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b3,righttrigger:b7,start:b9,x:b1,y:b2,",
        "03000000222c00000223000000000000,Qanba Obsidian Arcade Joystick PS3 Mode,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000222c00000023000000000000,Qanba Obsidian Arcade Joystick PS4 Mode,a:b1,b:b2,back:b13,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000000d0f00001100000000000000,REAL ARCADE PRO.3,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,rightshoulder:b5,rightstick:b11,righttrigger:b7,start:b9,x:b0,y:b3,",
        "030000000d0f00007000000000000000,REAL ARCADE PRO.4 VLX,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,rightshoulder:b5,rightstick:b11,righttrigger:b7,start:b9,x:b0,y:b3,",
        "030000000d0f00002200000000000000,REAL ARCADE Pro.V3,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000321500000003000000000000,Razer Hydra,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b8,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b9,righttrigger:a2,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "03000000321500000204000000000000,Razer Panthera (PS3),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000321500000104000000000000,Razer Panthera (PS4),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000000d0f00006a00000000000000,Real Arcade Pro.4,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000000d0f00006b00000000000000,Real Arcade Pro.4,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000000d0f00008a00000000000000,Real Arcade Pro.4,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000000d0f00008b00000000000000,Real Arcade Pro.4,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000000d0f00005b00000000000000,Real Arcade Pro.V4,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000000d0f00005c00000000000000,Real Arcade Pro.V4,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "0300000000f000000300000000000000,RetroUSB.com RetroPad,a:b1,b:b5,back:b2,leftshoulder:b6,leftx:a0,lefty:a1,rightshoulder:b7,start:b3,x:b0,y:b4,",
        "0300000000f00000f100000000000000,RetroUSB.com Super RetroPort,a:b1,b:b5,back:b2,leftshoulder:b6,leftx:a0,lefty:a1,rightshoulder:b7,start:b3,x:b0,y:b4,",
        "03000000790000001100000000000000,Retrolink SNES Controller,a:b2,b:b1,back:b8,dpdown:+a4,dpleft:-a3,dpright:+a3,dpup:-a4,leftshoulder:b4,rightshoulder:b5,start:b9,x:b3,y:b0,",
        "030000006b140000010d000000000000,Revolution Pro Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000006f0e00001e01000000000000,Rock Candy PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006f0e00002801000000000000,Rock Candy PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006f0e00002f01000000000000,Rock Candy PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000341a00000208000000000000,SL-6555-SBK,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b8,lefttrigger:-a4,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b9,righttrigger:a4,rightx:a3,righty:a2,start:b7,x:b2,y:b3,",
        "03000000341a00000908000000000000,SL-6566,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "03000000790000001c18000000000000,STK-7024X,a:b0,b:b1,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:b9,rightx:a3,righty:a4,start:b11,x:b3,y:b4,",
        "03000000ff1100003133000000000000,SVEN X-PAD,a:b2,b:b3,back:b4,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,righttrigger:b9,rightx:a2,righty:a4,start:b5,x:b0,y:b1,",
        "03000000a306000023f6000000000000,Saitek Cyborg V.1 Game pad,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a4,start:b9,x:b0,y:b3,",
        "03000000a30600001af5000000000000,Saitek Cyborg,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a3,righty:a4,start:b9,x:b0,y:b3,",
        "03000000300f00001201000000000000,Saitek Dual Analog Pad,a:b2,b:b3,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b11,righttrigger:b7,rightx:a3,righty:a2,start:b9,x:b0,y:b1,",
        "03000000a30600000cff000000000000,Saitek P2500 Force Rumble Pad,a:b2,b:b3,back:b11,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b8,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b9,righttrigger:b7,rightx:a2,righty:a3,x:b0,y:b1,",
        "03000000a30600000c04000000000000,Saitek P2900,a:b1,b:b2,back:b12,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a3,righty:a2,start:b9,x:b0,y:b3,",
        "03000000300f00001001000000000000,Saitek P480 Rumble Pad,a:b2,b:b3,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b11,righttrigger:b7,rightx:a3,righty:a2,start:b9,x:b0,y:b1,",
        "03000000a30600000b04000000010000,Saitek P990 Dual Analog Pad,a:b1,b:b2,back:b9,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a3,righty:a2,start:b8,x:b0,y:b3,",
        "03000000a30600000b04000000000000,Saitek P990,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a3,righty:a2,start:b9,x:b0,y:b3,",
        "03000000a30600002106000000000000,Saitek PS1000,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a4,start:b9,x:b0,y:b3,",
        "03000000a306000020f6000000000000,Saitek PS2700,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a4,start:b9,x:b0,y:b3,",
        "03000000300f00001101000000000000,Saitek Rumble Pad,a:b2,b:b3,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b10,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b11,righttrigger:b7,rightx:a3,righty:a2,start:b9,x:b0,y:b1,",
        "0300000000050000289b000000000000,Saturn_Adapter_2.0,a:b1,b:b2,leftshoulder:b6,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,righttrigger:b5,start:b9,x:b0,y:b3,",
        "030000009b2800000500000000000000,Saturn_Adapter_2.0,a:b1,b:b2,leftshoulder:b6,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,righttrigger:b5,start:b9,x:b0,y:b3,",
        "030000008f0e00000800000000000000,SpeedLink Strike FX,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000c01100000591000000000000,Speedlink Torid,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000381000001814000000000000,SteelSeries Stratus XL,a:b0,b:b1,back:b18,dpdown:b13,dpleft:b14,dpright:b15,dpup:b12,guide:b19,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b2,y:b3,",
        "03000000110100001914000000000000,SteelSeries,a:b0,b:b1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:,leftstick:b13,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:,rightstick:b14,righttrigger:b7,rightx:a3,righty:a4,start:b11,x:b3,y:b4,",
        "03000000d620000011a7000000000000,Switch,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000004f04000007d0000000000000,T Mini Wireless,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000fa1900000706000000000000,Team 5,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000b50700001203000000000000,Techmobility X6-38V,a:b2,b:b3,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b11,righttrigger:b7,rightx:a3,righty:a2,start:b9,x:b0,y:b1,",
        "030000004f04000015b3000000000000,Thrustmaster Dual Analog 4,a:b0,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b1,y:b3,",
        "030000004f04000023b3000000000000,Thrustmaster Dual Trigger 3-in-1,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000004f04000004b3000000000000,Thrustmaster Firestorm Dual Power 3,a:b0,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b1,y:b3,",
        "030000004f04000000b3000000000000,Thrustmaster Firestorm Dual Power,a:b0,b:b2,back:b9,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b11,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b12,righttrigger:b7,rightx:a2,righty:a3,start:b10,x:b1,y:b3,",
        "03000000666600000488000000000000,TigerGame PS/PS2 Game Controller Adapter,a:b2,b:b1,back:b9,dpdown:b14,dpleft:b15,dpright:b13,dpup:b12,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a2,righty:a3,start:b8,x:b3,y:b0,",
        "03000000d62000006000000000000000,Tournament PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000005f140000c501000000000000,Trust Gamepad,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000b80500000210000000000000,Trust Gamepad,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000d90400000200000000000000,TwinShock PS2,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a3,righty:a2,start:b9,x:b3,y:b0,",
        "03000000300f00000701000000000000,USB 4-Axis 12-Button Gamepad,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a3,righty:a2,start:b9,x:b3,y:b0,",
        "03000000632500002305000000000000,USB Vibration Joystick (BM),a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000341a00002308000000000000,USB gamepad,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "030000005509000000b4000000000000,USB gamepad,a:b10,b:b11,back:b5,dpdown:b1,dpleft:b2,dpright:b3,dpup:b0,guide:b14,leftshoulder:b8,leftstick:b6,lefttrigger:a4,leftx:a0,lefty:a1,rightshoulder:b9,rightstick:b7,righttrigger:a5,rightx:a2,righty:a3,start:b4,x:b12,y:b13,",
        "030000006b1400000203000000000000,USB gamepad,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "03000000790000000a00000000000000,USB gamepad,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a4,start:b9,x:b3,y:b0,",
        "03000000f0250000c183000000000000,USB gamepad,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000ff1100004133000000000000,USB gamepad,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a4,righty:a2,start:b9,x:b3,y:b0,",
        "03000000790000001b18000000000000,Venom Arcade Joystick,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,start:b9,x:b0,y:b3,",
        "03000000450c00002043000000000000,XEOX Gamepad SL-6556-BK,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "03000000341a00000608000000000000,Xeox,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "03000000172700004431000000000000,XiaoMi Game Controller,a:b0,b:b1,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b20,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:a7,rightx:a2,righty:a5,start:b11,x:b3,y:b4,",
        "03000000790000004f18000000000000,ZD-T Android,a:b0,b:b1,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:b9,rightx:a3,righty:a4,start:b11,x:b3,y:b4,",
        "03000000d81d00000f00000000000000,iBUFFALO BSGP1204 Series,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000d81d00001000000000000000,iBUFFALO BSGP1204P Series,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000830500006020000000000000,iBuffalo SNES Controller,a:b1,b:b0,back:b6,dpdown:+a1,dpleft:-a0,dpright:+a0,dpup:-a1,leftshoulder:b4,rightshoulder:b5,start:b7,x:b3,y:b2,",
        "030000004f04000003d0000000000000,run'n'drive,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b7,leftshoulder:a3,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:a4,rightstick:b11,righttrigger:b5,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "03000000101c0000171c000000000000,uRage Gamepad,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
#endif
#if defined(__MACOSX__)
        "03000000022000000090000001000000,8Bitdo NES30 Pro,a:b1,b:b0,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:b9,rightx:a2,righty:a3,start:b11,x:b4,y:b3,",
        "03000000203800000900000000010000,8Bitdo NES30 Pro,a:b1,b:b0,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:b9,rightx:a2,righty:a3,start:b11,x:b4,y:b3,",
        "03000000102800000900000000000000,8Bitdo SFC30 GamePad Joystick,a:b1,b:b0,back:b10,leftshoulder:b6,leftx:a0,lefty:a1,rightshoulder:b7,start:b11,x:b4,y:b3,",
        "03000000a00500003232000008010000,8Bitdo Zero GamePad,a:b0,b:b1,back:b10,dpdown:+a1,dpleft:-a0,dpright:+a0,dpup:-a1,leftshoulder:b6,rightshoulder:b7,start:b11,x:b3,y:b4,",
        "03000000a00500003232000009010000,8Bitdo Zero GamePad,a:b0,b:b1,back:b10,dpdown:+a1,dpleft:-a0,dpright:+a0,dpup:-a1,leftshoulder:b6,rightshoulder:b7,start:b11,x:b3,y:b4,",
        "030000008305000031b0000000000000,Cideko AK08b,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000260900008888000088020000,Cyber Gadget GameCube Controller,a:b0,b:b1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,lefttrigger:a4,leftx:a0,lefty:a1,rightshoulder:b6,righttrigger:a5,rightx:a2,righty:a3~,start:b7,x:b2,y:b3,",
        "03000000a306000022f6000001030000,Cyborg V.3 Rumble Pad,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:+a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:-a3,rightx:a2,righty:a4,start:b9,x:b0,y:b3,",
        "03000000790000000600000000000000,G-Shark GP-702,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a3,righty:a4,start:b9,x:b3,y:b0,",
        "0500000047532047616d657061640000,GameStop Gamepad,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "03000000ad1b000001f9000000000000,Gamestop BB-070 X360 Controller,a:b0,b:b1,back:b9,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b10,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,righttrigger:a5,rightx:a3,righty:a4,start:b8,x:b2,y:b3,",
        "030000000d0f00005f00000000000000,HORI Fighting Commander 4 PS3,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000000d0f00005e00000000000000,HORI Fighting Commander 4 PS4,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000000d0f00004d00000000000000,HORI Gem Pad 3,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000000d0f00006e00000000010000,HORIPAD 4 (PS3),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000000d0f00006600000000010000,HORIPAD 4 (PS4),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000000d0f00006600000000000000,HORIPAD FPS PLUS 4,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000000d0f00005f00000000010000,Hori Fighting Commander 4 (PS3),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000000d0f00005e00000000010000,Hori Fighting Commander 4 (PS4),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000008f0e00001330000011010000,HuiJia SNES Controller,a:b4,b:b2,back:b16,dpdown:+a2,dpleft:-a0,dpright:+a0,dpup:-a2,leftshoulder:b12,rightshoulder:b14,start:b18,x:b6,y:b0,",
        "030000006d04000016c2000000020000,Logitech Dual Action,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006d04000016c2000000030000,Logitech Dual Action,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006d04000016c2000014040000,Logitech Dual Action,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006d04000016c2000000000000,Logitech F310 Gamepad (DInput),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,", /* Guide button doesn't seem to be sent in DInput mode. */
        "030000006d04000018c2000000000000,Logitech F510 Gamepad (DInput),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006d0400001fc2000000000000,Logitech F710 Gamepad (XInput),a:b0,b:b1,back:b9,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b10,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,righttrigger:a5,rightx:a3,righty:a4,start:b8,x:b2,y:b3,",
        "030000006d04000019c2000000000000,Logitech Wireless Gamepad (DInput),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,", /* This includes F710 in DInput mode and the "Logitech Cordless RumblePad 2", at the very least. */
        "03000000d8140000cecf000000000000,MC Cthulhu,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,start:b9,x:b0,y:b3,",
        "03000000380700005032000000010000,Mad Catz FightPad PRO (PS3),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000380700005082000000010000,Mad Catz FightPad PRO (PS4),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "03000000380700008433000000010000,Mad Catz FightStick TE S+ (PS3),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000380700008483000000010000,Mad Catz FightStick TE S+ (PS4),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "03000000790000004418000000010000,Mayflash GameCube Controller,a:b1,b:b2,dpdown:b14,dpleft:b15,dpright:b13,dpup:b12,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b7,righttrigger:a4,rightx:a5,righty:a2,start:b9,x:b0,y:b3,",
        "0300000025090000e803000000000000,Mayflash Wii Classic Controller,a:b1,b:b0,back:b8,dpdown:b13,dpleft:b12,dpright:b14,dpup:b11,guide:b10,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b2,",
        "03000000790000000018000000000000,Mayflash WiiU Pro Game Controller Adapter (DInput),a:b4,b:b8,back:b32,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b16,leftstick:b40,lefttrigger:b24,leftx:a0,lefty:a4,rightshoulder:b20,rightstick:b44,righttrigger:b28,rightx:a8,righty:a12,start:b36,x:b0,y:b12,",
        "030000001008000001e5000006010000,NEXT SNES Controller,a:b2,b:b1,back:b8,dpdown:+a1,dpleft:-a0,dpright:+a0,dpup:-a1,leftshoulder:b4,rightshoulder:b6,start:b9,x:b3,y:b0,",
        "030000007e0500000920000000000000,Nintendo Switch Pro Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "030000004c0500006802000000000000,PS3 Controller,a:b14,b:b13,back:b0,dpdown:b6,dpleft:b7,dpright:b5,dpup:b4,guide:b16,leftshoulder:b10,leftstick:b1,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b11,rightstick:b2,righttrigger:b9,rightx:a2,righty:a3,start:b3,x:b15,y:b12,",
        "030000004c0500006802000000010000,PS3 Controller,a:b14,b:b13,back:b0,dpdown:b6,dpleft:b7,dpright:b5,dpup:b4,guide:b16,leftshoulder:b10,leftstick:b1,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b11,rightstick:b2,righttrigger:b9,rightx:a2,righty:a3,start:b3,x:b15,y:b12,",
        "030000004c050000a00b000000010000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000004c050000c405000000000000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000004c050000c405000000010000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000004c050000cc09000000010000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000008f0e00000300000000000000,Piranha xtreme,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a3,righty:a2,start:b9,x:b3,y:b0,",
        "030000008916000000fd000000000000,Razer Onza TE,a:b0,b:b1,back:b9,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b10,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,righttrigger:a5,rightx:a3,righty:a4,start:b8,x:b2,y:b3,",
        "03000000321500000204000000010000,Razer Panthera (PS3),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000321500000104000000010000,Razer Panthera (PS4),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "03000000321500000010000000010000,Razer RAIJU,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "0300000032150000030a000000000000,Razer Wildcat,a:b0,b:b1,back:b9,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b10,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,righttrigger:a5,rightx:a3,righty:a4,start:b8,x:b2,y:b3,",
        "03000000790000001100000000000000,Retrolink Classic Controller,a:b2,b:b1,back:b8,leftshoulder:b4,leftx:a3,lefty:a4,rightshoulder:b5,start:b9,x:b3,y:b0,",
        "03000000790000001100000006010000,Retrolink SNES Controller,a:b2,b:b1,back:b8,dpdown:+a4,dpleft:-a3,dpright:+a3,dpup:-a4,leftshoulder:b4,rightshoulder:b5,start:b9,x:b3,y:b0,",
        "030000006b140000010d000000010000,Revolution Pro Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000003512000021ab000000000000,SFC30 Joystick,a:b1,b:b0,back:b10,leftshoulder:b6,leftx:a0,lefty:a1,rightshoulder:b7,start:b11,x:b4,y:b3,",
        "03000000b40400000a01000000000000,Sega Saturn USB Gamepad,a:b0,b:b1,back:b5,guide:b2,leftshoulder:b6,leftx:a0,lefty:a1,rightshoulder:b7,start:b8,x:b3,y:b4,",
        "03000000811700007e05000000000000,Sega Saturn,a:b2,b:b4,dpdown:b16,dpleft:b15,dpright:b14,dpup:b17,leftshoulder:b8,lefttrigger:a5,leftx:a0,lefty:a2,rightshoulder:b9,righttrigger:a4,start:b13,x:b0,y:b6,",
        "030000004c050000cc09000000000000,Sony DualShock 4 V2,a:b1,b:b2,back:b13,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000004c050000a00b000000000000,Sony DualShock 4 Wireless Adaptor,a:b1,b:b2,back:b13,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000005e0400008e02000001000000,Steam Virtual Gamepad,a:b0,b:b1,back:b9,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,righttrigger:a5,rightx:a3,righty:a4,start:b8,x:b2,y:b3,",
        "03000000110100002014000000000000,SteelSeries Nimbus,a:b0,b:b1,dpdown:b9,dpleft:b11,dpright:b10,dpup:b8,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3,start:b12,x:b2,y:b3,",
        "03000000110100002014000001000000,SteelSeries Nimbus,a:b0,b:b1,dpdown:b9,dpleft:b11,dpright:b10,dpup:b8,guide:b12,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1~,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3~,x:b2,y:b3,",
        "03000000381000002014000001000000,SteelSeries Nimbus,a:b0,b:b1,dpdown:b9,dpleft:b11,dpright:b10,dpup:b8,guide:b12,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1~,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3~,x:b2,y:b3,",
        "03000000110100001714000000000000,SteelSeries Stratus XL,a:b0,b:b1,dpdown:b9,dpleft:b11,dpright:b10,dpup:b8,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1~,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3~,start:b12,x:b2,y:b3,",
        "03000000110100001714000020010000,SteelSeries Stratus XL,a:b0,b:b1,dpdown:b9,dpleft:b11,dpright:b10,dpup:b8,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1~,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3~,start:b12,x:b2,y:b3,",
        "030000004f04000015b3000000000000,Thrustmaster Dual Analog 3.2,a:b0,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b1,y:b3,",
        "030000004f04000000b3000000000000,Thrustmaster Firestorm Dual Power,a:b0,b:b2,back:b9,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b11,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,righttrigger:b7,rightx:a2,righty:a3,start:b10,x:b1,y:b3,",
        "03000000bd12000015d0000000000000,Tomee SNES USB Controller,a:b2,b:b1,back:b8,leftshoulder:b4,leftx:a0,lefty:a1,rightshoulder:b5,start:b9,x:b3,y:b0,",
        "03000000bd12000015d0000000010000,Tomee SNES USB Controller,a:b2,b:b1,back:b8,dpdown:+a1,dpleft:-a0,dpright:+a0,dpup:-a1,leftshoulder:b4,rightshoulder:b5,start:b9,x:b3,y:b0,",
        "03000000100800000100000000000000,Twin USB Joystick,a:b4,b:b2,back:b16,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b12,leftstick:b20,lefttrigger:b8,leftx:a0,lefty:a2,rightshoulder:b14,rightstick:b22,righttrigger:b10,rightx:a6,righty:a4,start:b18,x:b6,y:b0,",
        "050000005769696d6f74652028303000,Wii Remote,a:b4,b:b5,back:b7,dpdown:b3,dpleft:b0,dpright:b1,dpup:b2,guide:b8,leftshoulder:b11,lefttrigger:b12,leftx:a0,lefty:a1,start:b6,x:b10,y:b9,",
        "050000005769696d6f74652028313800,Wii U Pro Controller,a:b16,b:b15,back:b7,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b8,leftshoulder:b19,leftstick:b23,lefttrigger:b21,leftx:a0,lefty:a1,rightshoulder:b20,rightstick:b24,righttrigger:b22,rightx:a2,righty:a3,start:b6,x:b18,y:b17,",
        "030000005e0400008e02000000000000,X360 Controller,a:b0,b:b1,back:b9,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b10,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,righttrigger:a5,rightx:a3,righty:a4,start:b8,x:b2,y:b3,",
        "03000000c6240000045d000000000000,Xbox 360 Wired Controller,a:b0,b:b1,back:b9,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b10,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,righttrigger:a5,rightx:a3,righty:a4,start:b8,x:b2,y:b3,",
        "030000005e040000d102000000000000,Xbox One Wired Controller,a:b0,b:b1,back:b9,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b10,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,righttrigger:a5,rightx:a3,righty:a4,start:b8,x:b2,y:b3,",
        "030000005e040000dd02000000000000,Xbox One Wired Controller,a:b0,b:b1,back:b9,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b10,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,righttrigger:a5,rightx:a3,righty:a4,start:b8,x:b2,y:b3,",
        "030000005e040000e302000000000000,Xbox One Wired Controller,a:b0,b:b1,back:b9,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b10,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,righttrigger:a5,rightx:a3,righty:a4,start:b8,x:b2,y:b3,",
        "030000005e040000e002000000000000,Xbox Wireless Controller,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b8,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b9,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000005e040000e002000003090000,Xbox Wireless Controller,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b8,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b9,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000005e040000ea02000000000000,Xbox Wireless Controller,a:b0,b:b1,back:b9,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b10,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,righttrigger:a5,rightx:a3,righty:a4,start:b8,x:b2,y:b3,",
        "030000005e040000fd02000003090000,Xbox Wireless Controller,a:b0,b:b1,back:b16,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b15,leftshoulder:b6,leftstick:b13,lefttrigger:a5,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:a4,rightx:a2,righty:a3,start:b11,x:b3,y:b4,",
        "03000000172700004431000029010000,XiaoMi Game Controller,a:b0,b:b1,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b15,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:a6,rightx:a2,righty:a5,start:b11,x:b3,y:b4,",
        "03000000120c0000100e000000010000,ZEROPLUS P4 Gamepad,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "03000000830500006020000000010000,iBuffalo SNES Controller,a:b1,b:b0,back:b6,dpdown:+a1,dpleft:-a0,dpright:+a0,dpup:-a1,leftshoulder:b4,rightshoulder:b5,start:b7,x:b3,y:b2,",
        "03000000830500006020000000000000,iBuffalo USB 2-axis 8-button Gamepad,a:b1,b:b0,back:b6,leftshoulder:b4,leftx:a0,lefty:a1,rightshoulder:b5,start:b7,x:b3,y:b2,",
#endif
#if defined(__LINUX__)
        "03000000c82d00000190000011010000,8Bitdo NES30 Pro 8Bitdo NES30 Pro,a:b1,b:b0,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b13,lefttrigger:a4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:a5,rightx:a2,righty:a3,start:b11,x:b4,y:b3,",
        "03000000022000000090000011010000,8Bitdo NES30 Pro,a:b1,b:b0,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:b9,rightx:a2,righty:a3,start:b11,x:b4,y:b3,",
        "05000000203800000900000000010000,8Bitdo NES30 Pro,a:b1,b:b0,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:b9,rightx:a2,righty:a3,start:b11,x:b4,y:b3,",
        "05000000c82d00002038000000010000,8Bitdo NES30 Pro,a:b1,b:b0,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b2,leftshoulder:b6,leftstick:b13,lefttrigger:a5,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:a4,rightx:a2,righty:a3,start:b11,x:b4,y:b3,",
        "05000000c82d00000061000000010000,8Bitdo SF30 Pro,a:b1,b:b0,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b2,leftshoulder:b6,leftstick:b13,lefttrigger:a5,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:a4,rightx:a2,righty:a3,start:b11,x:b4,y:b3,",
        "05000000102800000900000000010000,8Bitdo SFC30 GamePad,a:b1,b:b0,back:b10,leftshoulder:b6,leftx:a0,lefty:a1,rightshoulder:b7,start:b11,x:b4,y:b3,",
        "05000000c82d00003028000000010000,8Bitdo SFC30 GamePad,a:b1,b:b0,back:b10,leftshoulder:b6,leftx:a0,lefty:a1,rightshoulder:b7,start:b11,x:b4,y:b3,",
        "05000000a00500003232000001000000,8Bitdo Zero GamePad,a:b0,b:b1,back:b10,leftshoulder:b6,leftx:a0,lefty:a1,rightshoulder:b7,start:b11,x:b3,y:b4,",
        "05000000a00500003232000008010000,8Bitdo Zero GamePad,a:b0,b:b1,back:b10,dpdown:+a1,dpleft:-a0,dpright:+a0,dpup:-a1,leftshoulder:b6,rightshoulder:b7,start:b11,x:b3,y:b4,",
        "05000000050b00000045000031000000,ASUS Gamepad,a:b0,b:b1,back:b9,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b6,leftshoulder:b4,leftstick:b7,lefttrigger:a5,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b8,righttrigger:a4,rightx:a2,righty:a3,start:b10,x:b2,y:b3,",
        "030000006f0e00003901000020060000,Afterglow Controller for Xbox One,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000006f0e00003901000000430000,Afterglow Prismatic Controller,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000006f0e00001302000000010000,Afterglow,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "03000000100000008200000011010000,Akishop Customs PS360+ v1.66,a:b1,b:b2,back:b12,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,start:b9,x:b0,y:b3,",
        "03000000b40400000a01000000010000,CYPRESS USB Gamepad,a:b0,b:b1,back:b5,guide:b2,leftshoulder:b6,leftx:a0,lefty:a1,rightshoulder:b7,start:b8,x:b3,y:b4,",
        "03000000e82000006058000001010000,Cideko AK08b,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000260900008888000000010000,Cyber Gadget GameCube Controller,a:b0,b:b1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,lefttrigger:a4,leftx:a0,lefty:a1,rightshoulder:b6,righttrigger:a5,rightx:a2,righty:a3~,start:b7,x:b2,y:b3,",
        "03000000a306000022f6000011010000,Cyborg V.3 Rumble Pad,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:+a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:-a3,rightx:a2,righty:a4,start:b9,x:b0,y:b3,",
        "03000000790000000600000010010000,DragonRise Inc. Generic USB Joystick,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a3,righty:a4,start:b9,x:b3,y:b0,",
        "030000006f0e00003001000001010000,EA Sports PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "0300000079000000d418000000010000,GPD Win 2 Controller,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "0500000047532067616d657061640000,GS gamepad,a:b0,b:b1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "03000000341a000005f7000010010000,GameCube {HuiJia USB box},a:b1,b:b2,dpdown:b14,dpleft:b15,dpright:b13,dpup:b12,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b7,righttrigger:a4,rightx:a5,righty:a2,start:b9,x:b0,y:b3,",
        "03000000bc2000000055000011010000,GameSir G3w,a:b0,b:b1,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b13,lefttrigger:a5,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:a4,rightx:a2,righty:a3,start:b11,x:b3,y:b4,",
        "0500000047532047616d657061640000,GameStop Gamepad,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "030000006f0e00000104000000010000,Gamestop Logic3 Controller,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000006f0e00001304000000010000,Generic X-Box pad,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:a0,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:a3,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "03000000f0250000c183000010010000,Goodbetterbest Ltd USB Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000280400000140000000010000,Gravis GamePad Pro USB ,a:b1,b:b2,back:b8,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,start:b9,x:b0,y:b3,",
        "030000008f0e00000610000000010000,GreenAsia Electronics 4Axes 12Keys GamePad ,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b9,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b10,righttrigger:b5,rightx:a3,righty:a2,start:b11,x:b3,y:b0,",
        "030000008f0e00001200000010010000,GreenAsia Inc. USB Joystick,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b11,righttrigger:b7,rightx:a3,righty:a2,start:b9,x:b2,y:b3,",
        "03000000c9110000f055000011010000,HJC Game GAMEPAD,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "030000000d0f00001000000011010000,HORI CO. LTD. FIGHTING STICK 3,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,start:b9,x:b0,y:b3,",
        "030000000d0f00002200000011010000,HORI CO. LTD. REAL ARCADE Pro.V3,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,start:b9,x:b0,y:b3,",
        "030000000d0f00006a00000011010000,HORI CO. LTD. Real Arcade Pro.4,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000000d0f00006b00000011010000,HORI CO. LTD. Real Arcade Pro.4,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000000d0f00006e00000011010000,HORIPAD 4 (PS3),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000000d0f00006600000011010000,HORIPAD 4 (PS4),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000000d0f00006700000001010000,HORIPAD ONE,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "06000000adde0000efbe000002010000,Hidromancer Game Controller,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "03000000d81400000862000011010000,HitBox (PS3/PC) Analog Mode,a:b1,b:b2,back:b8,guide:b9,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,start:b12,x:b0,y:b3,",
        "030000000d0f00005f00000011010000,Hori Fighting Commander 4 (PS3),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000000d0f00005e00000011010000,Hori Fighting Commander 4 (PS4),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "03000000ad1b000001f5000033050000,Hori Pad EX Turbo 2,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000008f0e00001330000010010000,HuiJia SNES Controller,a:b2,b:b1,back:b8,dpdown:+a1,dpleft:-a0,dpright:+a0,dpup:-a1,leftshoulder:b6,rightshoulder:b7,start:b9,x:b3,y:b0,",
        "03000000fd0500000030000000010000,InterAct GoPad I-73000 (Fighting Game Layout),a:b3,b:b4,back:b6,leftx:a0,lefty:a1,rightshoulder:b2,righttrigger:b5,start:b7,x:b0,y:b1,",
        "030000006e0500000320000010010000,JC-U3613M - DirectInput Mode,a:b2,b:b3,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b8,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b9,righttrigger:b7,rightx:a2,righty:a3,start:b11,x:b0,y:b1,",
        "03000000300f00001001000010010000,Jess Tech Dual Analog Rumble Pad,a:b2,b:b3,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b11,righttrigger:b7,rightx:a3,righty:a2,start:b9,x:b0,y:b1,",
        "03000000ba2200002010000001010000,Jess Technology USB Game Controller,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,rightx:a3,righty:a2,start:b9,x:b3,y:b0,",
        "030000006f0e00000103000000020000,Logic3 Controller,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000006d04000019c2000010010000,Logitech Cordless RumblePad 2,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006d04000016c2000010010000,Logitech Dual Action,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006d04000016c2000011010000,Logitech Dual Action,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006d0400001dc2000014400000,Logitech F310 Gamepad (XInput),a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000006d0400001ec2000020200000,Logitech F510 Gamepad (XInput),a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000006d04000019c2000011010000,Logitech F710 Gamepad (DInput),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,", /* Guide button doesn't seem to be sent in DInput mode. */
        "030000006d0400001fc2000005030000,Logitech F710 Gamepad (XInput),a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000006d04000015c2000010010000,Logitech Logitech Extreme 3D,a:b0,b:b4,back:b6,guide:b8,leftshoulder:b9,leftstick:h0.8,leftx:a0,lefty:a1,rightshoulder:b10,rightstick:h0.2,start:b7,x:b2,y:b5,",
        "030000006d04000018c2000010010000,Logitech RumblePad 2,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006d04000011c2000010010000,Logitech WingMan Cordless RumblePad,a:b0,b:b1,back:b2,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b5,leftshoulder:b6,lefttrigger:b9,leftx:a0,lefty:a1,rightshoulder:b7,righttrigger:b10,rightx:a3,righty:a4,start:b8,x:b3,y:b4,",
        "03000000250900006688000000010000,MP-8866 Super Dual Box,a:b2,b:b1,back:b9,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a2,righty:a3,start:b8,x:b3,y:b0,",
        "05000000380700006652000025010000,Mad Catz C.T.R.L.R ,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000380700005032000011010000,Mad Catz FightPad PRO (PS3),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000380700005082000011010000,Mad Catz FightPad PRO (PS4),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "03000000380700008433000011010000,Mad Catz FightStick TE S+ (PS3),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000380700008483000011010000,Mad Catz FightStick TE S+ (PS4),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "03000000ad1b00002ef0000090040000,Mad Catz Fightpad SFxT,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,lefttrigger:a2,rightshoulder:b5,righttrigger:a5,start:b7,x:b2,y:b3,",
        "03000000380700003847000090040000,Mad Catz Wired Xbox 360 Controller (SFIV),a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "03000000380700001647000010040000,Mad Catz Wired Xbox 360 Controller,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "03000000ad1b000016f0000090040000,Mad Catz Xbox 360 Controller,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "03000000380700008034000011010000,Mad Catz fightstick (PS3),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000380700008084000011010000,Mad Catz fightstick (PS4),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "03000000380700001888000010010000,MadCatz PC USB Wired Stick 8818,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000380700003888000010010000,MadCatz PC USB Wired Stick 8838,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:a0,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000790000004418000010010000,Mayflash GameCube Controller,a:b1,b:b2,dpdown:b14,dpleft:b15,dpright:b13,dpup:b12,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b7,righttrigger:a4,rightx:a5,righty:a2,start:b9,x:b0,y:b3,",
        "03000000780000000600000010010000,Microntek USB Joystick,a:b2,b:b1,back:b8,leftshoulder:b6,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,righttrigger:b5,start:b9,x:b3,y:b0,",
        "030000005e0400000e00000000010000,Microsoft SideWinder,a:b0,b:b1,back:b9,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,rightshoulder:b7,start:b8,x:b3,y:b4,",
        "030000005e0400008e02000004010000,Microsoft X-Box 360 pad,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000005e0400008e02000062230000,Microsoft X-Box 360 pad,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000005e040000d102000003020000,Microsoft X-Box One pad v2,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000005e040000d102000001010000,Microsoft X-Box One pad,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000005e0400008502000000010000,Microsoft X-Box pad (Japan),a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b5,leftstick:b8,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b2,rightstick:b9,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b3,y:b4,",
        "030000005e0400008902000021010000,Microsoft X-Box pad v2 (US),a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b5,leftstick:b8,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b2,rightstick:b9,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b3,y:b4,",
        "05000000d6200000ad0d000001000000,Moga Pro,a:b0,b:b1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b7,lefttrigger:a5,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b8,righttrigger:a4,rightx:a2,righty:a3,start:b6,x:b2,y:b3,",
        "030000001008000001e5000010010000,NEXT SNES Controller,a:b2,b:b1,back:b8,dpdown:+a1,dpleft:-a0,dpright:+a0,dpup:-a1,leftshoulder:b4,rightshoulder:b6,start:b9,x:b3,y:b0,",
        "03000000550900001072000011010000,NVIDIA Controller,a:b0,b:b1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b13,leftshoulder:b4,leftstick:b8,lefttrigger:a5,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b9,righttrigger:a4,rightx:a2,righty:a3,start:b7,x:b2,y:b3,",
        "03000000451300000830000010010000,NYKO CORE,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "050000007e0500000920000001000000,Nintendo Switch Pro Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "050000007e0500003003000001000000,Nintendo Wii Remote Pro Controller,a:b0,b:b1,back:b8,dpdown:b14,dpleft:b15,dpright:b16,dpup:b13,guide:b10,leftshoulder:b4,leftstick:b11,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b12,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b2,",
        "05000000010000000100000003000000,Nintendo Wiimote,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b11,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b12,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "030000000d0500000308000010010000,Nostromo n45 Dual Analog Gamepad,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b9,leftshoulder:b4,leftstick:b12,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b11,righttrigger:b7,rightx:a3,righty:a2,start:b10,x:b2,y:b3,",
        "05000000362800000100000002010000,OUYA Game Controller,a:b0,b:b3,dpdown:b9,dpleft:b10,dpright:b11,dpup:b8,guide:b14,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,righttrigger:a5,rightx:a3,righty:a4,x:b1,y:b2,",
        "05000000362800000100000003010000,OUYA Game Controller,a:b0,b:b3,dpdown:b9,dpleft:b10,dpright:b11,dpup:b8,guide:b14,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,righttrigger:a5,rightx:a3,righty:a4,x:b1,y:b2,",
        "030000005e0400000202000000010000,Old Xbox pad,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b5,leftstick:b8,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b2,rightstick:b9,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b3,y:b4,",
        "03000000ff1100003133000010010000,PC Game Controller,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "030000006f0e00006401000001010000,PDP Battlefield One,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "03000000ff1100004133000010010000,PS2 Controller,a:b2,b:b1,back:b8,leftshoulder:b6,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,righttrigger:b5,start:b9,x:b3,y:b0,",
        "03000000341a00003608000011010000,PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000004c0500006802000010010000,PS3 Controller,a:b14,b:b13,back:b0,dpdown:b6,dpleft:b7,dpright:b5,dpup:b4,guide:b16,leftshoulder:b10,leftstick:b1,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b11,rightstick:b2,righttrigger:b9,rightx:a2,righty:a3,start:b3,x:b15,y:b12,",
        "030000004c0500006802000010810000,PS3 Controller,a:b0,b:b1,back:b8,dpdown:b14,dpleft:b15,dpright:b16,dpup:b13,guide:b10,leftshoulder:b4,leftstick:b11,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b12,righttrigger:a5,rightx:a3,righty:a4,start:b9,x:b3,y:b2,",
        "030000004c0500006802000011010000,PS3 Controller,a:b14,b:b13,back:b0,dpdown:b6,dpleft:b7,dpright:b5,dpup:b4,guide:b16,leftshoulder:b10,leftstick:b1,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b11,rightstick:b2,righttrigger:b9,rightx:a2,righty:a3,start:b3,x:b15,y:b12,",
        "030000004c0500006802000011810000,PS3 Controller,a:b0,b:b1,back:b8,dpdown:b14,dpleft:b15,dpright:b16,dpup:b13,guide:b10,leftshoulder:b4,leftstick:b11,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b12,righttrigger:a5,rightx:a3,righty:a4,start:b9,x:b3,y:b2,",
        "030000006f0e00001402000011010000,PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000008f0e00000300000010010000,PS3 Controller,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "050000004c0500006802000000010000,PS3 Controller,a:b14,b:b13,back:b0,dpdown:b6,dpleft:b7,dpright:b5,dpup:b4,guide:b16,leftshoulder:b10,leftstick:b1,lefttrigger:a12,leftx:a0,lefty:a1,rightshoulder:b11,rightstick:b2,righttrigger:a13,rightx:a2,righty:a3,start:b3,x:b15,y:b12,",
        "050000004c0500006802000000800000,PS3 Controller,a:b0,b:b1,back:b8,dpdown:b14,dpleft:b15,dpright:b16,dpup:b13,guide:b10,leftshoulder:b4,leftstick:b11,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b12,righttrigger:b7,rightx:a3,righty:a4,start:b9,x:b3,y:b2,",
        "050000004c0500006802000000810000,PS3 Controller,a:b0,b:b1,back:b8,dpdown:b14,dpleft:b15,dpright:b16,dpup:b13,guide:b10,leftshoulder:b4,leftstick:b11,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b12,righttrigger:a5,rightx:a3,righty:a4,start:b9,x:b3,y:b2,",
        "05000000504c415953544154494f4e00,PS3 Controller,a:b14,b:b13,back:b0,dpdown:b6,dpleft:b7,dpright:b5,dpup:b4,guide:b16,leftshoulder:b10,leftstick:b1,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b11,rightstick:b2,righttrigger:b9,rightx:a2,righty:a3,start:b3,x:b15,y:b12,",
        "060000004c0500006802000000010000,PS3 Controller,a:b14,b:b13,back:b0,dpdown:b6,dpleft:b7,dpright:b5,dpup:b4,guide:b16,leftshoulder:b10,leftstick:b1,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b11,rightstick:b2,righttrigger:b9,rightx:a2,righty:a3,start:b3,x:b15,y:b12,",
        "030000004c050000a00b000011010000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000004c050000a00b000011810000,PS4 Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b11,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b12,righttrigger:a5,rightx:a3,righty:a4,start:b9,x:b3,y:b2,",
        "030000004c050000c405000011010000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000004c050000c405000011810000,PS4 Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b11,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b12,righttrigger:a5,rightx:a3,righty:a4,start:b9,x:b3,y:b2,",
        "030000004c050000cc09000000010000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000004c050000cc09000011010000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000004c050000cc09000011810000,PS4 Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b11,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b12,righttrigger:a5,rightx:a3,righty:a4,start:b9,x:b3,y:b2,",
        "050000004c050000c405000000010000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "050000004c050000c405000000810000,PS4 Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b11,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b12,righttrigger:a5,rightx:a3,righty:a4,start:b9,x:b3,y:b2,",
        "050000004c050000cc09000000010000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "050000004c050000cc09000000810000,PS4 Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b11,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b12,righttrigger:a5,rightx:a3,righty:a4,start:b9,x:b3,y:b2,",
        "050000004c050000cc09000001800000,PS4 Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b11,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b12,righttrigger:a5,rightx:a3,righty:a4,start:b9,x:b3,y:b2,",
        "03000000c62400000053000000010000,PowerA,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "03000000300f00001211000011010000,QanBa Arcade JoyStick,a:b2,b:b0,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b5,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,righttrigger:b6,start:b9,x:b1,y:b3,",
        "030000008916000001fd000024010000,Razer Onza Classic Edition,a:b0,b:b1,back:b6,dpdown:b14,dpleft:b11,dpright:b12,dpup:b13,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000008916000000fd000024010000,Razer Onza Tournament Edition,a:b0,b:b1,back:b6,dpdown:b14,dpleft:b11,dpright:b12,dpup:b13,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "03000000321500000204000011010000,Razer Panthera (PS3),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000321500000104000011010000,Razer Panthera (PS4),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "03000000321500000010000011010000,Razer RAIJU,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000008916000000fe000024010000,Razer Sabertooth,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "03000000c6240000045d000024010000,Razer Sabertooth,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "03000000c6240000045d000025010000,Razer Sabertooth,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "03000000321500000009000011010000,Razer Serval,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a5,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a4,rightx:a2,righty:a3,start:b7,x:b2,y:b3,",
        "050000003215000000090000163a0000,Razer Serval,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a5,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a4,rightx:a2,righty:a3,start:b7,x:b2,y:b3,",
        "0300000032150000030a000001010000,Razer Wildcat,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "0300000000f000000300000000010000,RetroPad,a:b1,b:b5,back:b2,leftshoulder:b6,leftx:a0,lefty:a1,rightshoulder:b7,start:b3,x:b0,y:b4,",
        "03000000790000001100000010010000,Retrolink SNES Controller,a:b2,b:b1,back:b8,dpdown:+a1,dpleft:-a0,dpright:+a0,dpup:-a1,leftshoulder:b4,rightshoulder:b5,start:b9,x:b3,y:b0,",
        "030000006b140000010d000011010000,Revolution Pro Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000006f0e00001e01000011010000,Rock Candy PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000006f0e00004601000001010000,Rock Candy Xbox One Controller,a:b0,b:b1,back:b6,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000006f0e00001f01000000010000,Rock Candy,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "03000000632500007505000010010000,SHANWAN PS3/PC Gamepad,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000341a00000908000010010000,SL-6566,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "03000000a306000023f6000011010000,Saitek Cyborg V.1 Game Pad,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a4,start:b9,x:b0,y:b3,",
        "03000000a30600000cff000010010000,Saitek P2500 Force Rumble Pad,a:b2,b:b3,back:b11,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b8,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b9,righttrigger:b7,rightx:a3,righty:a2,x:b0,y:b1,",
        "03000000a30600000c04000011010000,Saitek P2900 Wireless Pad,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b9,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a3,righty:a2,start:b12,x:b0,y:b3,",
        "03000000a30600000901000000010000,Saitek P880,a:b2,b:b3,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b8,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b9,righttrigger:b7,rightx:a3,righty:a2,x:b0,y:b1,",
        "03000000a30600000b04000000010000,Saitek P990 Dual Analog Pad,a:b1,b:b2,back:b9,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a3,righty:a2,start:b8,x:b0,y:b3,",
        "03000000a306000018f5000010010000,Saitek PLC Saitek P3200 Rumble Pad,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a3,righty:a4,start:b9,x:b0,y:b3,",
        "03000000c01600008704000011010000,Serial/Keyboard/Mouse/Joystick,a:b12,b:b10,back:b4,dpdown:b2,dpleft:b3,dpright:b1,dpup:b0,leftshoulder:b9,leftstick:b14,lefttrigger:b6,leftx:a1,lefty:a0,rightshoulder:b8,rightstick:b15,righttrigger:b7,rightx:a2,righty:a3,start:b5,x:b13,y:b11,",
        "03000000f025000021c1000010010000,ShanWan Gioteck PS3 Wired Controller,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000632500002305000010010000,ShanWan USB Gamepad,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b3,y:b0,",
        "03000000250900000500000000010000,Sony PS2 pad with SmartJoy adapter,a:b2,b:b1,back:b9,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a2,righty:a3,start:b8,x:b3,y:b0,",
        "030000005e0400008e02000020200000,SpeedLink XEOX Pro Analog Gamepad pad,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000005e0400008e02000073050000,Speedlink TORID Wireless Gamepad,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "03000000de2800000112000001000000,Steam Controller,a:b0,b:b1,back:b6,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:a3,start:b7,x:b2,y:b3,",
        "03000000de2800000211000001000000,Steam Controller,a:b0,b:b1,back:b6,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:a3,start:b7,x:b2,y:b3,",
        "03000000de2800004211000001000000,Steam Controller,a:b0,b:b1,back:b6,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:a3,start:b7,x:b2,y:b3,",
        "03000000de280000fc11000001000000,Steam Controller,a:b0,b:b1,back:b6,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "05000000de2800000212000001000000,Steam Controller,a:b0,b:b1,back:b6,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:a3,start:b7,x:b2,y:b3,",
        "05000000de2800000511000001000000,Steam Controller,a:b0,b:b1,back:b6,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:a3,start:b7,x:b2,y:b3,",
        "05000000de2800000611000001000000,Steam Controller,a:b0,b:b1,back:b6,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:a3,start:b7,x:b2,y:b3,",
        "03000000de280000ff11000001000000,Steam Virtual Gamepad,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "03000000ad1b000038f0000090040000,Street Fighter IV FightStick TE,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "03000000666600000488000000010000,Super Joy Box 5 Pro,a:b2,b:b1,back:b9,dpdown:b14,dpleft:b15,dpright:b13,dpup:b12,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a2,righty:a3,start:b8,x:b3,y:b0,",
        "0300000000f00000f100000000010000,Super RetroPort,a:b1,b:b5,back:b2,leftshoulder:b6,leftx:a0,lefty:a1,rightshoulder:b7,start:b3,x:b0,y:b4,",
        "030000004f04000020b3000010010000,Thrustmaster 2 in 1 DT,a:b0,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b1,y:b3,",
        "030000004f04000015b3000010010000,Thrustmaster Dual Analog 4,a:b0,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b1,y:b3,",
        "030000004f04000023b3000000010000,Thrustmaster Dual Trigger 3-in-1,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "030000004f04000000b3000010010000,Thrustmaster Firestorm Dual Power,a:b0,b:b2,back:b9,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b11,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b12,righttrigger:b7,rightx:a2,righty:a3,start:b10,x:b1,y:b3,",
        "030000004f04000009d0000000010000,Thrustmaster Run N Drive Wireless PS3,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "030000004f04000008d0000000010000,Thrustmaster Run N Drive Wireless,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "03000000bd12000015d0000010010000,Tomee SNES USB Controller,a:b2,b:b1,back:b8,dpdown:+a1,dpleft:-a0,dpright:+a0,dpup:-a1,leftshoulder:b4,rightshoulder:b5,start:b9,x:b3,y:b0,",
        "03000000d814000007cd000011010000,Toodles 2008 Chimp PC/PS3,a:b0,b:b1,back:b8,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,start:b9,x:b3,y:b2,",
        "03000000100800000100000010010000,Twin USB PS2 Adapter,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a3,righty:a2,start:b9,x:b3,y:b0,",
        "03000000100800000300000010010000,USB Gamepad,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a3,righty:a2,start:b9,x:b3,y:b0,",
        "03000000790000001100000000010000,USB Gamepad1,a:b2,b:b1,back:b8,dpdown:a0,dpleft:a1,dpright:a2,dpup:a4,start:b9,",
        "03000000790000000600000007010000,USB gamepad,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a3,righty:a4,start:b9,x:b3,y:b0,",
        "05000000ac0500003232000001000000,VR-BOX,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a3,righty:a2,start:b9,x:b2,y:b3,",
        "030000005e0400008e02000010010000,X360 Controller,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000005e0400008e02000014010000,X360 Controller,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000005e0400001907000000010000,X360 Wireless Controller,a:b0,b:b1,back:b6,dpdown:b14,dpleft:b11,dpright:b12,dpup:b13,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000005e0400009102000007010000,X360 Wireless Controller,a:b0,b:b1,back:b6,dpdown:b14,dpleft:b11,dpright:b12,dpup:b13,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000005e040000a102000000010000,X360 Wireless Controller,a:b0,b:b1,back:b6,dpdown:b14,dpleft:b11,dpright:b12,dpup:b13,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "030000005e040000a102000007010000,X360 Wireless Controller,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "03000000450c00002043000010010000,XEOX Gamepad SL-6556-BK,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
        "xinput,XInput Controller,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "0000000058626f782033363020576900,Xbox 360 Wireless Controller,a:b0,b:b1,back:b14,dpdown:b11,dpleft:b12,dpright:b13,dpup:b10,guide:b7,leftshoulder:b4,leftstick:b8,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b9,righttrigger:a5,rightx:a3,righty:a4,start:b6,x:b2,y:b3,",
        "030000005e040000a102000014010000,Xbox 360 Wireless Receiver (XBOX),a:b0,b:b1,back:b6,dpdown:b14,dpleft:b11,dpright:b12,dpup:b13,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "0000000058626f782047616d65706100,Xbox Gamepad (userspace driver),a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a5,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a4,rightx:a2,righty:a3,start:b7,x:b2,y:b3,",
        "050000005e040000e002000003090000,Xbox One Wireless Controller,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b8,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b9,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "050000005e040000fd02000003090000,Xbox One Wireless Controller,a:b0,b:b1,back:b15,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b16,leftshoulder:b6,leftstick:b13,lefttrigger:a5,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:a4,rightx:a2,righty:a3,start:b11,x:b3,y:b4,",
        "030000005e040000ea02000001030000,Xbox One Wireless Controller (Model 1708),a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,",
        "05000000172700004431000029010000,XiaoMi Game Controller,a:b0,b:b1,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b20,leftshoulder:b6,leftstick:b13,lefttrigger:a7,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,righttrigger:a6,rightx:a2,righty:a5,start:b11,x:b3,y:b4,",
        "03000000c0160000e105000001010000,Xin-Mo Xin-Mo Dual Arcade,a:b4,b:b3,back:b6,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b9,leftshoulder:b2,leftx:a0,lefty:a1,rightshoulder:b5,start:b7,x:b1,y:b0,",
        "03000000120c0000100e000011010000,ZEROPLUS P4 Gamepad,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
        "03000000666600006706000000010000,boom PSX to PC Converter,a:b2,b:b1,back:b8,dpdown:b14,dpleft:b15,dpright:b13,dpup:b12,leftshoulder:b6,leftstick:b9,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b10,righttrigger:b5,rightx:a2,righty:a3,start:b11,x:b3,y:b0,",
        "030000000d0f00000d00000000010000,hori,a:b0,b:b6,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b3,leftx:b4,lefty:b5,rightshoulder:b7,start:b9,x:b1,y:b2,",
        "03000000830500006020000010010000,iBuffalo SNES Controller,a:b1,b:b0,back:b6,dpdown:+a1,dpleft:-a0,dpright:+a0,dpup:-a1,leftshoulder:b4,rightshoulder:b5,start:b7,x:b3,y:b2,",
        "050000006964726f69643a636f6e0000,idroid:con,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,",
        "03000000b50700001503000010010000,impact,a:b2,b:b3,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b5,leftx:a0,lefty:a1,rightshoulder:b6,rightstick:b11,righttrigger:b7,rightx:a3,righty:a2,start:b9,x:b0,y:b1,",
        "030000009b2800000300000001010000,raphnet.net 4nes4snes v1.5,a:b0,b:b4,back:b2,leftshoulder:b6,leftx:a0,lefty:a1,rightshoulder:b7,start:b3,x:b1,y:b5,",
#endif
#if defined(__ANDROID__)
        "05000000d6020000e5890000dfff3f00,GPD XD Plus,a:b0,b:b1,back:b4,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,leftshoulder:b9,leftstick:b7,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b10,rightstick:b8,righttrigger:a4,rightx:a2,righty:a5,start:b6,x:b2,y:b3,",
        "05000000bc20000000550000ffff3f00,GameSir G3w,a:b0,b:b1,back:b4,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,leftshoulder:b9,leftstick:b7,lefttrigger:a5,leftx:a0,lefty:a1,rightshoulder:b10,rightstick:b8,righttrigger:a4,rightx:a2,righty:a3,start:b6,x:b2,y:b3,",
        "050000005509000003720000cf7f3f00,NVIDIA Controller v01.01,a:b0,b:b1,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,leftshoulder:b9,leftstick:b7,lefttrigger:a4,leftx:a0,lefty:a1,rightshoulder:b10,rightstick:b8,righttrigger:a5,rightx:a2,righty:a3,start:b6,x:b2,y:b3,",
        "050000005509000010720000ffff3f00,NVIDIA Controller v01.03,a:b0,b:b1,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,leftshoulder:b9,leftstick:b7,lefttrigger:a4,leftx:a0,lefty:a1,rightshoulder:b10,rightstick:b8,righttrigger:a5,rightx:a2,righty:a3,start:b6,x:b2,y:b3,",
        "050000007e05000009200000ffff0f00,Nintendo Switch Pro Controller,a:b0,b:b1,back:b15,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b5,leftshoulder:b3,leftstick:b4,lefttrigger:b9,leftx:a0,lefty:a1,rightshoulder:b18,rightstick:b6,righttrigger:b10,rightx:a2,righty:a3,start:b16,x:b17,y:b2,", /* Extremely slow in Bluetooth mode on Android */
        "050000004c05000068020000dfff3f00,PS3 Controller,a:b0,b:b1,back:b4,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,leftshoulder:b9,leftstick:b7,lefttrigger:a4,leftx:a0,lefty:a1,rightshoulder:b10,rightstick:b8,righttrigger:a5,rightx:a2,righty:a3,start:b6,x:b2,y:b3,",
        "050000004c050000c4050000fffe3f00,PS4 Controller,a:b1,b:b17,back:b15,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b5,leftshoulder:b3,leftstick:b4,lefttrigger:+a3,leftx:a0,lefty:a1,rightshoulder:b18,rightstick:b6,righttrigger:+a4,rightx:a2,righty:a5,start:b16,x:b0,y:b2,",
        "050000004c050000cc090000fffe3f00,PS4 Controller,a:b1,b:b17,back:b15,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b5,leftshoulder:b3,leftstick:b4,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b18,rightstick:b6,righttrigger:a4,rightx:a2,righty:a5,start:b16,x:b0,y:b2,",
        "050000003215000000090000bf7f3f00,Razer Serval,a:b0,b:b1,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b5,leftshoulder:b9,leftstick:b7,lefttrigger:a4,leftx:a0,lefty:a1,rightshoulder:b10,rightstick:b8,righttrigger:a5,rightx:a2,righty:a3,x:b2,y:b3,",
        "05000000de2800000511000001000000,Steam Controller,a:b0,b:b1,back:b6,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:a3,start:b7,x:b2,y:b3,",
        "05000000de2800000611000001000000,Steam Controller,a:b0,b:b1,back:b6,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:a3,start:b7,x:b2,y:b3,",
        "050000005e040000e00200000ffe3f00,Xbox One Wireless Controller,a:b0,b:b1,back:b9,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,leftshoulder:b3,leftstick:b15,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b18,rightstick:b16,righttrigger:a5,rightx:a3,righty:a4,start:b10,x:b17,y:b2,",
        "050000005e040000fd020000ffff3f00,Xbox One Wireless Controller,a:b0,b:b1,back:b4,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,leftshoulder:b9,leftstick:b7,lefttrigger:a5,leftx:a0,lefty:a1,rightshoulder:b10,rightstick:b8,righttrigger:a4,rightx:a2,righty:a3,start:b6,x:b2,y:b3,",
        "050000005e04000091020000ff073f00,Xbox Wireless Controller,a:b0,b:b1,back:b4,guide:b5,leftshoulder:b9,leftstick:b7,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b10,rightstick:b8,righttrigger:a5,rightx:a3,righty:a4,start:b6,x:b2,y:b3,", /* The DPAD doesn't seem to work on this controller on Android TV? */
#endif
#if defined(SDL_JOYSTICK_MFI)
        "05000000ac0500000100000000006d01,*,a:b0,b:b1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b6,leftshoulder:b4,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:a5,rightx:a3,righty:a4,x:b2,y:b3,",
        "05000000ac0500000200000000006d02,*,a:b0,b:b1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b6,leftshoulder:b4,rightshoulder:b5,x:b2,y:b3,",
        "05000000ac0500000300000000006d03,Remote,a:b0,b:b2,leftx:a0,lefty:a1,",
        "05000000de2800000511000001000000,Steam Controller,a:b0,b:b1,back:b6,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:a3,start:b7,x:b2,y:b3,",
        "05000000de2800000611000001000000,Steam Controller,a:b0,b:b1,back:b6,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:a3,start:b7,x:b2,y:b3,",
#endif
#if defined(SDL_JOYSTICK_EMSCRIPTEN)
        "default,Standard Gamepad,a:b0,b:b1,back:b8,dpdown:b13,dpleft:b14,dpright:b15,dpup:b12,guide:b16,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,",
#endif
        "hidapi,*,a:b0,b:b1,back:b4,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b5,leftshoulder:b9,leftstick:b7,lefttrigger:a4,leftx:a0,lefty:a1,rightshoulder:b10,rightstick:b8,righttrigger:a5,rightx:a2,righty:a3,start:b6,x:b2,y:b3,",
        NULL};

/*
 * Initialize the game controller system, mostly load our DB of controller config mappings
 */
int
SDL_GameControllerInitMappings(void)
{
    char szControllerMapPath[1024];
    int i = 0;
    const char *pMappingString = NULL;
    pMappingString = s_ControllerMappings[i];
    while (pMappingString) {
        SDL_PrivateGameControllerAddMapping(pMappingString, SDL_CONTROLLER_MAPPING_PRIORITY_DEFAULT);

        i++;
        pMappingString = s_ControllerMappings[i];
    }

    if (SDL_GetControllerMappingFilePath(NULL)) {
        SDL_GameControllerAddMappingsFromFile(szControllerMapPath);
    }

    /* load in any user supplied config */
    SDL_GameControllerLoadHints();

    SDL_AddHintCallback(SDL_HINT_GAMECONTROLLER_IGNORE_DEVICES, NULL, NULL);
    SDL_AddHintCallback(SDL_HINT_GAMECONTROLLER_IGNORE_DEVICES_EXCEPT, NULL, NULL);

    return (0);
}

int
SDL_GameControllerInit(void)
{
    int i;

    /* watch for joy events and fire controller ones if needed */
    SDL_AddEventWatch(SDL_GameControllerEventWatcher, NULL);

    /* Send added events for controllers currently attached */
    for (i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            SDL_Event deviceevent;
            deviceevent.type = SDL_CONTROLLERDEVICEADDED;
            deviceevent.cdevice.which = i;
            SDL_PushEvent(&deviceevent);
        }
    }

    return (0);
}


/*
 * Get the implementation dependent name of a controller
 */
const char *
SDL_GameControllerNameForIndex(int device_index)
{
    ControllerMapping_t *pSupportedController = SDL_PrivateGetControllerMapping(device_index);
    if (pSupportedController) {
        if (SDL_strcmp(pSupportedController->name, "*") == 0) {
            return SDL_JoystickNameForIndex(device_index);
        } else {
            return pSupportedController->name;
        }
    }
    return NULL;
}


/**
 *  Get the mapping of a game controller.
 *  This can be called before any controllers are opened.
 *  If no mapping can be found, this function returns NULL.
 */
char *
SDL_GameControllerMappingForDeviceIndex(int joystick_index)
{
    char *pMappingString = NULL;
    ControllerMapping_t *mapping;

    SDL_LockJoysticks();
    mapping = SDL_PrivateGetControllerMapping(joystick_index);
    if (mapping) {
        SDL_JoystickGUID guid;
        char pchGUID[33];
        size_t needed;
        guid = SDL_JoystickGetDeviceGUID(joystick_index);
        SDL_JoystickGetGUIDString(guid, pchGUID, sizeof(pchGUID));
        /* allocate enough memory for GUID + ',' + name + ',' + mapping + \0 */
        needed = SDL_strlen(pchGUID) + 1 + SDL_strlen(mapping->name) + 1 + SDL_strlen(mapping->mapping) + 1;
        pMappingString = SDL_malloc(needed);
        if (!pMappingString) {
            SDL_OutOfMemory();
            SDL_UnlockJoysticks();
            return NULL;
        }
        SDL_snprintf(pMappingString, needed, "%s,%s,%s", pchGUID, mapping->name, mapping->mapping);
    }
    SDL_UnlockJoysticks();
    return pMappingString;
}


/*
 * Return 1 if the joystick with this name and GUID is a supported controller
 */
SDL_bool
SDL_IsGameControllerNameAndGUID(const char *name, SDL_JoystickGUID guid)
{
    ControllerMapping_t *pSupportedController = SDL_PrivateGetControllerMappingForNameAndGUID(name, guid);
    if (pSupportedController) {
        return SDL_TRUE;
    }
    return SDL_FALSE;
}

/*
 * Return 1 if the joystick at this device index is a supported controller
 */
SDL_bool
SDL_IsGameController(int device_index)
{
    ControllerMapping_t *pSupportedController = SDL_PrivateGetControllerMapping(device_index);
    if (pSupportedController) {
        return SDL_TRUE;
    }
    return SDL_FALSE;
}

/*
 * Return 1 if the game controller should be ignored by SDL
 */
SDL_bool SDL_ShouldIgnoreGameController(const char *name, SDL_JoystickGUID guid)
{
    int i;
    Uint16 vendor;
    Uint16 product;
    Uint16 version;
    Uint32 vidpid;

    if (SDL_allowed_controllers.num_entries == 0 &&
        SDL_ignored_controllers.num_entries == 0) {
        return SDL_FALSE;
    }

    SDL_GetJoystickGUIDInfo(guid, &vendor, &product, &version);

    if (SDL_GetHintBoolean("SDL_GAMECONTROLLER_ALLOW_STEAM_VIRTUAL_GAMEPAD", SDL_FALSE)) {
        /* We shouldn't ignore Steam's virtual gamepad since it's using the hints to filter out the real controllers so it can remap input for the virtual controller */
        SDL_bool bSteamVirtualGamepad = SDL_FALSE;
#if defined(__LINUX__)
        bSteamVirtualGamepad = (vendor == 0x28DE && product == 0x11FF);
#elif defined(__MACOSX__)
        bSteamVirtualGamepad = (vendor == 0x045E && product == 0x028E && version == 1);
#elif defined(__WIN32__)
        /* We can't tell on Windows, but Steam will block others in input hooks */
        bSteamVirtualGamepad = SDL_TRUE;
#endif
        if (bSteamVirtualGamepad) {
            return SDL_FALSE;
        }
    }

    vidpid = MAKE_VIDPID(vendor, product);

    if (SDL_allowed_controllers.num_entries > 0) {
        for (i = 0; i < SDL_allowed_controllers.num_entries; ++i) {
            if (vidpid == SDL_allowed_controllers.entries[i]) {
                return SDL_FALSE;
            }
        }
        return SDL_TRUE;
    } else {
        for (i = 0; i < SDL_ignored_controllers.num_entries; ++i) {
            if (vidpid == SDL_ignored_controllers.entries[i]) {
                return SDL_TRUE;
            }
        }
        return SDL_FALSE;
    }
}

/*
 * Open a controller for use - the index passed as an argument refers to
 * the N'th controller on the system.  This index is the value which will
 * identify this controller in future controller events.
 *
 * This function returns a controller identifier, or NULL if an error occurred.
 */
SDL_GameController *
SDL_GameControllerOpen(int device_index)
{
    SDL_JoystickID instance_id;
    SDL_GameController *gamecontroller;
    SDL_GameController *gamecontrollerlist;
    ControllerMapping_t *pSupportedController = NULL;

    SDL_LockJoysticks();

    gamecontrollerlist = SDL_gamecontrollers;
    /* If the controller is already open, return it */
    instance_id = SDL_JoystickGetDeviceInstanceID(device_index);
    while (gamecontrollerlist) {
        if (instance_id == gamecontrollerlist->joystick->instance_id) {
            gamecontroller = gamecontrollerlist;
            ++gamecontroller->ref_count;
            SDL_UnlockJoysticks();
            return (gamecontroller);
        }
        gamecontrollerlist = gamecontrollerlist->next;
    }

    /* Find a controller mapping */
    pSupportedController =  SDL_PrivateGetControllerMapping(device_index);
    if (!pSupportedController) {
        SDL_SetError("Couldn't find mapping for device (%d)", device_index);
        SDL_UnlockJoysticks();
        return NULL;
    }

    /* Create and initialize the controller */
    gamecontroller = (SDL_GameController *) SDL_calloc(1, sizeof(*gamecontroller));
    if (gamecontroller == NULL) {
        SDL_OutOfMemory();
        SDL_UnlockJoysticks();
        return NULL;
    }

    gamecontroller->joystick = SDL_JoystickOpen(device_index);
    if (!gamecontroller->joystick) {
        SDL_free(gamecontroller);
        SDL_UnlockJoysticks();
        return NULL;
    }

    if (gamecontroller->joystick->naxes) {
        gamecontroller->last_match_axis = (SDL_ExtendedGameControllerBind **)SDL_calloc(gamecontroller->joystick->naxes, sizeof(*gamecontroller->last_match_axis));
        if (!gamecontroller->last_match_axis) {
            SDL_OutOfMemory();
            SDL_JoystickClose(gamecontroller->joystick);
            SDL_free(gamecontroller);
            SDL_UnlockJoysticks();
            return NULL;
        }
    }
    if (gamecontroller->joystick->nhats) {
        gamecontroller->last_hat_mask = (Uint8 *)SDL_calloc(gamecontroller->joystick->nhats, sizeof(*gamecontroller->last_hat_mask));
        if (!gamecontroller->last_hat_mask) {
            SDL_OutOfMemory();
            SDL_JoystickClose(gamecontroller->joystick);
            SDL_free(gamecontroller->last_match_axis);
            SDL_free(gamecontroller);
            SDL_UnlockJoysticks();
            return NULL;
        }
    }

    SDL_PrivateLoadButtonMapping(gamecontroller, pSupportedController->name, pSupportedController->mapping);

    /* Add the controller to list */
    ++gamecontroller->ref_count;
    /* Link the controller in the list */
    gamecontroller->next = SDL_gamecontrollers;
    SDL_gamecontrollers = gamecontroller;

    SDL_UnlockJoysticks();

    return (gamecontroller);
}

/*
 * Manually pump for controller updates.
 */
void
SDL_GameControllerUpdate(void)
{
    /* Just for API completeness; the joystick API does all the work. */
    SDL_JoystickUpdate();
}

/*
 * Get the current state of an axis control on a controller
 */
Sint16
SDL_GameControllerGetAxis(SDL_GameController * gamecontroller, SDL_GameControllerAxis axis)
{
    int i;

    if (!gamecontroller)
        return 0;

    for (i = 0; i < gamecontroller->num_bindings; ++i) {
        SDL_ExtendedGameControllerBind *binding = &gamecontroller->bindings[i];
        if (binding->outputType == SDL_CONTROLLER_BINDTYPE_AXIS && binding->output.axis.axis == axis) {
            int value = 0;
            SDL_bool valid_input_range;
            SDL_bool valid_output_range;

            if (binding->inputType == SDL_CONTROLLER_BINDTYPE_AXIS) {
                value = SDL_JoystickGetAxis(gamecontroller->joystick, binding->input.axis.axis);
                if (binding->input.axis.axis_min < binding->input.axis.axis_max) {
                    valid_input_range = (value >= binding->input.axis.axis_min && value <= binding->input.axis.axis_max);
                } else {
                    valid_input_range = (value >= binding->input.axis.axis_max && value <= binding->input.axis.axis_min);
                }
                if (valid_input_range) {
                    if (binding->input.axis.axis_min != binding->output.axis.axis_min || binding->input.axis.axis_max != binding->output.axis.axis_max) {
                        float normalized_value = (float)(value - binding->input.axis.axis_min) / (binding->input.axis.axis_max - binding->input.axis.axis_min);
                        value = binding->output.axis.axis_min + (int)(normalized_value * (binding->output.axis.axis_max - binding->output.axis.axis_min));
                    }
                }
            } else if (binding->inputType == SDL_CONTROLLER_BINDTYPE_BUTTON) {
                value = SDL_JoystickGetButton(gamecontroller->joystick, binding->input.button);
                if (value == SDL_PRESSED) {
                    value = binding->output.axis.axis_max;
                }
            } else if (binding->inputType == SDL_CONTROLLER_BINDTYPE_HAT) {
                int hat_mask = SDL_JoystickGetHat(gamecontroller->joystick, binding->input.hat.hat);
                if (hat_mask & binding->input.hat.hat_mask) {
                    value = binding->output.axis.axis_max;
                }
            }

            if (binding->output.axis.axis_min < binding->output.axis.axis_max) {
                valid_output_range = (value >= binding->output.axis.axis_min && value <= binding->output.axis.axis_max);
            } else {
                valid_output_range = (value >= binding->output.axis.axis_max && value <= binding->output.axis.axis_min);
            }
            /* If the value is zero, there might be another binding that makes it non-zero */
            if (value != 0 && valid_output_range) {
                return (Sint16)value;
            }
        }
    }
    return 0;
}

/*
 * Get the current state of a button on a controller
 */
Uint8
SDL_GameControllerGetButton(SDL_GameController * gamecontroller, SDL_GameControllerButton button)
{
    int i;

    if (!gamecontroller)
        return 0;

    for (i = 0; i < gamecontroller->num_bindings; ++i) {
        SDL_ExtendedGameControllerBind *binding = &gamecontroller->bindings[i];
        if (binding->outputType == SDL_CONTROLLER_BINDTYPE_BUTTON && binding->output.button == button) {
            if (binding->inputType == SDL_CONTROLLER_BINDTYPE_AXIS) {
                SDL_bool valid_input_range;

                int value = SDL_JoystickGetAxis(gamecontroller->joystick, binding->input.axis.axis);
                int threshold = binding->input.axis.axis_min + (binding->input.axis.axis_max - binding->input.axis.axis_min) / 2;
                if (binding->input.axis.axis_min < binding->input.axis.axis_max) {
                    valid_input_range = (value >= binding->input.axis.axis_min && value <= binding->input.axis.axis_max);
                    if (valid_input_range) {
                        return (value >= threshold) ? SDL_PRESSED : SDL_RELEASED;
                    }
                } else {
                    valid_input_range = (value >= binding->input.axis.axis_max && value <= binding->input.axis.axis_min);
                    if (valid_input_range) {
                        return (value <= threshold) ? SDL_PRESSED : SDL_RELEASED;
                    }
                }
            } else if (binding->inputType == SDL_CONTROLLER_BINDTYPE_BUTTON) {
                return SDL_JoystickGetButton(gamecontroller->joystick, binding->input.button);
            } else if (binding->inputType == SDL_CONTROLLER_BINDTYPE_HAT) {
                int hat_mask = SDL_JoystickGetHat(gamecontroller->joystick, binding->input.hat.hat);
                return (hat_mask & binding->input.hat.hat_mask) ? SDL_PRESSED : SDL_RELEASED;
            }
        }
    }
    return SDL_RELEASED;
}

const char *
SDL_GameControllerName(SDL_GameController * gamecontroller)
{
    if (!gamecontroller)
        return NULL;

    if (SDL_strcmp(gamecontroller->name, "*") == 0) {
        return SDL_JoystickName(SDL_GameControllerGetJoystick(gamecontroller));
    } else {
        return gamecontroller->name;
    }
}

int
SDL_GameControllerGetPlayerIndex(SDL_GameController *gamecontroller)
{
    return SDL_JoystickGetPlayerIndex(SDL_GameControllerGetJoystick(gamecontroller));
}

Uint16
SDL_GameControllerGetVendor(SDL_GameController * gamecontroller)
{
    return SDL_JoystickGetVendor(SDL_GameControllerGetJoystick(gamecontroller));
}

Uint16
SDL_GameControllerGetProduct(SDL_GameController * gamecontroller)
{
    return SDL_JoystickGetProduct(SDL_GameControllerGetJoystick(gamecontroller));
}

Uint16
SDL_GameControllerGetProductVersion(SDL_GameController * gamecontroller)
{
    return SDL_JoystickGetProductVersion(SDL_GameControllerGetJoystick(gamecontroller));
}

/*
 * Return if the controller in question is currently attached to the system,
 *  \return 0 if not plugged in, 1 if still present.
 */
SDL_bool
SDL_GameControllerGetAttached(SDL_GameController * gamecontroller)
{
    if (!gamecontroller)
        return SDL_FALSE;

    return SDL_JoystickGetAttached(gamecontroller->joystick);
}

/*
 * Get the joystick for this controller
 */
SDL_Joystick *SDL_GameControllerGetJoystick(SDL_GameController * gamecontroller)
{
    if (!gamecontroller)
        return NULL;

    return gamecontroller->joystick;
}


/*
 * Find the SDL_GameController that owns this instance id
 */
SDL_GameController *
SDL_GameControllerFromInstanceID(SDL_JoystickID joyid)
{
    SDL_GameController *gamecontroller;

    SDL_LockJoysticks();
    gamecontroller = SDL_gamecontrollers;
    while (gamecontroller) {
        if (gamecontroller->joystick->instance_id == joyid) {
            SDL_UnlockJoysticks();
            return gamecontroller;
        }
        gamecontroller = gamecontroller->next;
    }
    SDL_UnlockJoysticks();
    return NULL;
}


/*
 * Get the SDL joystick layer binding for this controller axis mapping
 */
SDL_GameControllerButtonBind SDL_GameControllerGetBindForAxis(SDL_GameController * gamecontroller, SDL_GameControllerAxis axis)
{
    int i;
    SDL_GameControllerButtonBind bind;
    SDL_zero(bind);

    if (!gamecontroller || axis == SDL_CONTROLLER_AXIS_INVALID)
        return bind;

    for (i = 0; i < gamecontroller->num_bindings; ++i) {
        SDL_ExtendedGameControllerBind *binding = &gamecontroller->bindings[i];
        if (binding->outputType == SDL_CONTROLLER_BINDTYPE_AXIS && binding->output.axis.axis == axis) {
            bind.bindType = binding->inputType;
            if (binding->inputType == SDL_CONTROLLER_BINDTYPE_AXIS) {
                /* FIXME: There might be multiple axes bound now that we have axis ranges... */
                bind.value.axis = binding->input.axis.axis;
            } else if (binding->inputType == SDL_CONTROLLER_BINDTYPE_BUTTON) {
                bind.value.button = binding->input.button;
            } else if (binding->inputType == SDL_CONTROLLER_BINDTYPE_HAT) {
                bind.value.hat.hat = binding->input.hat.hat;
                bind.value.hat.hat_mask = binding->input.hat.hat_mask;
            }
            break;
        }
    }
    return bind;
}


/*
 * Get the SDL joystick layer binding for this controller button mapping
 */
SDL_GameControllerButtonBind SDL_GameControllerGetBindForButton(SDL_GameController * gamecontroller, SDL_GameControllerButton button)
{
    int i;
    SDL_GameControllerButtonBind bind;
    SDL_zero(bind);

    if (!gamecontroller || button == SDL_CONTROLLER_BUTTON_INVALID)
        return bind;

    for (i = 0; i < gamecontroller->num_bindings; ++i) {
        SDL_ExtendedGameControllerBind *binding = &gamecontroller->bindings[i];
        if (binding->outputType == SDL_CONTROLLER_BINDTYPE_BUTTON && binding->output.button == button) {
            bind.bindType = binding->inputType;
            if (binding->inputType == SDL_CONTROLLER_BINDTYPE_AXIS) {
                bind.value.axis = binding->input.axis.axis;
            } else if (binding->inputType == SDL_CONTROLLER_BINDTYPE_BUTTON) {
                bind.value.button = binding->input.button;
            } else if (binding->inputType == SDL_CONTROLLER_BINDTYPE_HAT) {
                bind.value.hat.hat = binding->input.hat.hat;
                bind.value.hat.hat_mask = binding->input.hat.hat_mask;
            }
            break;
        }
    }
    return bind;
}


int
SDL_GameControllerRumble(SDL_GameController *gamecontroller, Uint16 low_frequency_rumble, Uint16 high_frequency_rumble, Uint32 duration_ms)
{
    return SDL_JoystickRumble(SDL_GameControllerGetJoystick(gamecontroller), low_frequency_rumble, high_frequency_rumble, duration_ms);
}

void
SDL_GameControllerClose(SDL_GameController * gamecontroller)
{
    SDL_GameController *gamecontrollerlist, *gamecontrollerlistprev;

    if (!gamecontroller)
        return;

    SDL_LockJoysticks();

    /* First decrement ref count */
    if (--gamecontroller->ref_count > 0) {
        SDL_UnlockJoysticks();
        return;
    }

    SDL_JoystickClose(gamecontroller->joystick);

    gamecontrollerlist = SDL_gamecontrollers;
    gamecontrollerlistprev = NULL;
    while (gamecontrollerlist) {
        if (gamecontroller == gamecontrollerlist) {
            if (gamecontrollerlistprev) {
                /* unlink this entry */
                gamecontrollerlistprev->next = gamecontrollerlist->next;
            } else {
                SDL_gamecontrollers = gamecontroller->next;
            }
            break;
        }
        gamecontrollerlistprev = gamecontrollerlist;
        gamecontrollerlist = gamecontrollerlist->next;
    }

    SDL_free(gamecontroller->bindings);
    SDL_free(gamecontroller->last_match_axis);
    SDL_free(gamecontroller->last_hat_mask);
    SDL_free(gamecontroller);

    SDL_UnlockJoysticks();
}


/*
 * Quit the controller subsystem
 */
void
SDL_GameControllerQuit(void)
{
    SDL_LockJoysticks();
    while (SDL_gamecontrollers) {
        SDL_gamecontrollers->ref_count = 1;
        SDL_GameControllerClose(SDL_gamecontrollers);
    }
    SDL_UnlockJoysticks();
}

void
SDL_GameControllerQuitMappings(void)
{
    ControllerMapping_t *pControllerMap;

    while (s_pSupportedControllers) {
        pControllerMap = s_pSupportedControllers;
        s_pSupportedControllers = s_pSupportedControllers->next;
        SDL_free(pControllerMap->name);
        SDL_free(pControllerMap->mapping);
        SDL_free(pControllerMap);
    }

    SDL_DelEventWatch(SDL_GameControllerEventWatcher, NULL);

    SDL_DelHintCallback(SDL_HINT_GAMECONTROLLER_IGNORE_DEVICES, NULL, NULL);
    SDL_DelHintCallback(SDL_HINT_GAMECONTROLLER_IGNORE_DEVICES_EXCEPT, NULL, NULL);

    if (SDL_allowed_controllers.entries) {
        SDL_free(SDL_allowed_controllers.entries);
        SDL_zero(SDL_allowed_controllers);
    }
    if (SDL_ignored_controllers.entries) {
        SDL_free(SDL_ignored_controllers.entries);
        SDL_zero(SDL_ignored_controllers);
    }
}

/*
 * Event filter to transform joystick events into appropriate game controller ones
 */
static int
SDL_PrivateGameControllerAxis(SDL_GameController *gamecontroller, SDL_GameControllerAxis axis, Sint16 value)
{
    int posted;

    /* translate the event, if desired */
    posted = 0;
#if !SDL_EVENTS_DISABLED
    if (SDL_GetEventState(SDL_CONTROLLERAXISMOTION) == SDL_ENABLE)
    {
        SDL_Event event;
        event.type = SDL_CONTROLLERAXISMOTION;
        event.caxis.which = gamecontroller->joystick->instance_id;
        event.caxis.axis = axis;
        event.caxis.value = value;
        posted = SDL_PushEvent(&event) == 1;
    }
#endif /* !SDL_EVENTS_DISABLED */
    return (posted);
}

/*
 * Event filter to transform joystick events into appropriate game controller ones
 */
static int
SDL_PrivateGameControllerButton(SDL_GameController * gamecontroller, SDL_GameControllerButton button, Uint8 state)
{
    int posted;
#if !SDL_EVENTS_DISABLED
    SDL_Event event;

    if (button == SDL_CONTROLLER_BUTTON_INVALID)
        return (0);

    switch (state) {
        case SDL_PRESSED:
            event.type = SDL_CONTROLLERBUTTONDOWN;
            break;
        case SDL_RELEASED:
            event.type = SDL_CONTROLLERBUTTONUP;
            break;
        default:
            /* Invalid state -- bail */
            return (0);
    }
#endif /* !SDL_EVENTS_DISABLED */

    if (button == SDL_CONTROLLER_BUTTON_GUIDE) {
        Uint32 now = SDL_GetTicks();
        if (state == SDL_PRESSED) {
            gamecontroller->guide_button_down = now;

            if (gamecontroller->joystick->delayed_guide_button) {
                /* Skip duplicate press */
                return (0);
            }
        } else {
            if (!SDL_TICKS_PASSED(now, gamecontroller->guide_button_down+SDL_MINIMUM_GUIDE_BUTTON_DELAY_MS) && !gamecontroller->joystick->force_recentering) {
                gamecontroller->joystick->delayed_guide_button = SDL_TRUE;
                return (0);
            }
            gamecontroller->joystick->delayed_guide_button = SDL_FALSE;
        }
    }

    /* translate the event, if desired */
    posted = 0;
#if !SDL_EVENTS_DISABLED
    if (SDL_GetEventState(event.type) == SDL_ENABLE) {
        event.cbutton.which = gamecontroller->joystick->instance_id;
        event.cbutton.button = button;
        event.cbutton.state = state;
        posted = SDL_PushEvent(&event) == 1;
    }
#endif /* !SDL_EVENTS_DISABLED */
    return (posted);
}

/*
 * Turn off controller events
 */
int
SDL_GameControllerEventState(int state)
{
#if SDL_EVENTS_DISABLED
    return SDL_IGNORE;
#else
    const Uint32 event_list[] = {
            SDL_CONTROLLERAXISMOTION, SDL_CONTROLLERBUTTONDOWN, SDL_CONTROLLERBUTTONUP,
            SDL_CONTROLLERDEVICEADDED, SDL_CONTROLLERDEVICEREMOVED, SDL_CONTROLLERDEVICEREMAPPED,
    };
    unsigned int i;

    switch (state) {
        case SDL_QUERY:
            state = SDL_IGNORE;
            for (i = 0; i < SDL_arraysize(event_list); ++i) {
                state = SDL_EventState(event_list[i], SDL_QUERY);
                if (state == SDL_ENABLE) {
                    break;
                }
            }
            break;
        default:
            for (i = 0; i < SDL_arraysize(event_list); ++i) {
                SDL_EventState(event_list[i], state);
            }
            break;
    }
    return (state);
#endif /* SDL_EVENTS_DISABLED */
}

void
SDL_GameControllerHandleDelayedGuideButton(SDL_Joystick *joystick)
{
    SDL_GameController *controllerlist = SDL_gamecontrollers;
    while (controllerlist) {
        if (controllerlist->joystick == joystick) {
            SDL_PrivateGameControllerButton(controllerlist, SDL_CONTROLLER_BUTTON_GUIDE, SDL_RELEASED);
            break;
        }
        controllerlist = controllerlist->next;
    }
}

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "/home/estebagel/UE_4.20-unstoppable/Engine/Source/ThirdParty/SDL2/SDL-gui-backend/include/close_code.h"

//#endif /* SDL_rwops_h_ */




/* vi: set ts=4 sw=4 expandtab: */

