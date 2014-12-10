
#include "menu.h"

Menu* Menu::activeMenu;
int Menu::menuState;
int Menu::lastMenuState;

Menu::Menu()
{
	selectedItem = -1;	//nothing return
	menuItemFont = new TextDX();
	menuHeadingFont = new TextDX();
}

void Menu::initialize(Graphics *g, Input *i, Menu* p)
{
	dynamic = false;
	
	if(p == NULL) {
		rootMenu = this;
		parent = this;
	}
	else {
		rootMenu = p->rootMenu;
		parent = p;
	}

	activeMenu = rootMenu;
	offset = 0;
	opacity = 255;
	highlightColor = graphicsNS::RED;
	normalColor = graphicsNS::WHITE;
	menuAnchor = D3DXVECTOR2(270,10);
	input = i;
	verticalOffset = 30;
	linePtr = 0;
	menuState = 0;
	lastMenuState = 0;
	selectedItem = -1;
	graphics = g;
	menuItemFont = new TextDX();
	menuHeadingFont = new TextDX();
	menuItemFontHighlight = new TextDX();
	if(menuItemFont->initialize(graphics, 15, true, false, "Calibri") == false)
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing menuItem font"));
	if(menuItemFontHighlight->initialize(graphics, 18, true, false, "Calibri") == false)
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing menuItem font"));
	if(menuHeadingFont->initialize(graphics, 25, true, false, "Calibri") == false)
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing menuHeading font"));
	menuHeadingFont->setFontColor(normalColor);
	menuItemFont->setFontColor(normalColor);
	menuItemFontHighlight->setFontColor(highlightColor);
	upDepressedLastFrame = false;
	downDepressedLastFrame = false;
}

void Menu::initialize(Graphics *g, Input *i, Menu* p, std::vector<ItemInstance>* v, std::string h)
{
	// Single layer menu
	dynamic = true;
	heading = h;
	items = v;

	rootMenu = NULL;
	parent = NULL;

	activeMenu = this;
	offset = 0;
	opacity = 255;
	highlightColor = graphicsNS::RED;
	normalColor = graphicsNS::WHITE;
	menuAnchor = D3DXVECTOR2(270,10);
	input = i;
	verticalOffset = 30;
	linePtr = 0;
	menuState = 0;
	lastMenuState = 0;
	selectedItem = -1;
	graphics = g;
	menuItemFont = new TextDX();
	menuHeadingFont = new TextDX();
	menuItemFontHighlight = new TextDX();
	if(menuItemFont->initialize(graphics, 15, true, false, "Calibri") == false)
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing menuItem font"));
	if(menuItemFontHighlight->initialize(graphics, 18, true, false, "Calibri") == false)
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing menuItem font"));
	if(menuHeadingFont->initialize(graphics, 25, true, false, "Calibri") == false)
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing menuHeading font"));
	menuHeadingFont->setFontColor(normalColor);
	menuItemFont->setFontColor(normalColor);
	menuItemFontHighlight->setFontColor(highlightColor);
	upDepressedLastFrame = false;
	downDepressedLastFrame = false;
}

void Menu::update()
{
	if(dynamic) {
		menuState=0;
		if (input->wasKeyPressed(VK_UP))
		{
			linePtr--;
		}
		if (input->wasKeyPressed(VK_DOWN))
		{
			linePtr++;
		}

		if (linePtr > (int)(items->size()-1))
			linePtr = 0;
		if (linePtr < 0)
			linePtr = items->size()-1;

		if (input->wasKeyPressed(VK_RETURN)) {
			menuState = linePtr;
		}
	} else {
		menuState=0;
		if (input->wasKeyPressed(VK_UP))
		{
			linePtr--;
		}
		if (input->wasKeyPressed(VK_DOWN))
		{
			linePtr++;
		}

		if (linePtr > (int)(menuItems.size()-1))
			linePtr = 0;
		if (linePtr < 0)
			linePtr = menuItems.size()-1;

		if (input->wasKeyPressed(VK_RETURN)) {
			if(children[linePtr] != NULL) {
				selectedItem = linePtr;
				activeMenu = children[linePtr];
				activeMenu->setOffset(MENU_OFFSET);	// set it forward and move it back
				menuState = (lastMenuState-(lastMenuState % MODDER)+linePtr+1)*MODDER;
			} else {
				menuState = lastMenuState-(lastMenuState % MODDER) + linePtr+1;
			}
			lastMenuState = menuState;
		}
		if (input->wasKeyPressed(VK_BACK)) {
			if(activeMenu != parent && parent != NULL) {
				activeMenu = parent;
				selectedItem = -1;
				setOpacity(255);
				menuState = lastMenuState/MODDER;
				lastMenuState = menuState;
			}
		}
	}
}


void Menu::displayMenu(float frametime)
{	
	if(dynamic) {
		menuHeadingFont->print(heading, menuAnchor.x, menuAnchor.y);

		for(int i=0;i<items->size();++i) {
			if (linePtr==i)	// Only highlight the active menu
			menuItemFontHighlight->print((*items)[i].getName(), activeMenu->menuAnchor.x, activeMenu->menuAnchor.y+verticalOffset*(i+1));
		else
			menuItemFont->print((*items)[i].getName(), activeMenu->menuAnchor.x, activeMenu->menuAnchor.y+verticalOffset*(i+1));
		}

		if(this == activeMenu) {	// Constantly move to 0
			if(activeMenu->getOffset() > 0)
				activeMenu->setOffset(activeMenu->getOffset()-300.0f*frametime);
			if(activeMenu->getOffset() < 0)
				activeMenu->setOffset(activeMenu->getOffset()+300.0f*frametime);
		}

	} else {
		DWORD c1 = menuHeadingFont->getFontColor();
		DWORD c2 = menuItemFont->getFontColor();
		if(this != activeMenu) {	// FADE IT BRO
			menuHeadingFont->setFontColor(SETCOLOR_ARGB(opacity,255,255,255));
			menuItemFont->setFontColor(SETCOLOR_ARGB(opacity,255,255,255));
		}

		menuHeadingFont->print(this->menuHeading, menuAnchor.x+this->getOffset(), menuAnchor.y);

		for(int i=0;i<this->menuItems.size();++i) {
			if (linePtr==i && this == activeMenu)	// Only highlight the active menu
			menuItemFontHighlight->print(this->menuItems[i], activeMenu->menuAnchor.x+this->getOffset(), activeMenu->menuAnchor.y+verticalOffset*(i+1));
		else
			menuItemFont->print(this->menuItems[i], activeMenu->menuAnchor.x+this->getOffset(), activeMenu->menuAnchor.y+verticalOffset*(i+1));
		}

		if(this == activeMenu) {	// Constantly move to 0
			if(activeMenu->getOffset() > 0)
				activeMenu->setOffset(activeMenu->getOffset()-300.0f*frametime);
			if(activeMenu->getOffset() < 0)
				activeMenu->setOffset(activeMenu->getOffset()+300.0f*frametime);
		}

		if(parent != NULL && parent != this) {	// If it's not the root menu
			parent->setOffset(getOffset()-MENU_OFFSET);	// All relative to the menu above it

			float op = 255+parent->getOffset();
			if(op<25) op = 25; else if (op>255) op = 255;
			parent->setOpacity(op);
			parent->displayMenu(frametime);
		}

		menuHeadingFont->setFontColor(c1);
		menuItemFont->setFontColor(c2);
	}

}

void Menu::setMenuItems(std::vector<std::string> v) {
	menuItems = v;
	children.resize(v.size());
	for(int i=0;i<children.size();++i) {
		children[i] = NULL;
	}
}