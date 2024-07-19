import iup
import ../core/practice, ../core/config

proc menuItemListPractices(ih: PIhandle): cint {.cdecl.} =
  let recipes = listRecipes()
  var msg = "Available practices:\n\n"
  for recipe in recipes:
    msg.add("  " & recipe & "\n")
  iup.message("Practices", msg)
  IUP_DEFAULT

proc menuItemRunPractice(ih: PIhandle): cint {.cdecl.} =
  # Implement practice selection and running
  iup.message("Run Practice", "Practice selection and running not implemented yet")
  IUP_DEFAULT

proc menuItemUpdatePractices(ih: PIhandle): cint {.cdecl.} =
  if updateRecipes():
    iup.message("Update", "Recipes updated successfully")
  else:
    iup.message("Update", "Failed to update recipes")
  IUP_DEFAULT

proc menuItemConfigurePractice(ih: PIhandle): cint {.cdecl.} =
  # Implement practice selection and configuration
  iup.message("Configure Practice", "Practice configuration not implemented yet")
  IUP_DEFAULT

proc menuItemExit(ih: PIhandle): cint {.cdecl.} =
  IUP_CLOSE

proc guiMain*() =
  discard iup.open(nil, nil)
  
  var itemListPractices = iup.item("List Practices", nil)
  var itemRunPractice = iup.item("Run Practice", nil)
  var itemUpdatePractices = iup.item("Update Practices", nil)
  var itemConfigurePractice = iup.item("Configure Practice", nil)
  var itemExit = iup.item("Exit", nil)

  discard iup.setCallback(itemListPractices, "ACTION", cast[ICallback](menuItemListPractices))
  discard iup.setCallback(itemRunPractice, "ACTION", cast[ICallback](menuItemRunPractice))
  discard iup.setCallback(itemUpdatePractices, "ACTION", cast[ICallback](menuItemUpdatePractices))
  discard iup.setCallback(itemConfigurePractice, "ACTION", cast[ICallback](menuItemConfigurePractice))
  discard iup.setCallback(itemExit, "ACTION", cast[ICallback](menuItemExit))

  var fileMenu = iup.menu(
    itemListPractices,
    itemRunPractice,
    itemUpdatePractices,
    itemConfigurePractice,
    iup.separator(),
    itemExit,
    nil
  )

  var mainMenu = iup.menu(iup.subMenu("File", fileMenu), nil)
  discard iup.setHandle("mainMenu", mainMenu)

  var label = iup.label("Welcome to Sadhana")
  iup.setAttribute(label, "ALIGNMENT", "ACENTER")

  var vbox = iup.vbox(label, nil)
  iup.setAttribute(vbox, "ALIGNMENT", "ACENTER")
  iup.setAttribute(vbox, "GAP", "10")
  iup.setAttribute(vbox, "MARGIN", "10x10")

  var dlg = iup.dialog(vbox)
  iup.setAttribute(dlg, "TITLE", "Sadhana")
  iup.setAttribute(dlg, "SIZE", "300x200")
  iup.setAttribute(dlg, "MENU", "mainMenu")

  discard iup.showXY(dlg, IUP_CENTER, IUP_CENTER)
  discard iup.mainLoop()
  iup.close()

when isMainModule:
  guiMain()