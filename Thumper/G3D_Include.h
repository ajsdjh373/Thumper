/*
Author: Nathan Dunn
Module: G3D

This is the central include file for the G3D module. Including this file will give access
to all G3D components. This file should be the only G3D include needed in files that use
the G3D module.

Known bugs and limitations:
- None currently
*/

#pragma once

// Core G3D Components
#include "G3D_Camera.h"
#include "G3D_RenderEngine.h"
#include "G3D_Utilities.h"

// G3D Objects
#include "G3D_Obj_Tile.h"
#include "G3D_Obj_WireFrame.h"

// G3D Shaders
#include "G3D_Shader_Texture.h"
#include "G3D_Shader_WireFrame.h"

// G3D Textures
#include "G3D_Texture2D.h" 