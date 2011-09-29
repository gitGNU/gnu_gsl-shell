#include <string.h>

#include "gui_element.h"

int text_field::handle(lua_State* L, gslshell::ret_status& st)
{
  const char *method = lua_tostring(L, 3);

  if (strcmp(method, "set_text") == 0) {
    const char *text = lua_tostring(L, 4);
    m_widget->setText(text);
    return 0;
  } else if (strcmp(method, "get_text") == 0) {
    FXString text = m_widget->getText();
    lua_pushstring(L, text.text());
    return 1;
  }

  st.error("invalid request", "text_field method");
  return 0;
}

int gui_window::handle(lua_State* L, gslshell::ret_status& st)
{
  const char *method = lua_tostring(L, 3);

  if (strcmp(method, "close") == 0) {
    m_widget->handle(m_widget, FXSEL(SEL_CLOSE,0), 0);
    return 0;
  }

  st.error("invalid request", "window method");
  return 0;
}
