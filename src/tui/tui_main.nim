import nimbox
import strutils
import ../core/practice, ../core/config

type
  MenuItem = enum
    miListPractices, miRunPractice, miUpdatePractices, miConfigurePractice, miExit

  Screen = enum
    scMain, scListPractices, scRunPractice, scConfigurePractice

const MenuItems = [
  "List Practices",
  "Run Practice",
  "Update Practices",
  "Configure Practice",
  "Exit"
]

proc drawMenu(nb: Nimbox, selected: int) =
  nb.clear()
  let width = nb.width
  let height = nb.height

  # Draw title
  let title = "Sadhana"
  let titleX = (width - title.len) div 2
  nb.print(titleX, 1, title, clrWhite, clrDefault)

  # Draw menu items
  for i, item in MenuItems:
    let y = 3 + i
    let x = (width - item.len) div 2
    if i == selected:
      nb.print(x, y, item, clrBlack, clrWhite)
    else:
      nb.print(x, y, item, clrWhite, clrDefault)

  nb.present()

proc drawList(nb: Nimbox, items: seq[string], selected: int, title: string) =
  nb.clear()
  let width = nb.width
  let height = nb.height

  # Draw title
  let titleX = (width - title.len) div 2
  nb.print(titleX, 1, title, clrWhite, clrDefault)

  # Draw items
  for i, item in items:
    let y = 3 + i
    let x = 2
    if i == selected:
      nb.print(x, y, item, clrBlack, clrWhite)
    else:
      nb.print(x, y, item, clrWhite, clrDefault)

  nb.print(2, height - 2, "Press 'q' to go back", clrWhite, clrDefault)
  nb.present()

proc showMessage(nb: Nimbox, message: string) =
  nb.clear()
  let width = nb.width
  let height = nb.height

  let lines = message.split('\n')
  for i, line in lines:
    let y = (height div 2) - (lines.len div 2) + i
    let x = (width - line.len) div 2
    nb.print(x, y, line, clrWhite, clrDefault)

  nb.print(2, height - 2, "Press any key to continue", clrWhite, clrDefault)
  nb.present()
  discard nb.peekEvent(timeout = -1)

proc listPracticesScreen(nb: Nimbox) =
  let recipes = listRecipes()
  var selected = 0
  while true:
    drawList(nb, recipes, selected, "Available Practices")
    let evt = nb.peekEvent(timeout = -1)
    case evt.kind
    of EventType.Key:
      case evt.sym
      of Symbol.Escape:
        return
      of Symbol.Up:
        selected = (selected - 1 + recipes.len) mod recipes.len
      of Symbol.Down:
        selected = (selected + 1) mod recipes.len
      else:
        discard
    else:
      discard

proc runPracticeScreen(nb: Nimbox) =
  let recipes = listRecipes()
  var selected = 0
  while true:
    drawList(nb, recipes, selected, "Select Practice to Run")
    let evt = nb.peekEvent(timeout = -1)
    case evt.kind
    of EventType.Key:
      case evt.sym
      of Symbol.Escape:
        return
      of Symbol.Up:
        selected = (selected - 1 + recipes.len) mod recipes.len
      of Symbol.Down:
        selected = (selected + 1) mod recipes.len
      of Symbol.Enter:
        if runPractice(recipes[selected]):
          showMessage(nb, "Practice completed successfully")
        else:
          showMessage(nb, "Failed to run practice")
        return
      else:
        discard
    else:
      discard

proc configurePracticeScreen(nb: Nimbox) =
  let recipes = listRecipes()
  var selected = 0
  while true:
    drawList(nb, recipes, selected, "Select Practice to Configure")
    let evt = nb.peekEvent(timeout = -1)
    case evt.kind
    of EventType.Key:
      case evt.sym
      of Symbol.Escape:
        return
      of Symbol.Up:
        selected = (selected - 1 + recipes.len) mod recipes.len
      of Symbol.Down:
        selected = (selected + 1) mod recipes.len
      of Symbol.Enter:
        if configurePractice(recipes[selected]):
          showMessage(nb, "Practice configured successfully")
        else:
          showMessage(nb, "Failed to configure practice")
        return
      else:
        discard
    else:
      discard

proc tuiMain*() =
  var nb = newNimbox()
  defer: nb.shutdown()

  var selected = 0
  var currentScreen = scMain

  while true:
    case currentScreen
    of scMain:
      drawMenu(nb, selected)
      let evt = nb.peekEvent(timeout = -1)
      case evt.kind
      of EventType.Key:
        case evt.sym
        of Symbol.Escape:
          return
        of Symbol.Up:
          selected = (selected - 1 + MenuItems.len) mod MenuItems.len
        of Symbol.Down:
          selected = (selected + 1) mod MenuItems.len
        of Symbol.Enter:
          case MenuItem(selected)
          of miListPractices:
            currentScreen = scListPractices
          of miRunPractice:
            currentScreen = scRunPractice
          of miUpdatePractices:
            if updateRecipes():
              showMessage(nb, "Recipes updated successfully")
            else:
              showMessage(nb, "Failed to update recipes")
          of miConfigurePractice:
            currentScreen = scConfigurePractice
          of miExit:
            return
        else:
          discard
      else:
        discard
    of scListPractices:
      listPracticesScreen(nb)
      currentScreen = scMain
    of scRunPractice:
      runPracticeScreen(nb)
      currentScreen = scMain
    of scConfigurePractice:
      configurePracticeScreen(nb)
      currentScreen = scMain

when isMainModule:
  tuiMain()