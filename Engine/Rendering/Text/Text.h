#pragma once
#include "../../Utility/TypeHelpers.h"
#include <string>
#include <iostream>


class Text
{
private:
	Vector2 position;
	Vector4 color;
	Vector2 scale;
	
	std::wstring text;

	bool isActive = true;
	

public:
	Text();
	Text(Vector2 pos, std::wstring text);
	Text(Vector2 pos, std::wstring text, Name name);


	~Text() = default;

	Name name;

	std::wstring& GetText() { return text; }
	void SetText(const std::wstring& text) { this->text = text;}
	void SetText(const std::wstring&, Vector4 color) { this->text = text; this->color = color; }
	
	Vector2& GetPosition() { return position; }
	void SetPosition(const Vector2& pos) { position = pos; }

	Vector4 GetColor() { return color; }
	void SetColor(Vector4 color) { this->color = color; }

	Vector2& GetScale() { return scale; }
	void SetScale(const Vector2& scale) { this->scale = scale; }

	bool IsActive() { return isActive; }
	void SetActive(bool active) { isActive = active; }


};

