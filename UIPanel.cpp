#include "UIPanel.h"

UIPanel::UIPanel(Name name, std::vector<Text*> textElementsToAdd)
{
	textElements = textElementsToAdd;
	this->name = name;
	//position = Vector2(10, 0);
	for (auto& text : textElements)
	{
		text->SetPosition(text->GetPosition() + position);
	}
}

UIPanel::~UIPanel()
{
}

void UIPanel::Update()
{
	UpdateTextPositions();
}

void UIPanel::UpdateTextPositions()
{
	for (auto& text : textElements)
	{
		text->SetPosition(text->GetPosition() + position);
	}
}

void UIPanel::SetTextElement(std::wstring text, Name name)
{
	for (auto& textElement : textElements)
	{
		if (textElement->name == name)
		{
			textElement->SetText(text);
		}
	}
}
