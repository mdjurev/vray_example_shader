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

/// The parameter descriptor for the TexExample plugin.
struct TexExample_Params: VRayParameterListDesc {
	TexExample_Params(void) {
		addParamTexture("texa", -1, "Texture input A");
		addParamTexture("texb", -1, "Texture input B");
	}
};

/// The TexExample plugin.
struct TexExample: VRayPlugin, TextureInterface {
	/// Constructor.
	TexExample(VRayPluginDesc *desc): VRayPlugin(desc) {
		paramList->setParamCache("texa", &texa);
		paramList->setParamCache("texb", &texb);
		texa=texb=NULL;
	}

	/// Destructor.
	~TexExample(void) {
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
		AColor a = texa ? texa->getTexColor(rc) : AColor(0.f, 0.f, 0.f, 0.f);
		AColor b = texb ? texb->getTexColor(rc) : AColor(0.f, 0.f, 0.f, 0.f);
		return a+b;
	}

	/// Returns the minimum and maximum values of the texture
	void getTexColorBounds(AColor &fmin, AColor &fmax) VRAY_OVERRIDE {
		AColor amin,amax,bmin,bmax;
		amin = bmin = AColor(0.f, 0.f, 0.f, 0.f);
		amax = bmax = AColor(1.f, 1.f, 1.f, 1.f);
		if(texa) texa->getTexColorBounds(amin, amax);
		if(texb) texb->getTexColorBounds(bmin, bmax);
		// Calculate the bounds for the result. The correct result here would mean that the 
		// texture behaves predictably in displacement
		fmax = amax+bmax;
		fmin = VR::Min(amin, bmin);
	}

	/// Returns the negative texture gradient of the color intensity (can be used for bump mapping).
	virtual Vector getColorBumpGradient(const VRayContext &rc) { return Vector(0.0f, 0.0f, 0.0f); }

private:
	TextureInterface *texa;
	TextureInterface *texb;

};


#define TexExample_PluginID PluginID(LARGE_CONST(2017052212))
static VRAY3_CONST_COMPAT tchar *libText = "Example texture for V-Ray";
static const tchar *descText = "A simple shader example for V-Ray";
#ifdef __VRAY40__
SIMPLE_PLUGIN_LIBRARY(TexExample_PluginID, "TexExample", libText, descText, TexExample, TexExample_Params, EXT_TEXTURE);
#else
SIMPLE_PLUGIN_LIBRARY(TexExample_PluginID, EXT_TEXTURE, "TexExample", libText, TexExample, TexExample_Params);
#endif
