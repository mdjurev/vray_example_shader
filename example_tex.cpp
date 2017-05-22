#pragma once


#include "utils.h"
#include "misc.h"
#include "vraybase.h"
#include "plugman.h"
#include "vector.h"
#include "vrayinterface.h"
#include "vrayrenderer.h"
#include "vrayplugins.h"

using namespace VR;

//********************************************************
// ExampleTex

/// The parameter descriptor for the ExampleTex plugin.
struct ExampleTex_Params: VRayParameterListDesc {
	ExampleTex_Params(void) {
		
	}
};

/// The ExampleTex plugin.
struct ExampleTex: VRayPlugin, TextureInterface {
	/// Constructor.
	ExampleTex(VRayPluginDesc *desc): VRayPlugin(desc) {
	
	}

	/// Destructor.
	~ExampleTex(void) {
	}

	/// Return the interfaces that we support.
	PluginInterface* newInterface(InterfaceID id) VRAY_OVERRIDE {
		return (id==EXT_TEXTURE)? static_cast<TextureInterface*>(this) : VRayPlugin::newInterface(id);
	}

	PluginBase* getPlugin(void) VRAY_OVERRIDE { return this; }

	//// From VRayPlugin
	//void frameBegin(VRayRenderer *vray) VRAY_OVERRIDE {VRayPlugin::frameBegin(vray);};
	//void frameEnd(VRayRenderer *vray) VRAY_OVERRIDE {VRayPlugin::frameEnd(vray);};

	// From TextureInterface	
	/// Returns the value of the texture for the point in the given context.
	AColor getTexColor(const VRayContext &rc)  VRAY_OVERRIDE {
		return AColor(0.f, 0.f, 0.f, 0.f);
	}

	/// Returns the minimum and maximum values of the texture
	void getTexColorBounds(AColor &fmin, AColor &fmax) VRAY_OVERRIDE {

	}

	/// Returns the negative texture gradient of the color intensity (can be used for bump mapping).
	virtual Vector getColorBumpGradient(const VRayContext &rc) { return Vector(0.0f, 0.0f, 0.0f); }

};


#define ExampleTex_PluginID PluginID(LARGE_CONST(2017052212))
static VRAY3_CONST_COMPAT tchar *libText = "Example texture for V-Ray";
static const tchar *descText = "A simple shader example for V-Ray";
#ifdef __VRAY40__
SIMPLE_PLUGIN_LIBRARY(ExampleTex_PluginID, "ExampleTex", libText, descText, ExampleTex, ExampleTex_Params, EXT_TEXTURE);
#else
SIMPLE_PLUGIN_LIBRARY(ExampleTex_PluginID, EXT_TEXTURE, "ExampleTex", libText, ExampleTex, ExampleTex_Params);
#endif
