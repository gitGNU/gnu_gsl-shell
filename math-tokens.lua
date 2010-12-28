-- tokens.js
-- 2009-05-17

-- (c) 2006 Douglas Crockford

-- Produce an array of simple token objects from a string.
-- A simple token object contains these members:
--      type: 'name', 'string', 'number', 'operator'
--      value: string or number value of the token
--      from: index of first character of the token
--      to: index of the last character + 1

-- Comments of the -- type are ignored.

-- Operators are by default single characters. Multicharacter
-- operators can be made by supplying a string of prefix and
-- suffix characters.
-- characters. For example,
--      '<>+-&', '=>&:'
-- will match any of these:
--      <=  >>  >>>  <>  >=  +: -: &: &&: &&


local function push(ls, e)
   ls[#ls+1] = e
end

function string_tokens(this) 
   local from                   -- The index of the start of the token.
   local i = 1                  -- The index of the current character.
   local length = #this

   local result = {}            -- An array to hold the results.

   local match = function(patt) return this:match('^' .. patt, i) end
   local get = function() return this:sub(from, i-1) end

   local add_match = function(patt)
			local m = this:match('^' .. patt, i)
			if m then
			   i = i + #m
			   return m
			end
		     end

   -- Make a token object.
   local make = function (type, value) 
		   return {type= type, value= value, from= from, to= i}
		end

   -- Begin tokenization. If the source string is empty, return nothing.

   if not this then return end

-- Loop through this text, one character at a time.

   while i <= length do 
      from = i

-- Ignore whitespace.

      if match '%s' then
	 local ss = this:match('^%s+', i)
	 i = i + #ss

-- name.

      elseif match '%a' then
	 add_match '%a[%a%d_]*'
	 push(result, make('name', get()))

-- number.

-- A number cannot start with a decimal point. It must start with a digit,
-- possibly '0'.

      elseif match '%d' then
	 add_match '%d+'
	 add_match '.%d+'
	 add_match 'eE[+-]?%d+'

-- Make sure the next character is not a letter.

	 if add_match '%a' then
	    error(string.format('bad number: %s', get()))
	 end

-- Convert the string value to a number. If it is finite, then it is a good
-- token.

	 local str = get()
	 local n = tonumber(str)
	 if n then
	    push(result, make('number', n))
	 else
	    error(string.format('bad number: %s', str))
	 end


-- single-character operator

      else
	 add_match '.'
	 push(result, make('operator', get()))
      end
   end
   return result
end
