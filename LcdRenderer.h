#ifndef _LCD_RENDERER_H
#define _LCD_RENDERER_H

#include <LiquidCrystal.h>
#include <MenuSystem.h>


class CustomNumericMenuItem;

class LCDRenderer : public MenuComponentRenderer
{

private:
    LiquidCrystal& _lcd;   
public:
    LCDRenderer(LiquidCrystal &lcd);
    virtual void render(Menu const& menu) const;
    virtual void render_menu_item(MenuItem const& menu_item) const;
    virtual void render_back_menu_item(BackMenuItem const& menu_item) const;
    virtual void render_numeric_menu_item(NumericMenuItem const& menu_item) const;
    //virtual void render_custom_numeric_menu_item(CustomNumericMenuItem const& menu_item) const;
    virtual void render_menu(Menu const& menu) const;
};

#endif // LCD_RENDERER_H

