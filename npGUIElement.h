//#pragma once
//
//
//class alGUIElement : public npUserData, public npID
//{
//protected:
//	npRenderer* m_renderer = 0;
//	npInput* m_input = 0;
//public:
//	alGUIElement(npGUIContext* ctx);
//	virtual ~alGUIElement() {}
//
//	npGUIColorTheme* m_colorTheme = 0;
//
//	virtual void Draw(float32_t dt) = 0;
//
//	virtual void OnDraw() {}
//	virtual void OnMouseEnter() {}
//	virtual void OnMouseMove() {}
//	virtual void OnMouseLeave() {}
//	virtual void OnLMBHit() {}
//	virtual void OnRMBHit() {}
//	virtual void OnMMBHit() {}
//	virtual void OnLMBRelease() {}
//	virtual void OnRMBRelease() {}
//	virtual void OnMMBRelease() {}
//
//	virtual void Update(float32_t dt);
//
//	virtual float32_t GetHeight()
//	{
//		return m_buildArea.w - m_buildArea.y;
//	}
//
//	alGUIContext* m_context = nullptr;
//	alList<alGUIElement*> m_children;
//
//	virtual void SetVisible(bool set)
//	{
//		m_visible = set;
//		auto c = m_children.m_head;
//		if (c)
//		{
//			auto l = c->m_left;
//			while (true)
//			{
//				c->m_data->SetVisible(set);
//				if (c == l)
//					break;
//				c = c->m_right;
//			}
//		}
//	}
//
//	virtual alGUIElement* GetParent() { return m_parent; }
//	virtual void SetParent(alGUIElement* parent)
//	{
//		if (m_parent)
//		{
//			m_parent->m_children.erase_first(this);
//		}
//
//		m_parent = parent;
//
//		if (parent)
//			parent->m_children.push_back(this);
//	}
//
//
//	virtual void UpdateTransform()
//	{
//		if (!m_visible)
//			return;
//
//		/*	if (m_onUpdateTransform)
//			{
//				m_onUpdateTransform(this);
//				return;
//			}*/
//
//			//	m_offsetTransformed = m_offset;
//
//				//if (m_parent)
//				//{
//
//				////	m_offsetTransformed += m_parent->m_offsetTransformed;
//
//				//	m_buildAreaTransformed.x += m_parent->m_buildAreaTransformed.x;
//				//	m_buildAreaTransformed.y += m_parent->m_buildAreaTransformed.y;
//				//	m_buildAreaTransformed.z += m_parent->m_buildAreaTransformed.x;
//				//	m_buildAreaTransformed.w += m_parent->m_buildAreaTransformed.y;
//				//	m_sensorAreaTransformed.x += m_parent->m_sensorAreaTransformed.x;
//				//	m_sensorAreaTransformed.y += m_parent->m_sensorAreaTransformed.y;
//				//	m_sensorAreaTransformed.z += m_parent->m_sensorAreaTransformed.x;
//				//	m_sensorAreaTransformed.w += m_parent->m_sensorAreaTransformed.y;
//				//	m_clipAreaTransformed.x += m_parent->m_clipAreaTransformed.x;
//				//	m_clipAreaTransformed.y += m_parent->m_clipAreaTransformed.y;
//				//	m_clipAreaTransformed.z += m_parent->m_clipAreaTransformed.x;
//				//	m_clipAreaTransformed.w += m_parent->m_clipAreaTransformed.y;
//
//
//				//	float32_t parentRectSizeX_1 = 1.f / (m_parent->m_buildAreaTransformed.z - m_parent->m_buildAreaTransformed.x);
//				//	float32_t parentRectSizeY_1 = 1.f / (m_parent->m_buildAreaTransformed.w - m_parent->m_buildAreaTransformed.y);
//
//				//	float32_t parentCreationCenter_X = m_parent->m_buildArea.x +
//				//		((m_parent->m_buildArea.z - m_parent->m_buildArea.x) * 0.5f);
//				//	float32_t parentCreationCenter_Y = m_parent->m_buildArea.y +
//				//		((m_parent->m_buildArea.w - m_parent->m_buildArea.y) * 0.5f);
//
//				//	float32_t parentCurrentCenter_X = m_parent->m_buildAreaTransformed.x +
//				//		((m_parent->m_buildAreaTransformed.z - m_parent->m_buildAreaTransformed.x) * 0.5f);
//				//	float32_t parentCurrentCenter_Y = m_parent->m_buildAreaTransformed.y +
//				//		((m_parent->m_buildAreaTransformed.w - m_parent->m_buildAreaTransformed.y) * 0.5f);
//
//				//	float32_t parentRectSizeDiff_X = parentCurrentCenter_X - parentCreationCenter_X;
//				//	float32_t parentRectSizeDiff_Y = parentCurrentCenter_Y - parentCreationCenter_Y;
//
//
//
//				//	switch (m_alignment)
//				//	{
//				//	case alGUIElementAlignment::LeftTop:
//				//		break;
//				//	case alGUIElementAlignment::RightTop:
//				//		m_buildAreaTransformed.x = m_parent->m_buildAreaTransformed.z - (m_parent->m_buildArea.z - m_buildArea.x);
//				//		m_buildAreaTransformed.z = m_parent->m_buildAreaTransformed.z - (m_parent->m_buildArea.z - m_buildArea.z);
//
//				//		m_sensorAreaTransformed.x = m_buildAreaTransformed.x;
//				//		m_sensorAreaTransformed.z = m_buildAreaTransformed.z;
//
//				//		m_clipAreaTransformed.x = m_buildAreaTransformed.x;
//				//		m_clipAreaTransformed.z = m_buildAreaTransformed.z;
//				//		break;
//				//	case alGUIElementAlignment::LeftBottom:
//				//		m_buildAreaTransformed.y = m_parent->m_buildAreaTransformed.w - (m_parent->m_buildArea.w - m_buildArea.y);
//				//		m_buildAreaTransformed.w = m_parent->m_buildAreaTransformed.w - (m_parent->m_buildArea.w - m_buildArea.w);
//
//				//		m_sensorAreaTransformed.y = m_buildAreaTransformed.y;
//				//		m_sensorAreaTransformed.w = m_buildAreaTransformed.w;
//
//				//		m_clipAreaTransformed.y = m_buildAreaTransformed.y;
//				//		m_clipAreaTransformed.w = m_buildAreaTransformed.w;
//				//		break;
//				//	case alGUIElementAlignment::RightBottom:
//				//		m_buildAreaTransformed.x = m_parent->m_buildAreaTransformed.z - (m_parent->m_buildArea.z - m_buildArea.x);
//				//		m_buildAreaTransformed.y = m_parent->m_buildAreaTransformed.w - (m_parent->m_buildArea.w - m_buildArea.y);
//				//		m_buildAreaTransformed.z = m_parent->m_buildAreaTransformed.z - (m_parent->m_buildArea.z - m_buildArea.z);
//				//		m_buildAreaTransformed.w = m_parent->m_buildAreaTransformed.w - (m_parent->m_buildArea.w - m_buildArea.w);
//
//				//		m_sensorAreaTransformed.x = m_buildAreaTransformed.x;
//				//		m_sensorAreaTransformed.y = m_buildAreaTransformed.y;
//				//		m_sensorAreaTransformed.z = m_buildAreaTransformed.z;
//				//		m_sensorAreaTransformed.w = m_buildAreaTransformed.w;
//
//				//		m_clipAreaTransformed.x = m_buildAreaTransformed.x;
//				//		m_clipAreaTransformed.y = m_buildAreaTransformed.y;
//				//		m_clipAreaTransformed.z = m_buildAreaTransformed.z;
//				//		m_clipAreaTransformed.w = m_buildAreaTransformed.w;
//				//		break;
//				//	case alGUIElementAlignment::Top:
//				//		m_buildAreaTransformed.x = m_buildArea.x + parentRectSizeDiff_X;
//				//		m_buildAreaTransformed.z = m_buildArea.z + parentRectSizeDiff_X;
//
//				//		m_sensorAreaTransformed.x = m_buildAreaTransformed.x;
//				//		m_sensorAreaTransformed.z = m_buildAreaTransformed.z;
//
//				//		m_clipAreaTransformed.x = m_buildAreaTransformed.x;
//				//		m_clipAreaTransformed.z = m_buildAreaTransformed.z;
//				//		break;
//				//	case alGUIElementAlignment::Left:
//				//		m_buildAreaTransformed.y = m_buildArea.y + parentRectSizeDiff_Y;
//				//		m_buildAreaTransformed.w = m_buildArea.w + parentRectSizeDiff_Y;
//
//				//		m_sensorAreaTransformed.y = m_buildAreaTransformed.y;
//				//		m_sensorAreaTransformed.w = m_buildAreaTransformed.w;
//
//				//		m_clipAreaTransformed.y = m_buildAreaTransformed.y;
//				//		m_clipAreaTransformed.w = m_buildAreaTransformed.w;
//				//		break;
//				//	case alGUIElementAlignment::Right:
//				//		m_buildAreaTransformed.y = m_buildArea.y + parentRectSizeDiff_Y;
//				//		m_buildAreaTransformed.w = m_buildArea.w + parentRectSizeDiff_Y;
//
//				//		m_sensorAreaTransformed.y = m_buildAreaTransformed.y;
//				//		m_sensorAreaTransformed.w = m_buildAreaTransformed.w;
//
//				//		m_clipAreaTransformed.y = m_buildAreaTransformed.y;
//				//		m_clipAreaTransformed.w = m_buildAreaTransformed.w;
//
//				//		// + rightTop
//				//		m_buildAreaTransformed.x = m_parent->m_buildAreaTransformed.z - (m_parent->m_buildArea.z - m_buildArea.x);
//				//		m_buildAreaTransformed.z = m_parent->m_buildAreaTransformed.z - (m_parent->m_buildArea.z - m_buildArea.z);
//
//				//		m_sensorAreaTransformed.x = m_buildAreaTransformed.x;
//				//		m_sensorAreaTransformed.z = m_buildAreaTransformed.z;
//
//				//		m_clipAreaTransformed.x = m_buildAreaTransformed.x;
//				//		m_clipAreaTransformed.z = m_buildAreaTransformed.z;
//				//		break;
//				//	case alGUIElementAlignment::Bottom:
//				//		m_buildAreaTransformed.x = m_buildArea.x + parentRectSizeDiff_X;
//				//		m_buildAreaTransformed.z = m_buildArea.z + parentRectSizeDiff_X;
//
//				//		m_sensorAreaTransformed.x = m_buildAreaTransformed.x;
//				//		m_sensorAreaTransformed.z = m_buildAreaTransformed.z;
//
//				//		m_clipAreaTransformed.x = m_buildAreaTransformed.x;
//				//		m_clipAreaTransformed.z = m_buildAreaTransformed.z;
//
//				//		// + leftBottom
//				//		m_buildAreaTransformed.y = m_parent->m_buildAreaTransformed.w - (m_parent->m_buildArea.w - m_buildArea.y);
//				//		m_buildAreaTransformed.w = m_parent->m_buildAreaTransformed.w - (m_parent->m_buildArea.w - m_buildArea.w);
//
//				//		m_sensorAreaTransformed.y = m_buildAreaTransformed.y;
//				//		m_sensorAreaTransformed.w = m_buildAreaTransformed.w;
//
//				//		m_clipAreaTransformed.y = m_buildAreaTransformed.y;
//				//		m_clipAreaTransformed.w = m_buildAreaTransformed.w;
//
//				//		break;
//				//	case alGUIElementAlignment::Center:
//				//	default:
//				//		// Right
//				//		m_buildAreaTransformed.y = m_buildArea.y + parentRectSizeDiff_Y;
//				//		m_buildAreaTransformed.w = m_buildArea.w + parentRectSizeDiff_Y;
//
//				//		m_sensorAreaTransformed.y = m_buildAreaTransformed.y;
//				//		m_sensorAreaTransformed.w = m_buildAreaTransformed.w;
//
//				//		m_clipAreaTransformed.y = m_buildAreaTransformed.y;
//				//		m_clipAreaTransformed.w = m_buildAreaTransformed.w;
//
//				//		// bottom
//				//		m_buildAreaTransformed.x = m_buildArea.x + parentRectSizeDiff_X;
//				//		m_buildAreaTransformed.z = m_buildArea.z + parentRectSizeDiff_X;
//
//				//		m_sensorAreaTransformed.x = m_buildAreaTransformed.x;
//				//		m_sensorAreaTransformed.z = m_buildAreaTransformed.z;
//
//				//		m_clipAreaTransformed.x = m_buildAreaTransformed.x;
//				//		m_clipAreaTransformed.z = m_buildAreaTransformed.z;
//				//		break;
//				//	}
//				//}
//	}
//
//	// All elements are placed in 2D space, and for this I need
//	// two things. 1 - position, 2 - size.
//	// Using position and size I can calculate ...Area vectors
//	// It will be calculated in Rebuild method
//	alVec2f m_position;
//	alVec2f m_size;
//
//	alVec4f m_buildArea;  // using for create triangles
//	alVec4f m_sensorArea; //   for detecting cursor in area or not
//	alVec4f m_clipArea;   //   for scissor
//
//	// for some reason you can change sensor area
//	alVec4f m_sensorAreaIndent;
//	alVec4f m_clipAreaIndent;
//
//	bool m_isCursorInSensorArea = false;
//	bool m_isLMBClicked = false;
//	bool m_isRMBClicked = false;
//	bool m_isMMBClicked = false;
//	bool m_isX1MBClicked = false;
//	bool m_isX2MBClicked = false;
//
//	virtual void Rebuild();
//
//
//
//	// Must calculate using parents _Transformed vectors.
//	//  build elements and detect cursor in area only using this.
//	//alVec4f m_buildAreaTransformed;
//	//alVec4f m_sensorAreaTransformed;
//	//alVec4f m_clipAreaTransformed;
//
//	//alVec2f m_offset;
//	//alVec2f m_offsetTransformed;
//
//	alGUIElementAlignment m_alignment = alGUIElementAlignment::LeftTop;
//
//
//	// use SetVisible();
//	bool m_visible = true;
//
//	bool m_ignoreInput = false; // if true then only draw
//
//	// For exmple for text in button. Buttons will draw elements in button->Draw();
//	bool m_ignoreDraw = false;
//
//	//bool m_isInActiveAreaRect = false;
//
//	// it should be root object by default. 
//	// it always must be if(m_parent) so this if unnecessary
//	alGUIElement* m_parent = nullptr;
//};
