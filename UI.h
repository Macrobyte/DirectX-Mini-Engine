#pragma once
#include <vector>
#include "UIPanel.h"

class UI
{
public:
	UI() {};
	~UI() {};

	void AddPanel(UIPanel* panel);
	void RemovePanel(UIPanel* panel);

	void UpdatePanels();

	std::vector<UIPanel*> GetPanels() { return panels; }
	UIPanel* GetPanel(Name name);

private:

	std::vector<UIPanel*> panels;


};

