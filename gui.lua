
local bit = require 'bit'

local LAYOUT = require 'fox-layout'
local DECOR  = require 'fox-decor'
local SEL    = require 'fox-selector'

local create = gsl.fox_window

local M = {}

local GUI = {
   MAIN_WINDOW      = 0,
   HORIZONTAL_FRAME = 1,
   VERTICAL_FRAME   = 2,
   LABEL            = 3,
   TEXT_FIELD       = 4,
   BUTTON           = 5,
   CANVAS           = 6,
}

local function parse_layout_options(opts)
   local r = 0
   for i, name in ipairs(opts) do
      local x = LAYOUT[name]
      r = bit.bor(r, x)
   end
   return r
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

function M.MainWindow(spec)
   local w, h = spec.width or 800, spec.height or 600
   local ctor = { 
      type_id = GUI.MAIN_WINDOW,
      id = get_element_id(),
      args = {spec.title or 'Test Window', DECOR.ALL, w or 0, h or 0}
   }
   ctor.body = parse_childs(nil, ctor.id, spec)
   return ctor
end

local function base_ctor(type_id, spec)
   return { type_id = type_id,
	    id      = get_element_id(),
	    name    = spec.name }
end
      

function M.VerticalFrame(spec)
   local ctor = base_ctor(GUI.VERTICAL_FRAME, spec)
   ctor.args = { parse_layout_options(spec.layout) }
   return parse_childs(ctor, ctor.id, spec)
end


function M.HorizontalFrame(spec)
   local ctor = base_ctor(GUI.HORIZONTAL_FRAME, spec)
   ctor.args = { parse_layout_options(spec.layout) }
   return parse_childs(ctor, ctor.id, spec)
end

function M.TextField(spec)
   local ctor = base_ctor(GUI.TEXT_FIELD, spec)
   ctor.args = { spec.columns or 12 }
   return { ctor }
end

function M.Button(spec)
   local ctor = base_ctor(GUI.BUTTON, spec)
   ctor.args = { spec.text or "<Unspecified>" }
   if spec.onCommand then
      local hs = {id = ctor.id}
      hs[#hs+1] = {SEL.COMMAND, spec.onCommand}
      ctor.handlers = hs
   end
   return { ctor }
end

function M.Canvas(spec)
   local ctor = base_ctor(GUI.CANVAS, spec)
   ctor.args = { }
   return { ctor }
end

function M.Create(ctors)
   return create(ctors)
end

return M
