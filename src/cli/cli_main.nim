import os, strutils
import ../core/practice, ../core/config

proc printHelp() =
  echo "Usage: sadhana <command> [args...]"
  echo "Commands:"
  echo "  init               Initialize the recipe hub"
  echo "  update             Update recipes"
  echo "  list               List available practices"
  echo "  run <practice>     Run a specific practice"
  echo "  config <practice>  Configure a specific practice"
  echo "  help               Display this help message"

proc cliMain*() =
  if paramCount() < 1:
    printHelp()
    return

  let command = paramStr(1)

  case command
  of "help":
    printHelp()
  of "init":
    if initializeRecipes():
      echo "Recipes initialized successfully"
    else:
      echo "Failed to initialize recipes"
  of "update":
    if updateRecipes():
      echo "Recipes updated successfully"
    else:
      echo "Failed to update recipes"
  of "list":
    let recipes = listRecipes()
    echo "Available practices:"
    for recipe in recipes:
      echo "  ", recipe
  of "run":
    if paramCount() < 2:
      echo "Usage: sadhana run <practice_name>"
      return
    let practiceName = paramStr(2)
    if runPractice(practiceName):
      echo "Practice completed successfully"
    else:
      echo "Failed to run practice"
  of "config":
    if paramCount() < 2:
      echo "Usage: sadhana config <practice_name>"
      return
    let practiceName = paramStr(2)
    if configurePractice(practiceName):
      echo "Practice configured successfully"
    else:
      echo "Failed to configure practice"
  else:
    echo "Unknown command: ", command
    printHelp()

when isMainModule:
  cliMain()
