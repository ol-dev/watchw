// implementation of LCD rendrer
#include "LCDRenderer.h"

LCDRenderer::LCDRenderer (LiquidCrystal &lcd) : _lcd(lcd) {}
    
void LCDRenderer::render(Menu const& menu) const
{
  
    _lcd.clear();
    _lcd.setCursor(0,0);
    _lcd.print(menu.get_name());
    _lcd.setCursor(0,1);
    menu.get_current_component()->render(*this);
}

void  LCDRenderer::render_menu_item(MenuItem const& menu_item) const
{
    _lcd.print(menu_item.get_name());
}

void  LCDRenderer::render_back_menu_item(BackMenuItem const& menu_item) const
{
    _lcd.print(menu_item.get_name());
}

void  LCDRenderer::render_numeric_menu_item(NumericMenuItem const& menu_item) const
{
    _lcd.print(menu_item.get_name());
}

void  LCDRenderer::render_menu(Menu const& menu) const
{
    _lcd.print(menu.get_name());
}


