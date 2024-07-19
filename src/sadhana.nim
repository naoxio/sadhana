import os
import cli/cli_main
import tui/tui_main
import gui/gui_main

when isMainModule:
  if paramCount() > 0 and paramStr(1) == "gui":
    guiMain()
  elif paramCount() > 0 and paramStr(1) == "tui":
    tuiMain()
  else:
    cliMain()

