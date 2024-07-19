import parsecfg, os

const ConfigDir = getHomeDir() / ".config/sadhana"

proc readConfig*(practiceName: string): Config =
  let configPath = ConfigDir / practiceName & ".ini"
  if fileExists(configPath):
    result = loadConfig(configPath)
  else:
    result = newConfig()

proc writeConfig*(practiceName: string, cfg: Config) =
  createDir(ConfigDir)
  let configPath = ConfigDir / practiceName & ".ini"
  cfg.writeConfig(configPath)

proc getConfigValue*(cfg: Config, section, key: string): string =
  result = cfg.getSectionValue(section, key)

proc setConfigValue*(cfg: var Config, section, key, value: string) =
  cfg.setSectionKey(section, key, value)
