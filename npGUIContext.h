#pragma once
//class npGUIContext
//{
//	npRenderer* m_gs = 0;
//	alArray<alGUIPanel*> m_panels;
//public:
//	npGUIContext(alSystemWindow*, alGS*);
//	~npGUIContext();
//	void Update(float32_t dt);
//	void Draw(float32_t dt);
//
//	alGUIPanel* GetNewPanel();
//	void DeleteAllPanels();
//
//	alGS* GetGS() { return m_gs; }
//	alSystemWindow* GetWindow() { return m_window; }
//
//	bool m_isElementUnderCursor = false; // this will be set false every Update
//	alGUIElement* m_elementUnderCursor = 0;
//	//alGUIPanel* m_panelUnderCursor = 0;
//
//	// if some text input element activated
//	alGUITextInput* m_inputTextElementActivated = 0;
//
//	// when click and hold
//	alGUIElement* m_elementInMouseFocus = 0;
//
//	alGSMesh* _create_bg_mesh(
//		const alVec4f& buildRect,
//		float32_t smooth_corner_indent,
//		uint32_t smooth_corner_iterations,
//		alVec4f* UV);
//
//	alGUIRadioGroup m_radioGroups[100];
//};
//
