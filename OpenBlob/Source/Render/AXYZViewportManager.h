#pragma once
#include <Toshi/Render/TViewport.h>
#include <Toshi/Render/TCameraObject.h>
#include "Cameras/ACamera.h"

class AXYZViewportManager
{
public:
	enum Viewport
	{
		VIEWPORT_NONE = -1,
		VIEWPORT_Fullscreen,
		VIEWPORT_2X2_TOPLEFT,
		VIEWPORT_2X2_TOPRIGHT,
		VIEWPORT_2X2_BOTTOMLEFT,
		VIEWPORT_2X2_BOTTOMRIGHT,
		VIEWPORT_1P1_HORIZONTAL_TOP,
		VIEWPORT_1P1_HORIZONTAL_BOTTOM,
		VIEWPORT_1P1_VERTICAL_LEFT,
		VIEWPORT_1P1_VERTICAL_RIGHT,
		VIEWPORT_1P1P1_LEFT,
		VIEWPORT_1P1P1_CENTER,
		VIEWPORT_1P1P1_RIGHT,
		VIEWPORT_Count,
	};

	static constexpr float sm_fDefaultFOV = Toshi::TMath::DegToRad(60.0f);

public:
	AXYZViewportManager();
	~AXYZViewportManager() = default;

	Toshi::TViewport* CreateViewport(float x, float y, float w, float h);

	void GetUseableArea(float& x, float& y, float& w, float& h);

	void SetMainFullScreen(bool fullscreen);

	int GetViewportBeingRendered() const
	{
		return m_iRenderingViewportIndex;
	}

private:
	Toshi::TViewport* m_Viewports[VIEWPORT_Count];
	Toshi::TCameraObject* m_CameraObjects[VIEWPORT_Count];
	ACamera* m_Cameras[VIEWPORT_Count];
	TBOOL m_UsedCameras[VIEWPORT_Count];
	void* m_Unk;
	int m_iRenderingViewportIndex;

	/*void SetMargins(int left, int right, int top, int bottom)
	{
		m_iMarginLeft = left;
		m_iMarginRight = right;
		m_iMarginTop = top;
		m_iMarginBottom = bottom;
	}

private:
	int m_iMarginLeft;
	int m_iMarginRight;
	int m_iMarginTop;
	int m_iMarginBottom;*/
};

