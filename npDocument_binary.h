#pragma once
class npDocument_binary : public npDocument
{
	npFont* m_fontEditor = 0;
	int32_t m_fontEditorSize = 10;
	npVec4f m_editorArea;
	npVec4f m_editorArea_bytes;
	npVec4f m_editorArea_text;

	FILE* m_file = 0;

	npArray<uint8_t> m_bytes;
	void _setTitleFromPath();

	uint32_t m_maxNumOfLines = 0;
	void _findMaxNumOfLines();
	
	float m_nextLinePositionOffset = 0.f;

	uint64_t m_fileFirstDrawBytePosition = 0;
	uint32_t m_numOfBytesToDraw = 0;
	void _readBytes();

	enum
	{
		editMode_bytes,
		editMode_text,
	};
	uint32_t m_editMode = editMode_bytes;

public:
	npDocument_binary(npRenderer*, FILE*f, const char* filePath);
	virtual ~npDocument_binary();

	virtual void Update(float dt) override;
	virtual void Draw(float dt) override;
	virtual void Save() override;
	virtual void SaveAs() override;
};

