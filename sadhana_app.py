#!/usr/bin/env python3

import os
import argparse
import git
from textual.app import App, ComposeResult
from textual.widgets import Button, Static
from textual.containers import Container
from textual import work

# Import separate scripts
from extra_options_screen import ExtraOptionsScreen
from list_practices import ListPracticesScreen, get_practices
from config_practice import ConfigPracticeScreen
from base_screen import BaseScreen
from cli_handler import handle_cli_command

SADHANA_DIR = os.path.expanduser("~/.local/share/sadhana")
USER_CONFIG_DIR = os.path.expanduser("~/.config/sadhana")
REPO_URL = "https://github.com/naoxio/sadhana.git"

class MainScreen(BaseScreen):
    def compose(self) -> ComposeResult:
        yield from super().compose()
        with Container(id="content"):
            yield Static("Welcome to Sadhana", id="welcome")
            yield Button("Run Practice", id="run")
            yield Button("Update Practices", id="update")
            yield Button("Configure Practice", id="conf")
            yield Button("Extra Options", id="extra")
            yield Button("Exit", id="exit")

class SadhanaApp(App):
    CSS = """
    Screen {
        align: center middle;
    }
    #content {
        width: 50%;
        height: auto;
        align: center middle;
    }
    Button {
        width: 100%;
        margin: 1 0;
    }
    #welcome {
        text-align: center;
        margin-bottom: 1;
    }
    """
    
    def on_mount(self):
        self.push_screen(MainScreen())

    def on_button_pressed(self, event: Button.Pressed) -> None:
        if event.button.id == "list":
            self.action_list()
        elif event.button.id == "run":
            self.action_run()
        elif event.button.id == "update":
            self.action_update()
        elif event.button.id == "conf":
            self.action_conf()
        elif event.button.id == "extra":
            self.push_screen(ExtraOptionsScreen())
        elif event.button.id == "exit":
            self.exit()

    def action_quit(self) -> None:
        self.exit()

    @work(thread=True)
    def action_install(self) -> None:
        if os.path.exists(SADHANA_DIR) and os.path.exists(os.path.join(SADHANA_DIR, ".git")):
            self.notify("Sadhana is already installed.")
            return

        os.makedirs(SADHANA_DIR, exist_ok=True)
        git.Repo.clone_from(REPO_URL, SADHANA_DIR)
        self.notify("Sadhana installed successfully.")

    @work(thread=True)
    def action_run(self) -> None:
        practices = get_practices(SADHANA_DIR)
        if not practices:
            self.notify("No practices found. Please install practices first.")
            return
        self.call_from_thread(self.show_list_screen, practices)

    def show_list_screen(self, practices):
        self.push_screen(ListPracticesScreen(practices))

    @work(thread=True)
    def action_conf(self) -> None:
        practices = get_practices(SADHANA_DIR)
        self.call_from_thread(self.show_conf_screen, practices)

    def show_conf_screen(self, practices):
        self.push_screen(ConfigPracticeScreen(practices))

    @work(thread=True)
    def action_update(self) -> None:
        if not os.path.exists(SADHANA_DIR) or not os.path.exists(os.path.join(SADHANA_DIR, ".git")):
            self.action_install()
        else:
            repo = git.Repo(SADHANA_DIR)
            repo.remotes.origin.pull()
        self.notify("Practices updated successfully.")

def main():
    parser = argparse.ArgumentParser(description="Manage Sadhana practices and installations.")
    parser.add_argument('command', choices=['list', 'run', 'update', 'conf', 'gui', 'extra'],
                        help="Command to execute")
    parser.add_argument('practice', nargs='?', help="Practice name for 'run' and 'conf' commands")
    args = parser.parse_args()

    if args.command == 'gui':
        app = SadhanaApp()
        app.run()
    else:
        handle_cli_command(args.command, args.practice)

if __name__ == "__main__":
    main()