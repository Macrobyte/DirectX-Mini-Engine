#include "UI.h"


void UI::AddPanel(UIPanel* panel)
{
	panels.push_back(panel);
}

void UI::RemovePanel(UIPanel* panel)
{
	for (int i = 0; i < panels.size(); i++)
	{
		if (panels[i] == panel)
		{
			panels.erase(panels.begin() + i);
			return;
		}
	}
}

void UI::UpdatePanels()
{
	for (auto& panel : panels)
	{
		panel->Update();
	}
}

UIPanel* UI::GetPanel(Name name)
{
	for (auto& panel : panels)
	{
		if (panel->GetName() == name)
		{
			return panel;
		}
	}
	return nullptr;
}

