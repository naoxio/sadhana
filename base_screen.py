# base_screen.py

from textual.app import ComposeResult
from textual.screen import Screen
from textual.widgets import Header, Footer

class BaseScreen(Screen):
    BINDINGS = [("q", "quit", "Quit")]

    def compose(self) -> ComposeResult:
        yield Header()
        yield Footer()

    def action_quit(self):
        self.app.exit()