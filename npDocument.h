/*
* npDocument база для всех типов документов.
* Типы документов например:
* - текстовый документ
* - двоичный документ
* 
* Для текстового документа должен быть дочерний класс
* который будет рисовать этот документ.
* Для двоичного документа будет рисоваться HEX редактор.
*/
#pragma once


enum class npDocumentType
{
	Text,
	Binary,
	_end
};

class npDocument
{
protected:
	bool m_isSaved = false;
	npRenderer* m_renderer = 0;
	npDocumentType m_type = npDocumentType::Text;
public:
	npDocument(npRenderer* r) : m_renderer(r){}
	virtual ~npDocument() {}

	virtual void Update(float dt) = 0;
	virtual void Draw(float dt) = 0;
	virtual void Save() = 0;
	virtual void SaveAs() = 0;
	
	virtual size_t GetSelectionBegin() { return 0; };
	virtual size_t GetSelectionEnd() { return 0; };
	
	virtual void OnTextSearch(bool down) {}
	virtual void GoTo(uint32_t line, uint32_t col, bool tabs) {}
	virtual void Select(size_t begin, size_t end) {}
	virtual void Select(size_t num) {}

	virtual bool IsNeedToSave() { return m_isSaved == false; }
	virtual void OnPopupCommand(uint32_t cmd) {}
	virtual void OnWindowSize() {}
	
	// когда нажали на item в docbar
	virtual void OnActivate() {}
	virtual void OnWindowActivate() {}
	virtual void OnWindowDeactivate() {}

	virtual void SetTitle(const wchar_t* str)
	{
		m_title = str;
		if (m_title.size() > 35)
		{
			for (size_t i = m_title.size() - 30; i >= 0; )
			{
				m_title.pop_back();

				if (!i)
					break;
				--i;
			}
			m_title.append("...");
		}
	}

	npDocumentType GetType() { return m_type; }

	npStringA m_filePath;
	npStringW m_filePathW;
	npStringW m_title;
	float32_t m_titleLenInPixels = 0.f;
	npVec4f m_docbarItemRect;
};

