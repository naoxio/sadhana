import nigui
import nigui/msgbox
import ../core/practice, ../core/config

proc guiMain*() =
  app.init()
  
  var window = newWindow("Sadhana")
  window.width = 400
  window.height = 300

  var container = newLayoutContainer(Layout_Vertical)
  window.add(container)

  var label = newLabel("Welcome to Sadhana")
  label.fontSize = 20
  container.add(label)

  var btnListPractices = newButton("List Practices")
  var btnRunPractice = newButton("Run Practice")
  var btnUpdatePractices = newButton("Update Practices")
  var btnConfigurePractice = newButton("Configure Practice")
  var btnExit = newButton("Exit")

  container.add(btnListPractices)
  container.add(btnRunPractice)
  container.add(btnUpdatePractices)
  container.add(btnConfigurePractice)
  container.add(btnExit)

  btnListPractices.onClick = proc(event: ClickEvent) =
    let recipes = listRecipes()
    var msg = "Available practices:\n\n"
    for recipe in recipes:
      msg.add("  " & recipe & "\n")
    discard window.msgBox(msg, "Practices")

  btnRunPractice.onClick = proc(event: ClickEvent) =
    let res = window.msgBox("Select a practice to run:", "Run Practice", "Practice 1", "Practice 2", "Cancel")
    case res
    of 0: discard window.msgBox("Running Practice 1", "Run Practice")
    of 1: discard window.msgBox("Running Practice 2", "Run Practice")
    else: discard

  btnUpdatePractices.onClick = proc(event: ClickEvent) =
    if updateRecipes():
      window.alert("Recipes updated successfully")
    else:
      window.alert("Failed to update recipes")

  btnConfigurePractice.onClick = proc(event: ClickEvent) =
    let res = window.msgBox("Select a practice to configure:", "Configure Practice", "Practice 1", "Practice 2", "Cancel")
    case res
    of 0: discard window.msgBox("Configuring Practice 1", "Configure Practice")
    of 1: discard window.msgBox("Configuring Practice 2", "Configure Practice")
    else: discard

  btnExit.onClick = proc(event: ClickEvent) =
    let res = window.msgBox("Are you sure you want to exit?", "Exit", "Yes", "No")
    if res == 0:
      app.quit()  # Use app.quit() to close the application

  window.onCloseClick = proc(event: CloseClickEvent) =
    case window.msgBox("Do you want to quit?", "Quit?", "Quit", "Minimize", "Cancel")
    of 1: window.dispose()
    of 2: window.minimize()
    else: discard

  window.show()
  app.run()

when isMainModule:
  guiMain()