#include "Text.h"
#include "SimpleMath.h"
#include <DirectXColors.h>

Text::Text(Vector2 pos, std::wstring text)
{
	position = pos;
	this->text = text;
	name = "Null";
	color = DirectX::Colors::White;
	scale = Vector2(0.5f, 0.5f);
}

Text::Text(DirectX::SimpleMath::Vector2 pos, std::wstring text, Name name)
{
	position = pos;
	this->text = text;
	this->name = name;
	color = DirectX::Colors::White;
	scale = Vector2(0.5f, 0.5f);

}

