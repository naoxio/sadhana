import os, strutils, httpclient
import zip/zipfiles

const
  RecipesUrl = "https://github.com/naoxio/sadhana/archive/refs/heads/main.zip"
  RecipesFile = "sadhana-main.zip"
  RecipesDir = getHomeDir() / ".local/share/sadhana"

proc downloadRecipes*(): bool =
  var client = newHttpClient()
  try:
    client.downloadFile(RecipesUrl, RecipesFile)
    result = true
  except:
    echo "Failed to download recipes: ", getCurrentExceptionMsg()
    result = false
  finally:
    client.close()

proc extractRecipes*(): bool =
  try:
    var z: ZipArchive
    if not z.open(RecipesFile):
      echo "Unable to open the zip file"
      return false
    
    createDir(RecipesDir)
    for file in z.walkFiles:
      let destPath = RecipesDir / file.replace("sadhana-main/", "")
      createDir(destPath.parentDir)
      z.extractFile(file, destPath)
    
    z.close()
    removeFile(RecipesFile)  # Clean up the zip file after extraction
    result = true
  except:
    echo "Failed to extract recipes: ", getCurrentExceptionMsg()
    result = false

proc initializeRecipes*(): bool =
  result = downloadRecipes() and extractRecipes()

proc updateRecipes*(): bool =
  result = initializeRecipes()

proc listRecipes*(): seq[string] =
  let practicesDir = RecipesDir / "practices"
  for kind, path in walkDir(practicesDir):
    if kind == pcFile and path.endsWith(".yaml"):
      result.add(path.extractFilename())

proc runPractice*(practiceName: string): bool =
  echo "Running practice: ", practiceName
  # Implement actual practice running logic here
  result = true

proc configurePractice*(practiceName: string): bool =
  echo "Configuring practice: ", practiceName
  # Implement practice configuration logic here
  result = true