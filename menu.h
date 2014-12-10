
              // file is included in more than one place
#define WIN32_LEAN_AND_MEAN
#ifndef MENU_H                 // Prevent multiple definitions if this 
#define MENU_H 

class Menu;

#include "graphics.h"
#include "constants.h"
#include "textDX.h"
#include <string>
#include <sstream>
#include <vector>
#include "input.h"
#include "ItemInstance.h"

namespace menuNS
{ }

// inherits from Entity class
class Menu 
{
private:
   TextDX *menuItemFont;
   TextDX *menuItemFontHighlight;
   TextDX *menuHeadingFont;
   Input   *input;         // pointer to the input system
   Graphics *graphics;
   int selectedItem;
   float offset;
   int opacity;
   std::vector<Menu*> children;
   Menu* parent;
   Menu* rootMenu;
   static Menu* activeMenu;
   std::string menuHeading;
   std::vector<std::string> menuItems;
   D3DXVECTOR2 menuAnchor;
   int verticalOffset;
   int linePtr;
   COLOR_ARGB highlightColor ;
   COLOR_ARGB normalColor;
   bool upDepressedLastFrame;
   bool downDepressedLastFrame;
   bool dynamic;
   static int menuState;
   static int lastMenuState;
   std::string heading;
   std::vector<ItemInstance>* items;

   float shift;

   int MODDER;

public:
    // constructor
    Menu();
	void initialize(Graphics *g, Input *i, Menu* p);
	void initialize(Graphics *g, Input *i, Menu* p, std::vector<ItemInstance>* v, std::string h);
	void update();
	int getSelectedItem() {return selectedItem;}
	void displayMenu(float frametime);
	void setMenuHeading(std::string s) { menuHeading = s; }
	std::vector<Menu*> getChildren() { return children; }
	void setChildren(std::vector<Menu*> v) { children = v; }
	std::vector<std::string> getMenuItems() { return menuItems; }
	void setMenuItems(std::vector<std::string> v);
	Menu* getActiveMenu() { return activeMenu; }
	float getOffset() { return offset; }
	void setOffset(float o) { offset = o; }
	void setOpacity(int o) { opacity = o; }
	int getMenuState() { return menuState; }
	
};
#endif

