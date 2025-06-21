#pragma once
#include "./Engine/Utility/TypeHelpers.h"
#include "Text.h"
#include <vector>
#include <map>

class UIPanel
{
private:
	Name name;
	Vector2 position;
	std::vector<Text*> textElements;

	bool isActive = true;

public:
	UIPanel() {};
	UIPanel(Name name, std::vector<Text*> textElementsToAdd);
	~UIPanel();

	void Update();
	void UpdateTextPositions();

	std::vector<Text*> GetTextElements() { return textElements; }
	void SetTextElements(std::vector<Text*> textElements) { this->textElements = textElements; }

	void SetTextElement(std::wstring text, Name name);

	Name& GetName() { return name; }

	Vector2& GetPosition() { return position; }
	void SetPosition(const Vector2& position) { this->position = position; UpdateTextPositions(); }

	bool IsActive() { return isActive; }
	void SetActive(bool active) { isActive = active; }



	

};

