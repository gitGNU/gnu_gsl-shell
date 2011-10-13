
local bit = require 'bit'

local LAYOUT = require 'fox-layout'
local DECOR  = require 'fox-decor'
local FRAME  = require 'fox-frame'
local SEL    = require 'fox-selector'

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
}

local function parse_gen_options(table, opts)
   if not opts then return 0 end
   local r = 0
   for i, name in ipairs(opts) do
      local x = table[name]
      r = bit.bor(r, x)
   end
   return r
end

local function parse_options(spec)
   local lay_opts = parse_gen_options(LAYOUT, spec.layout)
   local sty_opts = parse_gen_options(FRAME,  spec.style)
   return bit.bor(lay_opts, sty_opts)
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

local function raw_ctor(type_id)
   local id = get_element_id()
   return { type_id = type_id, id = id }
end

local function base_ctor(type_id, spec)
   local id = get_element_id()
   local handlers = parse_handlers(spec, id)
   return { type_id  = type_id,
	    id       = id,
	    name     = spec.name,
	    handlers = handlers }
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
   ctor.args = { parse_options(spec) }
   return parse_childs(ctor, ctor.id, spec)
end


function M.HorizontalFrame(spec)
   local ctor = base_ctor(GUI.HORIZONTAL_FRAME, spec)
   ctor.args = { parse_options(spec) }
   return parse_childs(ctor, ctor.id, spec)
end

function M.TextField(spec)
   local ctor = base_ctor(GUI.TEXT_FIELD, spec)
   ctor.args = { spec.columns or 12 }
   return { ctor }
end

function M.Label(spec)
   local ctor = base_ctor(GUI.LABEL, spec)
   ctor.args = { spec.text or '<Unspecified>' }
   return { ctor }
end

function M.Button(spec)
   local ctor = base_ctor(GUI.BUTTON, spec)
   ctor.args = { spec.text or "<Unspecified>" }
   return { ctor }
end

function M.Canvas(spec)
   local ctor = base_ctor(GUI.CANVAS, spec)
   ctor.args = { parse_options(spec) }
   return { ctor }
end

function M.MenuCommand(spec)
   local ctor = base_ctor(GUI.MENU_COMMAND, spec)
   ctor.args = { spec[1] or '<Unspecified>' }
   return { ctor }
end

function M.MenuTitle(spec)
   local ctor = base_ctor(GUI.MENU_PANE, spec)
   local ctors = parse_childs(ctor, ctor.id, spec)

   -- MenuTitle creator line
   mtctor = raw_ctor(GUI.MENU_TITLE)
   mtctor.args = { spec.text or '<Unspecified>', ctor.id, parse_options(spec) }

   ctors[#ctors+1] = mtctor
   return ctors
end

function M.RadioButton(spec)
   local ctor = base_ctor(GUI.RADIO_BUTTON, spec)
   ctor.args = { spec.text or "<Unspecified>" }
   return { ctor }
end

function M.CheckButton(spec)
   local ctor = base_ctor(GUI.CHECK_BUTTON, spec)
   ctor.args = { spec.text or "<Unspecified>" }
   return { ctor }
end

function M.MenuBar(spec)
   local wctor = base_ctor(GUI.MENU_BAR, spec)
   wctor.args = { parse_options(spec) }

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
