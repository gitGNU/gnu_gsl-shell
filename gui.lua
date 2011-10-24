
local bor = (require 'bit').bor

local LAYOUT  = require 'fox-layout'
local DECOR   = require 'fox-decor'
local FRAME   = require 'fox-frame'
local SEL     = require 'fox-selector'
local OPTIONS = require 'fox-options'
local OP      = require 'fox-method'

local create = gsl.fox_window

local M = {}

local GUI = {
   MAIN_WINDOW      = 1,
   HORIZONTAL_FRAME = 2,
   VERTICAL_FRAME   = 3,
   LABEL            = 4,
   TEXT_FIELD       = 5,
   BUTTON           = 6,
   CANVAS           = 7,
   MENU_BAR         = 8,
   MENU_TITLE       = 9,
   MENU_PANE        = 10,
   MENU_COMMAND     = 11,
   DIALOG_BOX       = 12,
   RADIO_BUTTON     = 13,
   CHECK_BUTTON     = 14,
   COMBO_BOX        = 15,
}

local BT_OPTS = bor(OPTIONS.JUSTIFY_NORMAL,OPTIONS.ICON_BEFORE_TEXT)

option_default = {
   LAYOUT = {
      [GUI.BUTTON] = BT_OPTS,
      [GUI.CHECK_BUTTON] = BT_OPTS,
      [GUI.RADIO_BUTTON] = BT_OPTS,
   },
   FRAME = {
      [GUI.BUTTON] = bor(FRAME.RAISED,FRAME.THICK),
      [GUI.TEXT_FIELD] = bor(FRAME.SUNKEN,FRAME.THICK)
   },
}

local function parse_gen_options(table, opts)
   if not opts then return 0 end
   local r = 0
   for i, name in ipairs(opts) do r = bor(r, table[name]) end
   return r
end

local function parse_options(spec, type_id)
   local lay_opts = spec.layout and parse_gen_options(LAYOUT, spec.layout) or (option_default.LAYOUT[type_id] or 0)
   local sty_opts = spec.style and parse_gen_options(FRAME, spec.style) or (option_default.FRAME[type_id] or 0)
   return bor(lay_opts, sty_opts)
end

local current_element_id = 0

local function get_element_id()
   local id = current_element_id
   current_element_id = current_element_id + 1
   return id
end

local function get_child_ctor(ctors, parent_id, spec)
   for i, ctor_childs in ipairs(spec) do
      for k, c in ipairs(ctor_childs) do
	 if not c.parent then c.parent = parent_id end
	 ctors[#ctors + 1] = c
      end
   end
end

local function parse_childs(ctor, parent_id, spec)
   local ctors = ctor and { ctor } or {}
   get_child_ctor(ctors, parent_id, spec)
   return ctors
end

local handler_table = {
   onCommand           = SEL.COMMAND,
   onPaint             = SEL.PAINT,
   onLeftButtonPress   = SEL.LEFTBUTTONPRESS,
   onLeftButtonRelease = SEL.LEFTBUTTONRELEASE,
   onMotion            = SEL.MOTION,
   onUpdate            = SEL.UPDATE,
}

local function parse_handlers(spec, id)
   local hs = { }
   for k, v in pairs(spec) do
      local sel = handler_table[k]
      if sel then hs[#hs+1] = {sel, v} end
   end
   if #hs > 0 then return hs end
end

local function base_ctor(type_id, spec)
   local id = get_element_id()
   local opts = parse_options(spec, type_id)
   local handlers = parse_handlers(spec, id)
   local init_func = spec.onCreate
   return { type_id  = type_id,
	    id       = id,
	    name     = spec.name,
	    handlers = handlers,
	    options  = opts,
	    init     = init_func, }
end

function M.MainWindow(spec)
   local w, h = spec.width or 800, spec.height or 600
   local ctor = { 
      type_id = GUI.MAIN_WINDOW,
      id = get_element_id(),
      name = spec.name,
      args = {spec.title or 'Test Window', DECOR.ALL, w or 0, h or 0}
   }
   ctor.body = parse_childs(nil, ctor.id, spec)
   return ctor
end

function M.Dialog(spec)
   local w, h = spec.width or 640, spec.height or 480
   local ctor = { 
      type_id = GUI.DIALOG_BOX,
      id = get_element_id(),
      name = spec.name,
      args = {spec.title or 'Test Dialog', DECOR.ALL, w or 0, h or 0}
   }
   ctor.body = parse_childs(nil, ctor.id, spec)
   return ctor
end

function M.VerticalFrame(spec)
   local ctor = base_ctor(GUI.VERTICAL_FRAME, spec)
   return parse_childs(ctor, ctor.id, spec)
end


function M.HorizontalFrame(spec)
   local ctor = base_ctor(GUI.HORIZONTAL_FRAME, spec)
   return parse_childs(ctor, ctor.id, spec)
end

function M.TextField(spec)
   local ctor = base_ctor(GUI.TEXT_FIELD, spec)
   ctor.args = { spec.columns or 12 }
   return { ctor }
end

local function generic_text_ctor(type_id, spec)
   local ctor = base_ctor(type_id, spec)
   ctor.args = { spec.text or '<Unspecified>' }
   return ctor
end

function M.Label(spec)
   return { generic_text_ctor(GUI.LABEL, spec) }
end

function M.Button(spec)
   return { generic_text_ctor(GUI.BUTTON, spec) }
end

function M.Canvas(spec)
   local ctor = base_ctor(GUI.CANVAS, spec)
   return { ctor }
end

function M.MenuCommand(spec)
   local ctor = base_ctor(GUI.MENU_COMMAND, spec)
   ctor.args = { spec[1] or '<Unspecified>' }
   return { ctor }
end

function M.MenuTitle(spec)
   mtspec = { layout = spec.layout, style = spec.style }
   spec.layout = nil
   spec.style = nil

   local ctor = base_ctor(GUI.MENU_PANE, spec)
   local ctors = parse_childs(ctor, ctor.id, spec)

   -- MenuTitle creator line
   mtctor = base_ctor(GUI.MENU_TITLE, mtspec)
   mtctor.args = { spec.text or '<Unspecified>', ctor.id }

   ctors[#ctors+1] = mtctor
   return ctors
end

function M.RadioButton(spec)
   return { generic_text_ctor(GUI.RADIO_BUTTON, spec) }
end

function M.RadioButtonSet(labels)
   local id = {}
   local choice
   local set = function(k) return function(w) choice = k end end
   local upd = function(k)
		  return function(w)
			    local msg = (choice == k and OP.CHECK or OP.UNCHECK)
			    w:handle(id[k], msg)
			 end
	       end

   local ctls = {}

   for i, text in ipairs(labels) do
      local spec = {
	 text = text,
	 onCommand = set(i),
	 onUpdate  = upd(i),
      }
      local ctor = generic_text_ctor(GUI.RADIO_BUTTON, spec)
      id[i] = ctor.id
      ctls[#ctls+1] = ctor
   end

   return ctls
end

function M.CheckButton(spec)
   return { generic_text_ctor(GUI.CHECK_BUTTON, spec) }
end

function M.ComboBox(spec)
   return { generic_text_ctor(GUI.COMBO_BOX, spec) }
end

function M.MenuBar(spec)
   local wctor = base_ctor(GUI.MENU_BAR, spec)

   local ctors = parse_childs(wctor, wctor.id, spec)
   for _, c in ipairs(ctors) do
      if c.type_id == GUI.MENU_PANE then c.parent = nil end
   end

   return ctors
end

function M.Create(ctors)
   return create(ctors)
end

return M
