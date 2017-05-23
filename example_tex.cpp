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
		addParamTexture("texa", -1, "Texture input A", "displayName=(Texture A)");
		addParamTexture("texb", -1, "Texture input B", "displayName=(Texture B)");
		addParamInt("operation", 0, -1, "The mode of operation: 0 - add A and B; 1 - multiply A and B; 2 - mix A and B using ratio; 3 - mix A and B using UVs",
			"displayName=(Mode), enum=(0:Add;1:Multiply;2:Mix by ratio;3:Mix by UV)");
		addParamTextureFloat("ratio", -1, "The ratio of mixing A/B. 0-A only, 1-B only.", "displayName=(Ratio), enable=(::operation=2)");
		// Note: When implementing a texture with UVs, the texture class should extend MappedTexture, which provides it's own uvwgen parameter.
		// Here, for the sake of simplicity, we create one ourselves.
		addParamPlugin("uvwgen", EXT_UVWGEN, -1, "The UV generator. Used only when mode is 3.");
	}
};

/// The TexExample plugin.
struct TexExample: VRayPlugin, TextureInterface {
	/// Constructor.
	TexExample(VRayPluginDesc *desc): VRayPlugin(desc) {
		paramList->setParamCache("texa", &texa);
		paramList->setParamCache("texb", &texb);
		paramList->setParamCache("operation", &operation);
		paramList->setParamCache("ratio", &ratio);
		paramList->setParamCache("uvwgen", &uvwgen_plugin);
		texa=texb=NULL;
		ratio=NULL;
		operation=0;
		uvwgen_plugin=NULL;
		uvwgen=NULL;
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
	void frameBegin(VRayRenderer *vray) VRAY_OVERRIDE {
		// This populates the parameter caches, so it's very important to call:
		VRayPlugin::frameBegin(vray);
		uvwgen = queryInterface<UVWGenInterface>(uvwgen_plugin, EXT_UVWGEN);
	}
	void frameEnd(VRayRenderer *vray) VRAY_OVERRIDE {
		VRayPlugin::frameEnd(vray);
		uvwgen = NULL;
	}

	// From TextureInterface	
	/// Returns the value of the texture for the point in the given context.
	AColor getTexColor(const VRayContext &rc)  VRAY_OVERRIDE {
		AColor a = texa ? texa->getTexColor(rc) : AColor(0.f, 0.f, 0.f, 0.f);
		AColor b = texb ? texb->getTexColor(rc) : AColor(0.f, 0.f, 0.f, 0.f);
		if(operation==0)
			return a+b;
		else if(operation==1)
			return a*b;
		else if(operation==2) {
			float r = ratio ? ratio->getTexFloat(rc) : 0.5f;
			return a*r+(1.f-r)*b;
		}
		else if(operation=3) {
			Vector uvw(0.f);
			bool valid=false;
			if(uvwgen) {				
				uvw = uvwgen->getUVW(rc, valid);
			}
			if(!valid) {
				uvw.makeZero();
			}
			return a*uvw.x+b*uvw.y;
		}
		AColor zero; zero.makeZero();
		return zero;
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
		if(operation==0 || operation==2 || operation==3) {
			fmax = amax+bmax;
			fmin = VR::Min(amin, bmin);
		}
		else if(operation==1) {
			fmax=VR::Max(amax, bmax);
			fmax=VR::Max(fmax, amax*bmax);
			fmin=VR::Min(amin, bmin);
			fmin=VR::Min(fmin, amin*bmin);
		}
	}

	/// Returns the negative texture gradient of the color intensity (can be used for bump mapping).
	virtual Vector getColorBumpGradient(const VRayContext &rc) { return Vector(0.0f, 0.0f, 0.0f); }

private:
	TextureInterface *texa;
	TextureInterface *texb;
	int operation;
	TextureFloatInterface *ratio;
	PluginBase *uvwgen_plugin;
	UVWGenInterface *uvwgen;
};


#define TexExample_PluginID PluginID(LARGE_CONST(2017052212))
static VRAY3_CONST_COMPAT tchar *libText = "Example texture for V-Ray";
static const tchar *descText = "A simple shader example for V-Ray";
#ifdef __VRAY40__
SIMPLE_PLUGIN_LIBRARY(TexExample_PluginID, "TexExample", libText, descText, TexExample, TexExample_Params, EXT_TEXTURE);
#else
SIMPLE_PLUGIN_LIBRARY(TexExample_PluginID, EXT_TEXTURE, "TexExample", libText, TexExample, TexExample_Params);
#endif
