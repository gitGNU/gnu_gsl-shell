
local SEL = {}

SEL.NONE = 0
SEL.KEYPRESS = 1                         -- Key pressed
SEL.KEYRELEASE = 2                       -- Key released
SEL.LEFTBUTTONPRESS = 3                  -- Left mouse button pressed
SEL.LEFTBUTTONRELEASE = 4                -- Left mouse button released
SEL.MIDDLEBUTTONPRESS = 5                -- Middle mouse button pressed
SEL.MIDDLEBUTTONRELEASE = 6              -- Middle mouse button released
SEL.RIGHTBUTTONPRESS = 7                 -- Right mouse button pressed
SEL.RIGHTBUTTONRELEASE = 8               -- Right mouse button released
SEL.MOTION = 9                           -- Mouse motion
SEL.ENTER = 10                            -- Mouse entered window
SEL.LEAVE = 11                            -- Mouse left window
SEL.FOCUSIN = 12                          -- Focus into window
SEL.FOCUSOUT = 13                         -- Focus out of window
SEL.KEYMAP = 14
SEL.UNGRABBED = 15                        -- Lost the grab (Windows)
SEL.PAINT = 16                            -- Must repaint window
SEL.CREATE = 17
SEL.DESTROY = 18
SEL.UNMAP = 19                            -- Window was hidden
SEL.MAP = 20                              -- Window was shown
SEL.CONFIGURE = 21                        -- Resize
SEL.SELECTION_LOST = 22                   -- Widget lost selection
SEL.SELECTION_GAINED = 23                 -- Widget gained selection
SEL.SELECTION_REQUEST = 24                -- Inquire selection data
SEL.RAISED = 25                           -- Window to top of stack
SEL.LOWERED = 26                          -- Window to bottom of stack
SEL.CLOSE = 27                            -- Close window
SEL.DELETE = 28                           -- Delete window
SEL.MINIMIZE = 29                         -- Iconified
SEL.RESTORE = 30                          -- No longer iconified or maximized
SEL.MAXIMIZE = 31                         -- Maximized
SEL.UPDATE = 32                           -- GUI update
SEL.COMMAND = 33                          -- GUI command
SEL.CLICKED = 34                          -- Clicked
SEL.DOUBLECLICKED = 35                    -- Double-clicked
SEL.TRIPLECLICKED = 36                    -- Triple-clicked
SEL.MOUSEWHEEL = 37                       -- Mouse wheel
SEL.CHANGED = 38                          -- GUI has changed
SEL.VERIFY = 39                           -- Verify change
SEL.DESELECTED = 40                       -- Deselected
SEL.SELECTED = 41                         -- Selected
SEL.INSERTED = 42                         -- Inserted
SEL.REPLACED = 43                         -- Replaced
SEL.DELETED = 44                          -- Deleted
SEL.OPENED = 45                           -- Opened
SEL.CLOSED = 46                           -- Closed
SEL.EXPANDED = 47                         -- Expanded
SEL.COLLAPSED = 48                        -- Collapsed
SEL.BEGINDRAG = 49                        -- Start a drag
SEL.ENDDRAG = 50                          -- End a drag
SEL.DRAGGED = 51                          -- Dragged
SEL.LASSOED = 52                          -- Lassoed
SEL.TIMEOUT = 53                          -- Timeout occurred
SEL.SIGNAL = 54                           -- Signal received
SEL.CLIPBOARD_LOST = 55                   -- Widget lost clipboard
SEL.CLIPBOARD_GAINED = 56                 -- Widget gained clipboard
SEL.CLIPBOARD_REQUEST = 57                -- Inquire clipboard data
SEL.CHORE = 58                            -- Background chore
SEL.FOCUS_SELF = 59                       -- Focus on widget itself
SEL.FOCUS_RIGHT = 60                      -- Focus moved right
SEL.FOCUS_LEFT = 61                       -- Focus moved left
SEL.FOCUS_DOWN = 62                       -- Focus moved down
SEL.FOCUS_UP = 63                         -- Focus moved up
SEL.FOCUS_NEXT = 64                       -- Focus moved to next widget
SEL.FOCUS_PREV = 65                       -- Focus moved to previous widget
SEL.DND_ENTER = 66                        -- Drag action entering potential drop target
SEL.DND_LEAVE = 67                        -- Drag action leaving potential drop target
SEL.DND_DROP = 68                         -- Drop on drop target
SEL.DND_MOTION = 69                       -- Drag position changed over potential drop target
SEL.DND_REQUEST = 70                      -- Inquire drag and drop data
SEL.IO_READ = 71                          -- Read activity on a pipe
SEL.IO_WRITE = 72                         -- Write activity on a pipe
SEL.IO_EXCEPT = 73                        -- Except activity on a pipe
SEL.PICKED = 74                           -- Picked some location
SEL.QUERY_TIP = 75                        -- Message inquiring about tooltip
SEL.QUERY_HELP = 76                       -- Message inquiring about statusline help
SEL.DOCKED = 77                           -- Toolbar docked
SEL.FLOATED = 78                          -- Toolbar floated
SEL.SESSION_NOTIFY = 79                   -- Session is about to close
SEL.SESSION_CLOSED = 80                   -- Session is closed

return SEL