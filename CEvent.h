#pragma once
enum class EEventType
{
	Window,
	GUI,
};

struct SEvent_Window
{
	enum _event
	{
		size_changed,
	}
	m_event;

	bool is_equal(const SEvent_Window& other)
	{
		if (m_event != other.m_event)
			return false;
		return true;
	}
};

struct SEvent_GUI
{
	enum _event
	{
		cursor_entered_textEditor,
		cursor_leaved_textEditor,
	}
	m_event;

	bool is_equal(const SEvent_GUI& other)
	{
		if (m_event != other.m_event)
			return false;
		return true;
	}
};

class CEvent
{
public:
	CEvent() {}
	~CEvent() {}

	EEventType m_type = EEventType::Window;

	union
	{
		SEvent_Window m_event_window;
		SEvent_GUI m_event_gui;
	};

	bool IsEqual(const CEvent& event)
	{
		if (m_type != event.m_type) return false;
		switch (m_type)
		{
		case EEventType::Window:
			return m_event_window.is_equal(event.m_event_window);
		case EEventType::GUI:
			return m_event_gui.is_equal(event.m_event_gui);
		default:
			break;
		}
		return true;
	}
};
